# Ray Tracing

GPU-accelerated ray tracer built with C++ using the Walnut application framework.

![Language Distribution](https://img.shields.io/badge/C%2B%2B-94.0%25-orange) ![Lua](https://img.shields.io/badge/Lua-4.8%25-blue) ![Other](https://img.shields.io/badge/Other-1.2%25-lightgrey) ![License](https://img.shields.io/badge/License-MIT-green)

## Overview

A high-performance ray tracing implementation built on the [Walnut App Template](https://github.com/StudioCherno/WalnutAppTemplate), featuring real-time rendering capabilities with plans for CUDA GPU acceleration. This project implements physically-based ray tracing algorithms for rendering realistic 3D scenes with support for materials, lighting, and advanced rendering techniques.

## Features

### Current Features
- 🎨 **Ray Tracing Engine** - Core ray tracing implementation with ray-sphere intersection
- 🖼️ **Walnut Framework** - Modern C++ application framework for windowing and UI
- 🔧 **Premake Build System** - Cross-platform build configuration with Lua scripts
- 📦 **Submodule Architecture** - Walnut framework integrated as a Git submodule
- 🎯 **Material System** - Support for different material types (metals, dielectrics)

### Planned Features
- 🚀 **CUDA GPU Acceleration** - Move ray tracing computation to GPU for massive performance gains
- 🖼️ **Texture Loading** - Support for texture mapping on surfaces
- 🔷 **Extended Shapes** - Additional primitives (cylinders, boxes, triangles, etc.)
- 🔍 **BVH Acceleration** - Bounding Volume Hierarchy for faster ray-object intersection
- 🎨 **Advanced Materials** - Glass, metals, dielectrics with proper refraction
- 📊 **Monte Carlo Path Tracing** - Advanced global illumination techniques
- ⚡ **Performance Optimization** - Profiling and optimization of critical paths

## Project Structure

```
RayTracing/
├── Walnut/              # Walnut framework (Git submodule)
│   └── ...              # Application framework and windowing
├── Halide/              # Halide integration (if used)
│   └── ...              # Image processing library
├── scripts/             # Build and setup scripts
│   └── Setup.bat        # Windows setup script for Visual Studio
├── WalnutApp/           # Main application directory
│   └── src/
│       └── WalnutApp.cpp # Main application entry point
├── premake5.lua         # Premake build configuration
├── .gitignore          # Git ignore rules
├── LICENSE.txt         # MIT License
└── README.md           # This file
```

## Requirements

### Essential
- **Visual Studio 2022** (Windows) or compatible C++ compiler
- **Premake5** - Build system generator ([Download](https://premake.github.io/download))
- **C++17** or later - Modern C++ standard required
- **Git** - For submodule management

### Optional (for GPU acceleration)
- **CUDA Toolkit** 11.0+ - For GPU acceleration ([Download](https://developer.nvidia.com/cuda-downloads))
- **NVIDIA GPU** with CUDA support - Required for CUDA features

## Building

### Windows (Visual Studio)

1. **Clone the repository:**
   ```bash
   git clone https://github.com/AakashR13/RayTracing.git
   cd RayTracing
   ```

2. **Initialize and update submodules:**
   ```bash
   git submodule update --init --recursive
   ```
   This will download the Walnut framework and any other dependencies.

3. **Generate Visual Studio solution:**
   ```bash
   scripts/Setup.bat
   ```
   This runs Premake5 to generate the Visual Studio solution and project files.

4. **Build the project:**
   - Open the generated `WalnutApp.sln` (or similar) in Visual Studio 2022
   - Select your desired configuration (Debug/Release)
   - Build the solution (Ctrl+Shift+B) or press F5 to build and run

### Alternative: Manual Premake

If the setup script doesn't work, you can run Premake manually:

```bash
# Download premake5.exe and place it in your PATH or project root
premake5 vs2022
```

## Usage

### Running the Application

After building, run the executable from Visual Studio or navigate to the output directory:
- **Debug builds:** Typically in `WalnutApp/bin/Debug-windows-x86_64/WalnutApp/`
- **Release builds:** Typically in `WalnutApp/bin/Release-windows-x86_64/WalnutApp/`

### Customization

The main application code is located in `WalnutApp/src/WalnutApp.cpp`. This is where you can:
- Modify scene setup (objects, materials, lighting)
- Adjust rendering parameters
- Add new features to the ray tracer
- Integrate CUDA kernels for GPU acceleration

### Development Workflow

1. **Modify code** in `WalnutApp/src/WalnutApp.cpp`
2. **Rebuild** the project in Visual Studio
3. **Run** to see changes
4. **Iterate** on features and optimizations

## Development Status

🚧 **Active Development** - In Progress

### Current Work Items
- [ ] Separating Walnut submodule
- [ ] Fixing refraction implementation
- [ ] Adding texture loading support
- [ ] Adding additional shapes
- [ ] Implementing CUDA GPU acceleration

### Completed
- ✅ Basic ray tracing setup with Walnut framework
- ✅ Project structure initialized
- ✅ Premake build system configured
- ✅ Ray-sphere intersection implemented
- ✅ Basic material system

## Architecture

### Technology Stack

- **C++ (94.0%)** - Core ray tracing engine and application logic
- **Lua (4.8%)** - Premake build scripts for cross-platform configuration
- **Other (1.2%)** - Configuration files, licenses, documentation

### Key Components

1. **Ray Tracing Engine**
   - Ray generation and intersection tests
   - Material calculations (diffuse, metal, dielectric)
   - Lighting and shading

2. **Walnut Framework**
   - Window management
   - Input handling
   - Rendering context

3. **Build System**
   - Premake5 for project generation
   - Cross-platform support (Windows, Linux, macOS)


## Performance Considerations

- **CPU Ray Tracing** - Current implementation runs on CPU
- **GPU Acceleration** - CUDA integration planned for 10-100x speedup
- **Optimization Targets** - BVH, SIMD, multi-threading

## References

### Learning Resources
- [Ray Tracing in One Weekend](https://raytracing.github.io/) - Excellent introduction to ray tracing
- [Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html) - Advanced techniques
- [Ray Tracing: The Rest of Your Life](https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html) - Monte Carlo methods

### Framework & Tools
- [Walnut Framework](https://github.com/TheCherno/Walnut) - Application framework
- [Walnut App Template](https://github.com/StudioCherno/WalnutAppTemplate) - Template this project is based on
- [Premake](https://premake.github.io/) - Build system generator
- [CUDA Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/) - GPU programming

## Roadmap

### Short Term
- Fix refraction bugs
- Separate Walnut submodule properly
- Basic texture loading

### Medium Term
- CUDA GPU acceleration
- Additional shape primitives
- BVH acceleration structure

### Long Term
- Monte Carlo path tracing
- Advanced material system
- Real-time interactive rendering
- Scene file format support

## License

MIT License - See [LICENSE.txt](LICENSE.txt) for details.

This project is based on the [Walnut App Template](https://github.com/StudioCherno/WalnutAppTemplate), which is also MIT licensed.

---

**Author:** AakashR13  
**Repository:** [https://github.com/AakashR13/RayTracing](https://github.com/AakashR13/RayTracing)  
**Status:** 🚧 Active Development
