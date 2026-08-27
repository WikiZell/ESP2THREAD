# Security policy

## Sensitive data

The following values are secrets:

- Wi-Fi passwords
- Thread active and pending operational datasets
- Thread network keys, PSKc values and commissioning credentials
- Matter setup codes and device certificates

ESP2THREAD stores runtime credentials only in the device's non-volatile
storage. Documentation and automated tests use synthetic values. Logs must
redact passwords, complete datasets and keys.

Do not attach a flash backup to a public issue. A flash image can contain
network credentials.

## Reporting a vulnerability

Open a private GitHub security advisory for the repository. Do not publish
working credentials or an exploitable operational dataset in a public issue.

