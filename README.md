# PixelBatch

![License](https://img.shields.io/badge/license-GPL--3.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![Qt](https://img.shields.io/badge/Qt-5%2B-green.svg)

**PixelBatch** is a powerful, user-friendly desktop application for batch image optimization on Linux. Optimize hundreds of images at once while preserving quality, reducing file sizes, and saving disk space.

## 📸 Screenshots

<p align="center">
  <img src="screenshots/PixelBatch-Linux-1.png" alt="PixelBatch Main Interface" width="100%">
  <br>
  <em>Main Interface - Batch image optimization with real-time statistics</em>
</p>

<p align="center">
  <img src="screenshots/PixelBatch-Linux-2.png" alt="PixelBatch Settings" width="100%">
  <br>
  <em>Optimizer Settings - Fine-tune compression for each format</em>
</p>

<details>
<summary><strong>📷 View More Screenshots</strong> (click to expand)</summary>
<br>

<table>
  <tr>
    <td width="50%">
      <a href="screenshots/PixelBatch-Linux-3.png" target="_blank">
        <img src="screenshots/PixelBatch-Linux-3.png" alt="Screenshot 3" width="100%">
      </a>
    </td>
    <td width="50%">
      <a href="screenshots/PixelBatch-Linux-4.png" target="_blank">
        <img src="screenshots/PixelBatch-Linux-4.png" alt="Screenshot 4" width="100%">
      </a>
    </td>
  </tr>
  <tr>
    <td width="50%">
      <a href="screenshots/PixelBatch-Linux-5.png" target="_blank">
        <img src="screenshots/PixelBatch-Linux-5.png" alt="Screenshot 5" width="100%">
      </a>
    </td>
    <td width="50%">
      <a href="screenshots/PixelBatch-Linux-6.png" target="_blank">
        <img src="screenshots/PixelBatch-Linux-6.png" alt="Screenshot 6" width="100%">
      </a>
    </td>
  </tr>
  <tr>
    <td width="50%">
      <a href="screenshots/PixelBatch-Linux-7.png" target="_blank">
        <img src="screenshots/PixelBatch-Linux-7.png" alt="Screenshot 7" width="100%">
      </a>
    </td>
    <td width="50%">
      <a href="screenshots/PixelBatch-Linux-8.png" target="_blank">
        <img src="screenshots/PixelBatch-Linux-8.png" alt="Screenshot 8" width="100%">
      </a>
    </td>
  </tr>
  <tr>
    <td width="50%">
      <a href="screenshots/PixelBatch-Linux-9.png" target="_blank">
        <img src="screenshots/PixelBatch-Linux-9.png" alt="Screenshot 9" width="100%">
      </a>
    </td>
    <td width="50%">
      <a href="screenshots/PixelBatch-Linux-10.png" target="_blank">
        <img src="screenshots/PixelBatch-Linux-10.png" alt="Screenshot 10" width="100%">
      </a>
    </td>
  </tr>
  <tr>
    <td width="50%">
      <a href="screenshots/PixelBatch-Linux-11.png" target="_blank">
        <img src="screenshots/PixelBatch-Linux-11.png" alt="Screenshot 11" width="100%">
      </a>
    </td>
    <td width="50%">
      <a href="screenshots/PixelBatch-Linux-12.png" target="_blank">
        <img src="screenshots/PixelBatch-Linux-12.png" alt="Screenshot 12" width="100%">
      </a>
    </td>
  </tr>
</table>

</details>

## ✨ Features

- **🚀 Batch Processing**: Optimize multiple images simultaneously with configurable concurrent task limits
- **📁 Drag & Drop**: Simply drag images into the application to add them to the queue
- **🖼️ Multiple Formats**: Support for JPEG, PNG, GIF, and SVG images
- **⚡ Parallel Processing**: Process multiple images concurrently for maximum efficiency
- **📊 Real-time Statistics**: See original size, optimized size, and savings for each image
- **🎯 Lossless Optimization**: Reduce file size without quality loss using industry-standard tools
- **⚙️ Configurable**: Customize output paths, file prefixes, and optimization settings
- **📈 Progress Tracking**: Monitor the status of each task (Pending, Processing, Completed, Error)
- **🔍 Quick Actions**: Open optimized images directly from the application
- **💾 Smart Defaults**: Remembers your last used directories and preferences

## 🎯 Use Cases

- **Web Developers**: Optimize images for faster website loading times
- **Photographers**: Reduce storage requirements for image collections
- **Content Creators**: Prepare images for social media and online platforms
- **System Administrators**: Batch process server assets and reduce bandwidth usage
- **Designers**: Optimize graphics while maintaining quality

## 📦 Installation

### From Source

#### Prerequisites

Make sure you have the following dependencies installed:

```bash
# Ubuntu/Debian
sudo apt-get install qt5-default build-essential

# Required optimization tools
sudo apt-get install jpegoptim pngquant gifsicle

# SVGO (requires Node.js)
sudo apt-get install npm
sudo npm install -g svgo

# Note: pngquant 3.0+, gifsicle 1.9+, and SVGO 3.0+ recommended for best results
```

#### Build Instructions

```bash
# Clone the repository
git clone https://github.com/keshavbhatt/PixelBatch.git
cd PixelBatch/src

# Build the application
qmake PixelBatch.pro
make

# Run the application
./pixelbatch
```

#### System-wide Installation

```bash
# Install to /usr/local (default)
sudo make install

# Or install to custom prefix
qmake PREFIX=/opt/pixelbatch PixelBatch.pro
make
sudo make install
```

### Snap Package

```bash
# Install from Snap Store (coming soon)
snap install pixelbatch
```

### Flatpak Package

```bash
# Install from Flathub (coming soon)
flatpak install flathub com.ktechpit.pixelbatch

# Run the application
flatpak run com.ktechpit.pixelbatch
```

#### Building Flatpak Locally

If you want to build and test the Flatpak package locally:

```bash
# Easy way - use the build script
./build-flatpak.sh all

# Manual way
# Install flatpak-builder
sudo apt-get install flatpak-builder

# Add Flathub repository (if not already added)
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

# Install KDE runtime and SDK
flatpak install flathub org.kde.Platform//5.15-23.08 org.kde.Sdk//5.15-23.08

# Build and install the Flatpak
flatpak-builder --force-clean --user --install build-dir com.ktechpit.pixelbatch.yml

# Run the locally built Flatpak
flatpak run com.ktechpit.pixelbatch
```

For more details on Flatpak packaging, see [FLATPAK.md](FLATPAK.md).

## 🚀 Quick Start

### Basic Usage

1. **Launch PixelBatch** from your application menu or terminal
2. **Add Images**:
   - Click "Add Images" button, or
   - Drag and drop image files directly into the window
3. **Configure Settings** (optional):
   - Set output directory for optimized images
   - Configure file naming prefix
   - Adjust concurrent task limit
   - Customize optimizer settings (Edit → Optimizer Settings)
4. **Start Processing**:
   - Click "Process Images" button
   - Watch real-time progress for each image
5. **View Results**:
   - See file size reduction statistics
   - Right-click tasks for quick actions (open, view, remove)
6. **Re-process Images** (optional):
   - Change optimizer settings if needed
   - Click "Re-process Images" to optimize again with new settings
   - Useful for testing different quality/compression levels

### Supported Image Formats

| Format | Extension | Optimizer Used | Compression Type |
|--------|-----------|----------------|------------------|
| JPEG   | `.jpg`, `.jpeg` | jpegoptim | Lossless or Lossy |
| PNG    | `.png` | pngquant | Lossy with quality control |
| GIF    | `.gif` | gifsicle | Lossless or Lossy |
| SVG    | `.svg` | SVGO | Lossless (code optimization) |


### Keyboard Shortcuts

- **Ctrl+O**: Add images
- **Ctrl+Q**: Quit application
- **Delete**: Remove selected task
- **Enter/Return**: Open completed image in viewer
- **Escape**: Deselect current selection
- **Ctrl+A**: Select all tasks
- **F5**: Start processing

## 🔧 Advanced Usage

### Command Line Options

```bash
# Specify images to load on startup
pixelbatch image1.jpg image2.png image3.gif

# Open with specific working directory
pixelbatch --dir /path/to/images
```

### Getting Help

- **Documentation**: See [DEVELOPER_README.md](DEVELOPER_README.md) for technical details
- **Issues**: Report bugs on GitHub Issues
- **Community**: Join our discussion forums

## 🤝 Contributing

We welcome contributions! Here's how you can help:

### Ways to Contribute

- 🐛 **Report Bugs**: Open an issue with detailed reproduction steps
- 💡 **Suggest Features**: Share your ideas for improvements
- 📝 **Improve Documentation**: Fix typos, add examples, clarify instructions
- 🔧 **Submit Code**: Fix bugs or implement new features
- 🌍 **Translations**: Help translate PixelBatch to other languages

### Development Setup

See [DEVELOPER_README.md](DEVELOPER_README.md) for comprehensive development documentation including:
- Project structure and architecture
- Coding conventions and patterns
- Build system details
- How to add new features

### Pull Request Process

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes following our coding conventions
4. Test thoroughly
5. Commit with clear messages (`git commit -m 'Add amazing feature'`)
6. Push to your fork (`git push origin feature/amazing-feature`)
7. Open a Pull Request

## 📋 Requirements

### Runtime Requirements

- **Operating System**: Linux (Ubuntu 18.04+, Debian 10+, or equivalent)
- **Qt**: Version 5.9 or higher
- **Optimization Tools**:
  - jpegoptim (v1.5.0+) - for JPEG optimization
  - pngquant (v2.0+, v3.0+ recommended) - for PNG optimization
  - gifsicle (v1.9+, v1.95+ recommended) - for GIF optimization
  - SVGO (v3.0+, v4.0+ recommended) - for SVG optimization (requires Node.js)

### Build Requirements

- Qt5 development libraries
- C++17 compatible compiler (GCC 7+, Clang 5+)
- qmake build tool
- make

## 📄 License

PixelBatch is licensed under the **GNU General Public License v3.0**.

This means you are free to:
- ✅ Use the software for any purpose
- ✅ Study and modify the source code
- ✅ Share the software with others
- ✅ Distribute modified versions

**See [LICENSE](LICENSE) for full license text.**

## 🙏 Acknowledgments

- **jpegoptim**: Utility to optimize JPEG files
- **pngquant**: Lossy PNG compressor
- **Qt Framework**: Cross-platform application framework
- **Open Source Community**: For continuous inspiration and support

### Links

- 🏠 **Homepage**: [ktechpit.com](https://ktechpit.com)
- 📚 **Documentation**: [DEVELOPER_README.md](DEVELOPER_README.md)
- 🐛 **Bug Reports**: GitHub Issues
- 💬 **Discussions**: GitHub Discussions

---

**Made with ❤️ for the Linux community**

*If you find PixelBatch useful, please consider giving it a ⭐ on GitHub!*

