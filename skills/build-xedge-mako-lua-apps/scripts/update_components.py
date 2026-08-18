#!/usr/bin/env python3
"""Validate or refresh vendored Real Time Logic skill components."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import sys
import tempfile
import urllib.error
import urllib.request
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


SKILL_ROOT = Path(__file__).resolve().parents[1]
SOURCE_FILE = SKILL_ROOT / "references" / "upstream-sources.json"
UPSTREAM_DIR = SKILL_ROOT / "references" / "upstream"
MANIFEST_FILE = UPSTREAM_DIR / "manifest.json"
MAX_COMPONENT_BYTES = 16 * 1024 * 1024
MIN_COMPONENT_BYTES = 64
HTML_PREFIX = re.compile(br"\s*(?:<!doctype\s+html\b|<html\b)", re.IGNORECASE)


class ComponentError(RuntimeError):
    pass


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def load_sources() -> tuple[dict[str, Any], list[dict[str, str]]]:
    try:
        config = json.loads(SOURCE_FILE.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ComponentError(f"Cannot read {SOURCE_FILE}: {exc}") from exc

    components = config.get("components")
    if not isinstance(components, list) or not components:
        raise ComponentError("upstream-sources.json has no components")

    names: set[str] = set()
    normalized: list[dict[str, str]] = []
    for item in components:
        if not isinstance(item, dict):
            raise ComponentError("Each component must be an object")
        filename = item.get("filename")
        url = item.get("url")
        purpose = item.get("purpose")
        if not all(isinstance(value, str) and value for value in (filename, url, purpose)):
            raise ComponentError("Each component requires filename, url, and purpose")
        if Path(filename).name != filename or filename in names:
            raise ComponentError(f"Unsafe or duplicate component filename: {filename}")
        if not url.startswith("https://"):
            raise ComponentError(f"Component URL must use HTTPS: {url}")
        names.add(filename)
        normalized.append({"filename": filename, "url": url, "purpose": purpose})
    return config, normalized


def validate_content(filename: str, data: bytes, content_type: str = "") -> str:
    if len(data) < MIN_COMPONENT_BYTES:
        raise ComponentError(f"{filename}: response is empty or too small ({len(data)} bytes)")
    if len(data) > MAX_COMPONENT_BYTES:
        raise ComponentError(f"{filename}: exceeds {MAX_COMPONENT_BYTES} bytes")
    if b"\x00" in data:
        raise ComponentError(f"{filename}: contains NUL bytes")
    if HTML_PREFIX.match(data) or "text/html" in content_type.lower():
        raise ComponentError(f"{filename}: received HTML instead of Markdown")
    try:
        data.decode("utf-8")
    except UnicodeDecodeError as exc:
        raise ComponentError(f"{filename}: is not valid UTF-8: {exc}") from exc
    return sha256(data)


def download(component: dict[str, str], timeout: float) -> bytes:
    request = urllib.request.Request(
        component["url"],
        headers={
            "User-Agent": "BAS-skill-component-updater/1.0",
            "Accept": "text/markdown,text/plain;q=0.9,*/*;q=0.1",
        },
    )
    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            status = getattr(response, "status", None)
            if status != 200:
                raise ComponentError(f"{component['filename']}: HTTP {status}")
            data = response.read(MAX_COMPONENT_BYTES + 1)
            validate_content(
                component["filename"], data, response.headers.get("Content-Type", "")
            )
            return data
    except (urllib.error.URLError, TimeoutError, OSError) as exc:
        raise ComponentError(f"{component['filename']}: download failed: {exc}") from exc


def read_local(component: dict[str, str]) -> bytes:
    path = UPSTREAM_DIR / component["filename"]
    try:
        data = path.read_bytes()
    except OSError as exc:
        raise ComponentError(f"{component['filename']}: cannot read local copy: {exc}") from exc
    validate_content(component["filename"], data)
    return data


def write_json_atomic(path: Path, value: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    handle, temp_name = tempfile.mkstemp(prefix=f".{path.name}.", dir=path.parent)
    try:
        with os.fdopen(handle, "w", encoding="utf-8", newline="\n") as stream:
            json.dump(value, stream, indent=2, ensure_ascii=False)
            stream.write("\n")
        os.replace(temp_name, path)
    except BaseException:
        try:
            os.unlink(temp_name)
        except OSError:
            pass
        raise


def build_manifest(config: dict[str, Any], components: list[dict[str, str]]) -> dict[str, Any]:
    entries = []
    for component in components:
        data = read_local(component)
        entries.append(
            {
                "filename": component["filename"],
                "url": component["url"],
                "purpose": component["purpose"],
                "bytes": len(data),
                "sha256": sha256(data),
            }
        )
    return {
        "schema_version": 1,
        "refreshed_at_utc": datetime.now(timezone.utc).replace(microsecond=0).isoformat(),
        "guidance_source": config.get("guidance_source", {}),
        "components": entries,
    }


def load_manifest() -> dict[str, Any]:
    try:
        return json.loads(MANIFEST_FILE.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ComponentError(f"Cannot read {MANIFEST_FILE}: {exc}") from exc


def check_components(components: list[dict[str, str]], selected: set[str]) -> None:
    manifest = load_manifest()
    entries = {
        item.get("filename"): item
        for item in manifest.get("components", [])
        if isinstance(item, dict)
    }
    for component in components:
        if component["filename"] not in selected:
            continue
        data = read_local(component)
        entry = entries.get(component["filename"])
        if not entry:
            raise ComponentError(f"{component['filename']}: missing from manifest")
        if entry.get("url") != component["url"]:
            raise ComponentError(f"{component['filename']}: manifest URL does not match source list")
        if entry.get("bytes") != len(data) or entry.get("sha256") != sha256(data):
            raise ComponentError(f"{component['filename']}: local copy does not match manifest")
        print(f"OK        {component['filename']} ({len(data)} bytes)")


def refresh_components(
    config: dict[str, Any],
    components: list[dict[str, str]],
    selected: set[str],
    timeout: float,
) -> None:
    UPSTREAM_DIR.mkdir(parents=True, exist_ok=True)
    staged: dict[str, bytes] = {}
    for component in components:
        if component["filename"] in selected:
            staged[component["filename"]] = download(component, timeout)

    for component in components:
        filename = component["filename"]
        path = UPSTREAM_DIR / filename
        if filename not in selected:
            read_local(component)
            continue
        data = staged[filename]
        old_data = path.read_bytes() if path.exists() else None
        if old_data == data:
            print(f"UNCHANGED {filename} ({len(data)} bytes)")
            continue
        handle, temp_name = tempfile.mkstemp(prefix=f".{filename}.", dir=UPSTREAM_DIR)
        try:
            with os.fdopen(handle, "wb") as stream:
                stream.write(data)
            os.replace(temp_name, path)
        except BaseException:
            try:
                os.unlink(temp_name)
            except OSError:
                pass
            raise
        print(f"UPDATED   {filename} ({len(data)} bytes)")

    write_json_atomic(MANIFEST_FILE, build_manifest(config, components))
    print(f"MANIFEST  {MANIFEST_FILE}")


def main() -> int:
    config, components = load_sources()
    names = [component["filename"] for component in components]

    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--check", action="store_true", help="validate vendored files and hashes")
    mode.add_argument("--refresh", action="store_true", help="download and atomically refresh files")
    parser.add_argument(
        "--component",
        action="append",
        choices=names,
        help="operate on one component; repeat as needed (default: all)",
    )
    parser.add_argument("--timeout", type=float, default=30.0, help="download timeout in seconds")
    args = parser.parse_args()

    if args.timeout <= 0:
        parser.error("--timeout must be positive")
    selected = set(args.component or names)

    try:
        if args.refresh:
            refresh_components(config, components, selected, args.timeout)
        else:
            check_components(components, selected)
    except ComponentError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
