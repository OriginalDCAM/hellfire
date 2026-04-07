# Computer Graphics Final Assignment

## Introduction
This is my final assignment for the Computer Graphics course, where I developed a 3D graphics engine/framework using OpenGL. Throughout this course, I've learned extensively from resources like LearnOpenGL and the Real-Time Rendering book. As a demonstration of the engine's capabilities, I have built a realistic solar system simulation featuring orbital animations and an asteroid belt between Mars and Jupiter. This project has been an incredible learning experience, and I'm excited to continue developing it with more advanced features.

## Required Packages (Will be bundled with the project soon)
- GLUT in: C:/Libraries
- GLEW in: C:/Libraries
- GLM in: C:/Libraries

## Packages Bundled with Project
- **Assimp** - Model loading
- **ImGui** - Graphical user interface
- **ImGuizmo** - 3D manipulation gizmos
- **stb** - Image loading
- **nlohmann** - JSON parsing

## Installation (Visual Studio)
1. Clone the Git repository recursively (this project uses Git submodules):
   ```bash
   git clone --recursive [repository-url]
   ```
2. Right-click in the root directory of the project and select "Open with Visual Studio"
3. Run the Sandbox project

## Features
- **Core 3D Rendering**
    - Basic 3D geometry rendering
    - Instanced rendering for efficient mass object rendering (asteroid belt)
    - Advanced camera movement system (walk/drone modes)

- **Lighting System**
    - Multiple light types (ambient, diffuse, specular)
    - Point lights and directional lights
    - Advanced material system with custom shaders

- **Asset Pipeline**
    - Model loading (OBJ, GLTF, FBX formats)
    - Texture loading and caching
    - Material system with property-based uniforms

- **Animation & Scene Management**
    - Complex orbital mechanics with realistic physics
    - Scene transitions and management
    - C++ scripting system for custom behaviors

- **Advanced Graphics**
    - Multiple shader support with hot-reloading
    - Skybox/cubemap rendering
    - Transparency and alpha blending
    - Custom material creation system

## Current Demonstration
The engine showcases a scientifically accurate solar system featuring:
- **Realistic planetary orbits** with accurate orbital mechanics
- **10,000+ asteroids** rendered efficiently using instanced rendering
- **5 different asteroid types** with unique materials and models
- **Complex lighting** from the sun affecting all celestial bodies
- **Smooth camera controls** for exploring the solar system

## Future Improvements
- [ ] Fix scene serialization system
- [ ] Enhanced inspector interface
- [ ] Shadow mapping implementation
- [ ] Real-time reflections
- [ ] Water shader effects
- [ ] Multiple scene transitions demonstration
- [ ] 3D collision detection system
- [ ] Physically-Based Rendering (PBR) pipeline
- [ ] C# scripting support
- [ ] User interface redesign
- [ ] Frustum culling (performance optimization)
- [ ] Occlusion culling (performance optimization)
- [ ] Level-of-detail (LOD) system
- [ ] Post-processing effects pipeline

## Technical Highlights
- **Entity-Component-System (ECS)** architecture
- **Advanced instanced rendering** supporting 10,000+ objects
- **Professional asset loading pipeline** with caching
- **Modular material system** supporting multiple shader types
- **Scene serialization** to JSON format
- **Memory-efficient resource management**

## Performance
- Maintains 60+ FPS with 10,000 instanced asteroids
- Efficient memory usage through object pooling and caching
- Optimized rendering pipeline with minimal draw calls

