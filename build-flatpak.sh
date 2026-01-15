#!/bin/bash
# Flatpak build helper script for PixelBatch
# This script helps build and test the Flatpak locally

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

APP_ID="com.ktechpit.pixelbatch"
MANIFEST="${APP_ID}.yml"
BUILD_DIR="build-dir"
REPO_DIR="repo"

# Print colored message
print_msg() {
    local color=$1
    shift
    echo -e "${color}$@${NC}"
}

# Check if flatpak-builder is installed
check_requirements() {
    print_msg "$BLUE" "Checking requirements..."
    
    if ! command -v flatpak-builder &> /dev/null; then
        print_msg "$RED" "Error: flatpak-builder is not installed"
        print_msg "$YELLOW" "Install it with: sudo apt-get install flatpak-builder"
        exit 1
    fi
    
    if ! command -v flatpak &> /dev/null; then
        print_msg "$RED" "Error: flatpak is not installed"
        print_msg "$YELLOW" "Install it with: sudo apt-get install flatpak"
        exit 1
    fi
    
    print_msg "$GREEN" "✓ Requirements satisfied"
}

# Install runtime and SDK
install_runtime() {
    print_msg "$BLUE" "Checking runtime and SDK..."
    
    if ! flatpak list | grep -q "org.kde.Platform.*5.15-23.08"; then
        print_msg "$YELLOW" "Installing KDE Platform runtime..."
        flatpak install -y flathub org.kde.Platform//5.15-23.08
    else
        print_msg "$GREEN" "✓ KDE Platform runtime already installed"
    fi
    
    if ! flatpak list | grep -q "org.kde.Sdk.*5.15-23.08"; then
        print_msg "$YELLOW" "Installing KDE SDK..."
        flatpak install -y flathub org.kde.Sdk//5.15-23.08
    else
        print_msg "$GREEN" "✓ KDE SDK already installed"
    fi
}

# Clean build directory
clean_build() {
    print_msg "$BLUE" "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    rm -rf "$REPO_DIR"
    rm -f "${APP_ID}.flatpak"
    print_msg "$GREEN" "✓ Build directory cleaned"
}

# Build the Flatpak
build_flatpak() {
    print_msg "$BLUE" "Building Flatpak..."
    flatpak-builder --force-clean "$BUILD_DIR" "$MANIFEST"
    print_msg "$GREEN" "✓ Build completed successfully"
}

# Install the Flatpak
install_flatpak() {
    print_msg "$BLUE" "Installing Flatpak..."
    flatpak-builder --user --install --force-clean "$BUILD_DIR" "$MANIFEST"
    print_msg "$GREEN" "✓ Flatpak installed"
}

# Run the Flatpak
run_flatpak() {
    print_msg "$BLUE" "Running Flatpak..."
    flatpak run "$APP_ID"
}

# Create repository and bundle
create_bundle() {
    print_msg "$BLUE" "Creating Flatpak bundle..."
    flatpak-builder --repo="$REPO_DIR" --force-clean "$BUILD_DIR" "$MANIFEST"
    flatpak build-bundle "$REPO_DIR" "${APP_ID}.flatpak" "$APP_ID"
    print_msg "$GREEN" "✓ Bundle created: ${APP_ID}.flatpak"
}

# Show help
show_help() {
    cat << EOF
Flatpak build helper script for PixelBatch

Usage: $0 [COMMAND]

Commands:
    check       Check requirements (flatpak, flatpak-builder)
    runtime     Install KDE runtime and SDK
    clean       Clean build directory
    build       Build the Flatpak (without installing)
    install     Build and install the Flatpak
    run         Run the installed Flatpak
    bundle      Create a single-file Flatpak bundle
    all         Run all steps (check, runtime, clean, install)
    help        Show this help message

Examples:
    $0 all           # Complete build and install
    $0 install       # Quick build and install
    $0 run           # Run the installed application
    $0 bundle        # Create a distributable bundle

EOF
}

# Main script
case "${1:-help}" in
    check)
        check_requirements
        ;;
    runtime)
        check_requirements
        install_runtime
        ;;
    clean)
        clean_build
        ;;
    build)
        check_requirements
        build_flatpak
        ;;
    install)
        check_requirements
        install_flatpak
        ;;
    run)
        run_flatpak
        ;;
    bundle)
        check_requirements
        create_bundle
        ;;
    all)
        check_requirements
        install_runtime
        clean_build
        install_flatpak
        print_msg "$GREEN" "✓ All steps completed successfully!"
        print_msg "$BLUE" "You can now run the application with: flatpak run $APP_ID"
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        print_msg "$RED" "Unknown command: $1"
        show_help
        exit 1
        ;;
esac
