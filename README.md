# 🔥 Hellfire Game Engine

A lightweight, experimental 3D game engine built with modern C++ and OpenGL, featuring an integrated editor and entity-component architecture.

![Build Status](https://img.shields.io/badge/build-experimental-orange)
![Platform](https://img.shields.io/badge/platform-Windows-blue)
![OpenGL](https://img.shields.io/badge/OpenGL-4.3%2B-green)
![License](https://img.shields.io/badge/license-MIT-brightgreen)

## Overview

Hellfire is a personal game engine project focused on providing a solid foundation for 3D game development. Built from the ground up with OpenGL, it features modern rendering capabilities, flexible model importing, and an intuitive Dear ImGui-based editor interface. The engine is designed with an entity-component pattern for flexible game object management.

## ✨ Features

### Core Engine
- **3D Rendering Pipeline** - Modern OpenGL 4.3+ based renderer with custom shader support
- **Entity-Component System** - Flexible composition-based architecture for game object management
- **Hierarchical Scene Graph** - Parent-child entity relationships with transform inheritance
- **Advanced Material System**
    - Standard and PBR materials
    - Custom shader support (e.g., day/night transitions, animated surfaces)
    - UV tiling and opacity controls
    - Multi-texture support for complex effects
- **Geometry Generation** - Built-in primitives (spheres, cubes, planes) with customizable tessellation
- **Instanced Rendering** - Efficient rendering of thousands of objects (demonstrated with asteroid belts)
- **Model Importing** - Support for OBJ, FBX, and glTF formats via Assimp
- **Lighting System**
    - Directional lighting
    - Spot lighting
    - (Shadows coming soon)
- **Skybox Support** - Cubemap-based environment rendering
- **Camera System** - Perspective and orthographic cameras with flexible controls
- **C++ Scripting** - Native C++ component-based scripts (e.g., orbit controllers, player movement)

### Editor
- **Dear ImGui Interface** - Modern, customizable editor UI
- **Visual Editing Tools** - Powered by ImGuizmo for intuitive object manipulation
- **Real-time Preview** - See changes instantly in the viewport

> ⚠️ **Note**: The editor is in early development and doesn't yet expose all engine features through the UI.

## 🚀 Getting Started

### Prerequisites

- **Operating System**: Windows (Linux support may work with modifications)
- **OpenGL**: Version 4.3 or higher
- **RAM**: Minimum 8GB
- **Compiler**: MSVC (Visual Studio 2019 or later recommended)
- **CMake**: Version 3.14 or higher

### Dependencies

The following libraries are used and will be needed for building:

- [GLFW](https://www.glfw.org/) - Window management and input
- [Dear ImGui](https://github.com/ocornut/imgui) - Editor interface
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) - 3D manipulation gizmos
- [Assimp](https://github.com/assimp/assimp) - 3D model importing
- [stb_image](https://github.com/nothings/stb) - Image loading
- OpenGL - Graphics API

### Building from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/hellfire.git
cd hellfire

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -G "Visual Studio 16 2019" -A x64

# Build the project
cmake --build . --config Release
```

> **Note**: Currently, the project only compiles with MSVC due to template implementation specifics. GCC support is not available at this time.

### Quick Start

```cpp
// Example: Creating a simple scene with a rotating sphere
#include "hellfire/scene/Scene.h"
#include "hellfire/graphics/Geometry/Sphere.h"
#include "hellfire/graphics/lighting/DirectionalLight.h"

hellfire::Scene* create_scene() {
    auto* scene = new hellfire::Scene("My Scene");
    
    // Create a material
    auto material = hellfire::MaterialBuilder::create("Sphere Material");
    material->set_texture("textures/earth.jpg", hellfire::TextureType::DIFFUSE);
    
    // Create a sphere entity
    hellfire::EntityID sphere_id = hellfire::Sphere::create(scene, "Earth", {
        .material = material,
        .position = glm::vec3(0, 0, 0),
        .scale = glm::vec3(5.0f),
        .rings = 64,
        .sectors = 64
    });
    
    // Add a directional light
    hellfire::EntityID light_id = hellfire::DirectionalLight::create(scene, "Sun Light");
    
    // Add a custom component for rotation
    hellfire::Entity* sphere = scene->get_entity(sphere_id);
    sphere->add_component<RotationScript>(0.5f);
    
    return scene;
}
```

## 🏗️ Architecture

### Entity-Component System (ECS)

Hellfire uses a **composition-over-inheritance** approach for its entity system. This design decision was made to:

- **Decouple Logic**: Components are independent, making the system more maintainable and testable
- **Improve Flexibility**: Entities can be composed of any combination of components at runtime
- **Simplify Serialization**: Avoiding complex inheritance hierarchies eliminates issues with dynamic casting during deserialization
- **Enable Data-Driven Design**: Scenes can be easily constructed from configuration files

```cpp
// Example of the composition pattern in action
hellfire::Entity* planet = scene->get_entity(earth_id);
planet->add_component<OrbitController>();        // Add orbital movement
planet->add_component<RotationScript>();         // Add rotation
planet->add_component<AtmosphereRenderer>();     // Add atmospheric effects
// Components work independently and can be mixed and matched
```

### Scene Graph

The engine supports hierarchical scene organization with parent-child relationships:
- Transform inheritance (children follow parent transforms)
- Efficient culling through spatial partitioning
- Logical grouping of related entities

## 📁 Project Structure

```
Hellfire/
├── Editor/          # Editor application and UI
├── Engine/          # Core engine systems
│   ├── Rendering/   # OpenGL renderer, shaders, lighting
│   ├── Scene/       # Entity-component system, scene management
│   ├── Assets/      # Model loading, texture management
│   └── Core/        # Core systems and utilities
├── Tests/           # Unit and integration tests
└── Docs/           # Documentation (generated by Doxygen)
```

## 🎯 Roadmap

### In Progress
- [ ] Shadow mapping for directional and spot lights
- [ ] Physics integration (considering Bullet or PhysX)
- [ ] Editor feature parity with engine capabilities

### Planned Features
- [ ] Point lights and area lights
- [ ] Post-processing effects pipeline
- [ ] Audio system integration
- [ ] Hot-reload scripting system
- [ ] Scene serialization
- [ ] Linux and macOS support
- [ ] Vulkan renderer backend

### Future Considerations
- [ ] Network multiplayer support
- [ ] Visual scripting system
- [ ] Asset pipeline and packaging
- [ ] Performance profiling tools

## 🎮 Target Use Case

Hellfire is being developed with the goal of creating simple 3D games, with an initial focus on first-person shooter mechanics. While currently experimental, the engine aims to provide a solid foundation for small to medium-sized 3D game projects.

## 🌟 Showcase

### Solar System Demo
The engine's capabilities are demonstrated through a fully interactive solar system simulation featuring:
- Realistic planetary scales and orbits
- Custom shaders for atmospheric effects (day/night transitions)
- Instanced rendering for asteroid belts (250+ asteroids)
- Hierarchical transforms (moons orbiting planets)
- Dynamic material system with multi-texturing
- Skybox with space environment

This demo showcases the engine's ability to handle:
- Complex scene hierarchies
- Multiple rendering techniques simultaneously
- Custom shader integration
- Large numbers of objects through instancing

## 🤝 Contributing

While Hellfire is a personal project, contributions are welcome! Whether it's bug fixes, new features, or documentation improvements, feel free to:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

Please ensure your code follows the existing style and includes appropriate documentation.

## 📚 Documentation

API documentation is generated using Doxygen. To build the documentation:

```bash
cmake --build . --target project_docs
```

The generated documentation will be available in `build/docs/html/index.html`.

## ⚠️ Known Limitations

- **Platform Support**: Currently Windows-only, Linux support requires modifications
- **Compiler Support**: Only compiles with MSVC due to template implementation details
- **Editor Features**: Not all engine features are accessible through the editor UI yet
- **Scripting**: C++ scripting requires recompilation (no hot-reload yet)
- **Shadows**: Not yet implemented

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- The open-source community for the amazing libraries that make this project possible
- [LearnOpenGL](https://learnopengl.com/) for excellent OpenGL learning resources
- All contributors who help improve Hellfire

---

**Project Status**: 🚧 Under active development - expect breaking changes and incomplete features

For questions, issues, or discussions, please use the [Issues](https://github.com/yourusername/hellfire/issues) section.
