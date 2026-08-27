from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]


def test_required_documentation_exists():
    required = [
        "README.md",
        "SECURITY.md",
        "docs/ARCHITECTURE.md",
        "docs/BUILDING.md",
        "docs/ROADMAP.md",
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
