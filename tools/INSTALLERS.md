# macOS installers

Run from the repository root (requires Python 3 and Apple's command-line tools):

```sh
python3 tools/make_installer.py HexaGuitar --build-dir build/local-release
python3 tools/make_installer.py HexaGuitar Reverb Delay --build-dir build/local-release
python3 tools/make_installer.py --all --build-dir build/local-release
python3 tools/make_installer.py --list
```

AU and VST3 are included by default. To choose formats:

```sh
python3 tools/make_installer.py HexaGuitar --build-dir build/local-release --formats AU VST3 Standalone
```

The helper only packages existing Release binaries. `--build-dir` is mandatory
when creating an installer; `--list` and `--help` do not require it. No builds,
configuration changes, downloads, or local plugin installations are performed.
The GitHub Actions workflow and project CMake files are unchanged.

Packages are written to `build/installers`. Use `--output path/to/name.pkg` to
choose a filename. A multi-plugin invocation creates one combined installer.
Installer destinations are `/Library/Audio/Plug-Ins/Components`,
`/Library/Audio/Plug-Ins/VST3`, and `/Applications/Jonssonic`.

To package only AU from an existing build:

```sh
python3 tools/make_installer.py HexaGuitar --build-dir build/local-release --formats AU
```

The installer version comes from the built bundles. Use `--version 0.2.1` to
override it, or when combining plugins with different versions. Rebuild before
packaging to include source changes; this helper packages the bundles as they
currently exist.

These installers are unsigned and are not notarized. This helper does not add
Developer ID signing or change the existing release pipeline.
