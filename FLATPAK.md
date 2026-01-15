# Flatpak Packaging for PixelBatch

This document describes the Flatpak packaging for PixelBatch.

## Overview

PixelBatch is packaged as a Flatpak using the KDE runtime for Qt5 support. The Flatpak includes all necessary image optimization tools (jpegoptim, pngquant, gifsicle, and SVGO).

## Files

- `com.ktechpit.pixelbatch.yml` - Main Flatpak manifest
- `flathub.json` - Flathub configuration (architecture restrictions)

## Building Locally

### Prerequisites

```bash
# Install flatpak-builder
sudo apt-get install flatpak-builder

# Add Flathub repository
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

# Install KDE runtime and SDK
flatpak install flathub org.kde.Platform//5.15-25.08 org.kde.Sdk//5.15-25.08
```

### Build and Install

```bash
# Build the Flatpak
flatpak-builder --force-clean --user --install build-dir com.ktechpit.pixelbatch.yml

# Run the application
flatpak run com.ktechpit.pixelbatch
```

### Build Options

```bash
# Build without installing
flatpak-builder --force-clean build-dir com.ktechpit.pixelbatch.yml

# Build and create a repository
flatpak-builder --repo=repo --force-clean build-dir com.ktechpit.pixelbatch.yml

# Install from local repository
flatpak --user remote-add --no-gpg-verify pixelbatch-repo repo
flatpak --user install pixelbatch-repo com.ktechpit.pixelbatch

# Export as a single-file bundle
flatpak build-bundle repo pixelbatch.flatpak com.ktechpit.pixelbatch
```

## Manifest Structure

### Runtime and SDK

- **Runtime**: `org.kde.Platform` version `5.15-25.08`
- **SDK**: `org.kde.Sdk` version `5.15-25.08`

The KDE runtime provides Qt5 libraries and other dependencies needed by PixelBatch.

### Permissions (finish-args)

- `--share=ipc` - X11 shared memory access
- `--socket=x11` - X11 display access
- `--socket=wayland` - Wayland display access
- `--device=dri` - OpenGL/GPU acceleration
- `--filesystem=home` - Access to user's home directory (for image files)
- `--filesystem=/media` and `--filesystem=/run/media` - Removable media access
- `--share=network` - Network access (optional, for future features)

### Bundled Modules

The Flatpak bundles the following optimization tools:

1. **jpegoptim** (v1.5.5)
   - Built from source using autotools
   - For JPEG optimization

2. **pngquant** (v3.0.3)
   - Built from source using autotools
   - For PNG optimization with lossy compression

3. **gifsicle** (v1.95)
   - Built from source using autotools
   - For GIF optimization

4. **Node.js** (v18.19.0)
   - Built from source
   - Required to run SVGO

5. **SVGO** (v4.0.0)
   - Installed via npm
   - For SVG optimization

6. **PixelBatch** (main application)
   - Built using qmake
   - Installed to `/app` prefix

## Maintenance

### Updating Dependencies

To update a dependency version:

1. Update the `url` in the module's `sources` section
2. Update the `sha256` checksum (download the file and run `sha256sum`)
3. Update version numbers in comments

### Testing

After making changes to the manifest:

```bash
# Validate YAML syntax
yamllint com.ktechpit.pixelbatch.yml

# Test build
flatpak-builder --force-clean build-dir com.ktechpit.pixelbatch.yml

# Test run
flatpak-builder --run build-dir com.ktechpit.pixelbatch.yml pixelbatch
```

## Submitting to Flathub

To submit this application to Flathub:

1. Fork the [flathub/flathub](https://github.com/flathub/flathub) repository
2. Create a new repository named `com.ktechpit.pixelbatch`
3. Copy `com.ktechpit.pixelbatch.yml` and `flathub.json` to the new repository
4. Change the source type from `dir` to `archive` or `git` in the pixelbatch module:

   ```yaml
   # For git releases
   - type: git
     url: https://github.com/keshavbhatt/pixelbatch.git
     tag: v1.0.0
     commit: <commit-sha>
   
   # For archive releases
   - type: archive
     url: https://github.com/keshavbhatt/pixelbatch/archive/refs/tags/v1.0.0.tar.gz
     sha256: <checksum>
   ```

5. Submit a pull request to flathub/flathub with the link to your new repository
6. Follow the Flathub submission guidelines and wait for review

## Troubleshooting

### Build Fails

If the build fails, check:

1. All runtime and SDK versions are installed
2. URLs and checksums are correct
3. Build dependencies are specified correctly
4. The qmake project file path is correct

### Application Doesn't Run

If the application doesn't run:

1. Check permissions in `finish-args`
2. Verify the command name matches the binary installed by qmake
3. Check that all required libraries are provided by the runtime

### Optimizer Tools Not Found

If the optimizer tools are not found at runtime:

1. Verify they are built and installed to `/app/bin`
2. Check that PATH includes `/app/bin`
3. Ensure the tools are not in the `cleanup` section

## Resources

- [Flatpak Documentation](https://docs.flatpak.org/)
- [Flathub Guidelines](https://github.com/flathub/flathub/wiki)
- [KDE Runtime Documentation](https://github.com/KDE/flatpak-kde-runtime)
