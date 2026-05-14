# FalconOS

A lightweight operating system designed from scratch for low-end hardware. Perfect for reviving old computers and learning OS development.

## 🎯 Project Goals

- **Lightweight**: Optimized for systems with minimal resources
- **Performance**: Runs smoothly on hardware from 2005-2015 era
- **Educational**: Learn OS development from the ground up
- **Open Source**: GPL v3 licensed, community-driven

## 📋 Current Status

**Progress: ~25-50%**

- ✅ Basic bootloader (Multiboot compatible)
- ✅ Kernel initialization
- ✅ Screen/graphics output
- ✅ CPU interrupts handling
- ✅ Shell/CLI (planned)
- 🔄 Memory management (WIP)
- ⏳ File system (planned)
- ⏳ Addon Manager (planned)

## 🛠️ Requirements

### Windows
- **Clang** - C compiler
- **NASM** - Assembler
- **LLD** - Linker (part of LLVM)
- **QEMU** - Emulator for testing

### Linux/macOS
- `clang` or `gcc`
- `nasm`
- `lld` or `ld`
- `qemu-system-x86_64`

**Installation:**
```bash
# Ubuntu/Debian
sudo apt install clang nasm llvm qemu-system-x86

# macOS (with Homebrew)
brew install clang nasm llvm qemu

# Arch Linux
sudo pacman -S clang nasm llvm qemu
