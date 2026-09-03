#include "espscreen/runtime/firmware_provisioning_server.hpp"
#include "espscreen/runtime/provisioning_http_policy.hpp"

#include "esp_http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <array>

namespace espscreen::runtime {

struct ProvisioningHttpCallbacks final {
    static FirmwareProvisioningServer* self(httpd_req_t* request) {
        return request
            ? static_cast<FirmwareProvisioningServer*>(request->user_ctx)
            : nullptr;
    }
    static int serve_setup(httpd_req_t* request) {
        auto* server = self(request);
        return server ? server->serve_setup(request) : ESP_FAIL;
    }
    static int serve_health(httpd_req_t* request) {
        auto* server = self(request);
        return server ? server->serve_health(request) : ESP_FAIL;
    }
    static int save_wifi(httpd_req_t* request) {
        auto* server = self(request);
        return server ? server->save_wifi(request) : ESP_FAIL;
    }
};

namespace {

constexpr char setup_page[] =
    "<!doctype html><meta name=viewport content='width=device-width'>"
    "<title>EspScreenOS</title><style>body{font:16px system-ui;max-width:32rem;"
    "margin:3rem auto;padding:1rem}input,button{font:inherit;width:100%;padding:.8rem;"
    "margin:.4rem 0;box-sizing:border-box}</style><h1>EspScreenOS setup</h1>"
    "<form method=post action=/wifi><input name=ssid placeholder='Wi-Fi name' required>"
    "<input name=password type=password placeholder='Password'>"
    "<button>Save and restart</button></form>";

} // namespace

core::Status FirmwareProvisioningServer::start(
    services::IWifiProvisioner& provisioner) {
    const auto transition = lifecycle_.begin_start("provisioning server already active");
    if (!transition.status) return transition.status;
    if (!transition.acquire)
        return provisioner_.load() == &provisioner
                   ? core::Status::success()
                   : core::Status{core::ErrorCode::conflict,
                                  "provisioning server already active"};
    provisioner_.store(&provisioner);

    httpd_config_t configuration = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = nullptr;
    if (httpd_start(&server, &configuration) != ESP_OK) {
        stop();
        return {core::ErrorCode::unavailable, "provisioning server startup failed"};
    }
    server_.store(server);

    const std::array routes{
        httpd_uri_t{.uri = "/", .method = HTTP_GET,
                    .handler = &ProvisioningHttpCallbacks::serve_setup,
                    .user_ctx = this},
        httpd_uri_t{.uri = "/healthz", .method = HTTP_GET,
                    .handler = &ProvisioningHttpCallbacks::serve_health,
                    .user_ctx = this},
        httpd_uri_t{.uri = "/wifi", .method = HTTP_POST,
                    .handler = &ProvisioningHttpCallbacks::save_wifi,
                    .user_ctx = this},
    };
    for (const auto& route : routes) {
        if (httpd_register_uri_handler(server, &route) != ESP_OK) {
            stop();
            return {core::ErrorCode::io_error, "provisioning route registration failed"};
        }
    }
    lifecycle_.started();
    return core::Status::success();
}

void FirmwareProvisioningServer::stop() {
    if (!lifecycle_.begin_stop()) return;
    provisioner_.store(nullptr);
    if (auto* server = server_.exchange(nullptr))
        (void)httpd_stop(static_cast<httpd_handle_t>(server));
    lifecycle_.stopped();
}

int FirmwareProvisioningServer::serve_setup(void* opaque_request) {
    auto* request = static_cast<httpd_req_t*>(opaque_request);
    httpd_resp_set_type(request, "text/html");
    return httpd_resp_send(request, setup_page, HTTPD_RESP_USE_STRLEN);
}

int FirmwareProvisioningServer::serve_health(void* opaque_request) {
    auto* request = static_cast<httpd_req_t*>(opaque_request);
    httpd_resp_set_type(request, "application/json");
    return httpd_resp_sendstr(request, "{\"status\":\"ok\"}");
}

int FirmwareProvisioningServer::save_wifi(void* opaque_request) {
    auto* request = static_cast<httpd_req_t*>(opaque_request);
    if (!request || !lifecycle_.running()) return ESP_FAIL;
    auto* provisioner = provisioner_.load();
    if (!provisioner) return ESP_FAIL;

    char body[maximum_provisioning_form_bytes]{};
    if (request->content_len > sizeof(body)) {
        httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, "Wi-Fi form is too large");
        return ESP_FAIL;
    }
    const int received = httpd_req_recv(request, body, request->content_len);
    if (received <= 0) return ESP_FAIL;

    const auto saved = provisioner->provision_wifi(
        std::string_view(body, static_cast<std::size_t>(received)));
    if (!saved) {
        const auto client_error =
            provisioning_http_result(saved) == ProvisioningHttpResult::bad_request;
        httpd_resp_send_err(request,
                            client_error ? HTTPD_400_BAD_REQUEST
                                         : HTTPD_500_INTERNAL_SERVER_ERROR,
                            client_error ? "Invalid Wi-Fi credentials"
                                         : "Unable to save Wi-Fi credentials");
        return ESP_FAIL;
    }

    httpd_resp_sendstr(request, "Saved. The device is restarting.");
    vTaskDelay(pdMS_TO_TICKS(300));
    restart_.restart();
    return ESP_OK;
}

} // namespace espscreen::runtime
