from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]


def test_required_documentation_exists():
    required = [
        "README.md",
        "SECURITY.md",
        "docs/ARCHITECTURE.md",
        "docs/BUILDING.md",
        "docs/LED_STATUS.md",
        "docs/ROADMAP.md",
        "docs/SETUP.md",
        "docs/TEST_PLAN.md",
        "docs/TEST_RESULTS.md",
        "pytest.ini",
    ]
    assert all((ROOT / path).is_file() for path in required)


def test_documentation_has_no_placeholder_secrets():
    forbidden = [
        re.compile(r"CONFIG_EXAMPLE_WIFI_PASSWORD\s*=\s*\".+\""),
        re.compile(r"(?i)wifi_password\s*[:=]\s*['\"][^'\"]+['\"]"),
        re.compile(r"(?i)networkkey\s*[:=]\s*['\"][0-9a-f]{32}['\"]"),
    ]
    for path in [ROOT / "README.md", *sorted((ROOT / "docs").glob("*.md"))]:
        text = path.read_text(encoding="utf-8")
        assert not any(pattern.search(text) for pattern in forbidden), path


def test_internal_markdown_links_resolve():
    markdown_files = [ROOT / "README.md", *sorted((ROOT / "docs").glob("*.md"))]
    link_pattern = re.compile(r"\[[^]]+\]\(([^)]+)\)")
    for path in markdown_files:
        for target in link_pattern.findall(path.read_text(encoding="utf-8")):
            if "://" in target or target.startswith("#"):
                continue
            target_path = target.split("#", 1)[0]
            assert (path.parent / target_path).resolve().exists(), (path, target)


def test_firmware_targets_native_esp32c6_radio():
    config = (ROOT / "sdkconfig.defaults").read_text(encoding="utf-8")
    assert 'CONFIG_IDF_TARGET="esp32c6"' in config
    assert "CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y" in config
    assert "CONFIG_OPENTHREAD_RADIO_NATIVE=y" in config
    assert "CONFIG_ESP_COEX_SW_COEXIST_ENABLE=y" in config
    assert "CONFIG_OPENTHREAD_BR_START_WEB=y" in config
    assert "CONFIG_OPENTHREAD_BR_SOFTAP_SETUP=y" in config


def test_partition_table_fits_four_megabytes():
    text = (ROOT / "partitions.csv").read_text(encoding="utf-8")
    assert "ota_0" in text and "ota_1" in text
    assert "web_storage" in text
    # Last partition ends at 0x400000, the exact end of the detected 4 MB flash.
    assert "0x3a0000, 384K" in text


def test_no_build_time_wifi_credentials():
    config = (ROOT / "sdkconfig.defaults").read_text(encoding="utf-8")
    assert "CONFIG_EXAMPLE_WIFI_SSID=" not in config
    assert "CONFIG_EXAMPLE_WIFI_PASSWORD=" not in config


def test_setup_portal_uses_esp2thread_branding():
    frontend = (
        ROOT
        / "components"
        / "esp_ot_br_server"
        / "frontend"
    )
    portal = (frontend / "wifi_configuration.html").read_text(encoding="utf-8")
    assert "<title>ESP2THREAD - Wi-Fi Setup</title>" in portal
    assert "<h1>ESP2THREAD</h1>" in portal
    assert "M5Stack Thread Border Router" not in portal
    assert {
        "index.html",
        "network.html",
        "topology.html",
        "commission.html",
        "addresses.html",
        "tools.html",
        "about.html",
        "wifi_configuration.html",
    } <= {path.name for path in frontend.glob("*.html")}
    assert (frontend / "static" / "api.js").is_file()
    assert (frontend / "static" / "style.css").is_file()


def test_fresh_router_requires_explicit_thread_role_selection():
    router = (
        ROOT
        / "components"
        / "thread_border_router"
        / "src"
        / "border_router_launch.c"
    ).read_text(encoding="utf-8")
    setup = (
        ROOT
        / "components"
        / "esp_ot_br_server"
        / "frontend"
        / "setup.html"
    ).read_text(encoding="utf-8")

    assert "otDatasetCreateNewNetwork" not in router
    assert "waiting for explicit Create or Join selection" in router
    assert "Create new Thread network" in setup
    assert "Join existing Thread network" in setup
    assert "Content-Type': 'text/plain'" in setup
    assert "window.crypto.getRandomValues" in setup
    assert "Copy dataset for another router" in setup
    assert "sessionStorage.setItem" not in setup
    assert "Automatic clipboard access is unavailable on local HTTP" in setup


def test_management_ui_has_no_shared_example_thread_key():
    frontend = ROOT / "components" / "esp_ot_br_server" / "frontend"
    example_key = "00112233445566778899aabbccddeeff"
    for page in frontend.glob("*.html"):
        assert example_key not in page.read_text(encoding="utf-8"), page


def test_xiao_status_led_is_active_low_gpio15():
    source = (ROOT / "main" / "status_led.c").read_text(encoding="utf-8")
    assert "#define STATUS_LED_GPIO GPIO_NUM_15" in source
    assert "#define STATUS_LED_ON_LEVEL 0" in source
    assert "#define STATUS_LED_OFF_LEVEL 1" in source
    assert "LED_STATUS_WAITING_SETUP" in source
    assert "LED_STATUS_WIFI_DISCONNECTED" in source
    assert "LED_STATUS_THREAD_SELECTION" in source
    assert "LED_STATUS_THREAD_FORMING" in source
    assert "LED_STATUS_READY" in source
