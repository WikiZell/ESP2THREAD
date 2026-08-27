from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]


def test_required_documentation_exists():
    required = [
        "README.md",
        "SECURITY.md",
        "docs/ARCHITECTURE.md",
        "docs/BUILDING.md",
        "docs/DEVICES.md",
        "docs/HOME_ASSISTANT.md",
        "docs/LED_STATUS.md",
        "docs/ROADMAP.md",
        "docs/SETUP.md",
        "docs/TEST_PLAN.md",
        "docs/TEST_RESULTS.md",
        "docs/UPDATING.md",
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


def test_each_router_uses_a_mac_derived_identity():
    identity = (ROOT / "components" / "device_identity" / "device_identity.c").read_text(encoding="utf-8")
    launcher = (
        ROOT / "components" / "thread_border_router" / "src" / "border_router_launch.c"
    ).read_text(encoding="utf-8")
    main = (ROOT / "main" / "esp2thread_main.c").read_text(encoding="utf-8")

    assert "esp_read_mac(mac, ESP_MAC_BASE)" in identity
    assert '"esp2thread-%02x%02x%02x"' in identity
    assert "mdns_hostname_set(s_hostname)" in identity
    assert "mdns_instance_name_set(s_instance_name)" in identity
    assert "esp2thread_device_identity_apply_netif(get_example_netif())" in launcher
    assert 'ESP2THREAD_HOSTNAME "esp2thread"' not in main


def test_factory_reset_requires_physical_eight_second_hold():
    source = (ROOT / "main" / "factory_reset.c").read_text(encoding="utf-8")

    assert "#define FACTORY_RESET_GPIO GPIO_NUM_9" in source
    assert "#define FACTORY_RESET_ACTIVE_LEVEL 0" in source
    assert "#define FACTORY_RESET_HOLD_MS 8000" in source
    assert "gpio_get_level(FACTORY_RESET_GPIO)" in source
    assert "nvs_flash_deinit()" in source
    assert "nvs_flash_erase()" in source
    assert "FACTORY_RESET_RTC_MAGIC" in source
    assert "esp_restart()" in source


def test_release_version_and_redacted_support_bundle():
    cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
    about = (
        ROOT / "components" / "esp_ot_br_server" / "frontend" / "about.html"
    ).read_text(encoding="utf-8")
    dashboard = (
        ROOT / "components" / "esp_ot_br_server" / "frontend" / "index.html"
    ).read_text(encoding="utf-8")

    assert 'set(PROJECT_VER "0.1.0")' in cmake
    assert "copyRedactedDiagnostics" in about
    assert "Thread datasets, keys, PSKc" in about
    assert "OpenThread:PSKc" not in dashboard
    assert "p-pskc" not in dashboard


def test_flashing_guide_matches_release_layout_and_preserves_nvs():
    guide = (ROOT / "docs" / "UPDATING.md").read_text(encoding="utf-8")

    assert "ESP2THREAD-factory.bin" in guide
    assert "0x0 ESP2THREAD-factory.bin" in guide
    assert "0x0 bootloader.bin" in guide
    assert "0x8000 partition-table.bin" in guide
    assert "0xf000 ota-data-initial.bin" in guide
    assert "0x20000 esp2thread.bin" in guide
    assert "0x3a0000 web-storage.bin" in guide
    assert "0x9000` through `0xefff" in guide
    assert "Never add `erase_flash`" in guide
    assert "No Home Assistant restart is required" in guide
