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

## Local management boundary

The current Espressif-compatible REST service uses local HTTP without user
authentication. Anyone who can reach its management address may be able to
read or replace the Thread dataset. During development, connect ESP2THREAD only
to a trusted home LAN and do not expose port 80 to the internet or an untrusted
VLAN. Authenticated management or an equivalent protected transfer mechanism
is a stable-release requirement.

The setup page exports a dataset only after the local user selects **Copy
dataset for another router**. Join accepts it in an HTTP request body; the
dataset is never placed in a URL, browser storage, firmware log or Git file.

## Reporting a vulnerability

Open a private GitHub security advisory for the repository. Do not publish
working credentials or an exploitable operational dataset in a public issue.
