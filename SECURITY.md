# Security policy

Report security issues through [GitHub private vulnerability reporting](https://github.com/JerrettDavis/EspScreenOS/security/advisories/new) rather than opening a public issue when credentials, remote control, update integrity, filesystem access, or memory safety are involved.

The provisioning AP is intended for initial local setup. Product firmware should add an explicit setup-presence mechanism, authentication, transport encryption where applicable, credential redaction, and signed OTA policy before deployment outside a trusted environment.

Never expose mutating RPC methods over an unauthenticated network transport. Do not log Wi-Fi passwords, tokens, private keys, or complete authorization headers. Store project secrets in an encrypted or external credential facility appropriate to the deployment threat model.
