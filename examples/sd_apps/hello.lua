APP_ID = "sd-hello"
APP_TITLE = "SD Hello"

local message
function greet()
  ui.set_text(message, "Hello from a sideloaded Lua app!")
end

function build()
  message = ui.label("Loaded from /sd/apps/hello.lua")
  ui.button("Say hello", "greet")
end
