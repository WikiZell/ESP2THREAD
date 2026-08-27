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

