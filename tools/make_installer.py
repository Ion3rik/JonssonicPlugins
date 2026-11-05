#!/usr/bin/env python3
"""Package existing Jonssonic plugin builds into a macOS installer."""

import argparse
import hashlib
import plistlib
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
FORMATS = {
    "AU": (".component", "Library/Audio/Plug-Ins/Components"),
    "VST3": (".vst3", "Library/Audio/Plug-Ins/VST3"),
    "Standalone": (".app", "Applications/Jonssonic"),
}


def run(*args):
    subprocess.run([str(arg) for arg in args], check=True)


def main():
    plugins = {}
    for cmake in sorted((ROOT / "plugins").glob("*/CMakeLists.txt")):
        if cmake.parent.name.startswith(("_", ".")):
            continue
        match = re.search(r"add_plugin\(\s*([\w-]+)", cmake.read_text())
        if match:
            plugins[match.group(1)] = cmake.parent.name

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("plugins", nargs="*", help="Plugin targets, e.g. HexaAmp Reverb")
    parser.add_argument("--all", action="store_true", help="Include all plugins")
    parser.add_argument("--list", action="store_true", help="List available plugins")
    parser.add_argument("--formats", nargs="+", choices=FORMATS, default=["AU", "VST3"])
    parser.add_argument("--build-dir", type=Path, help="Required: directory containing existing Release builds")
    parser.add_argument("--version", help="Override the installer version")
    parser.add_argument("--output", type=Path, help="Output .pkg filename")
    args = parser.parse_args()
    if args.list:
        print("\n".join(plugins))
        return
    if args.build_dir is None:
        parser.error("--build-dir is required when creating an installer")
    selected = sorted(plugins) if args.all else sorted(set(args.plugins))
    if not selected or (args.all and args.plugins):
        parser.error("Specify plugin names or --all (use --list to see names)")
    unknown = set(selected) - plugins.keys()
    if unknown:
        parser.error("Unknown plugins: " + ", ".join(sorted(unknown)))
    if sys.platform != "darwin":
        parser.error("Installer creation requires macOS")
    for command in ("pkgbuild", "ditto"):
        if not shutil.which(command):
            parser.error(f"Required command not found: {command}")

    build = args.build_dir.resolve()
    formats = list(dict.fromkeys(args.formats))
    if not build.is_dir():
        parser.error(f"Build directory does not exist: {build}")

    bundles = []
    versions = set()
    for plugin in selected:
        for fmt in formats:
            suffix, destination = FORMATS[fmt]
            folder = build / "plugins" / plugins[plugin] / f"{plugin}_artefacts/Release" / fmt
            matches = list(folder.glob(f"*{suffix}"))
            if len(matches) != 1:
                raise RuntimeError(f"Expected one {fmt} bundle in {folder}; found {len(matches)}")
            bundle = matches[0]
            with (bundle / "Contents/Info.plist").open("rb") as stream:
                info = plistlib.load(stream)
            versions.add(info["CFBundleShortVersionString"])
            bundles.append((bundle, destination))
    if not args.version and len(versions) != 1:
        parser.error("Selected plugins have different versions; specify --version")
    version = args.version or versions.pop()
    if not re.fullmatch(r"\d+(?:\.\d+){0,2}", version):
        parser.error("Installer version must be numeric, e.g. 0.2.1")
    label = selected[0] if len(selected) == 1 else "JonssonicPlugins"
    output = (args.output or ROOT / "build/installers" / f"{label}-{version}.pkg").resolve()
    if output.suffix != ".pkg":
        parser.error("--output must end in .pkg")
    output.parent.mkdir(parents=True, exist_ok=True)
    identity = "\n".join(selected + sorted(formats))
    package_id = "com.jonssonic.installer." + hashlib.sha256(identity.encode()).hexdigest()[:16]
    with tempfile.TemporaryDirectory(prefix="jonssonic-installer-") as temporary:
        staging = Path(temporary) / "root"
        for bundle, destination in bundles:
            target = staging / destination / bundle.name
            if target.exists():
                raise RuntimeError(f"Duplicate installer destination: {target}")
            target.parent.mkdir(parents=True, exist_ok=True)
            run("ditto", bundle, target)
        component_plist = Path(temporary) / "components.plist"
        run("pkgbuild", "--analyze", "--root", staging, component_plist)
        with component_plist.open("rb") as stream:
            components = plistlib.load(stream)
        for component in components:
            component["BundleIsRelocatable"] = False
        with component_plist.open("wb") as stream:
            plistlib.dump(components, stream)
        run("pkgbuild", "--root", staging, "--component-plist", component_plist,
            "--identifier", package_id, "--version", version,
            "--install-location", "/", output)
    print(f"\nCreated: {output}\nUnsigned installer; no plugins were installed locally.")


if __name__ == "__main__":
    try:
        main()
    except (RuntimeError, OSError, subprocess.CalledProcessError) as error:
        print(f"Error: {error}", file=sys.stderr)
        sys.exit(1)
