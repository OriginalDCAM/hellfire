# Hellfire Engine Data Structures - High-Level Overview

This document provides a comprehensive overview of the data structures available in the Hellfire Engine. These structures form the foundation for building interactive 3D applications and games.

## Table of Contents

1. [Core Concepts](#core-concepts)
2. [Scene Management](#scene-management)
3. [Entity-Component System](#entity-component-system)
4. [Rendering & Graphics](#rendering--graphics)
5. [Asset Management](#asset-management)
6. [Quick Reference Table](#quick-reference-table)

---

## Core Concepts

The Hellfire Engine is built on a **component-based architecture** where game objects (Entities) are composed of reusable components. This design promotes flexibility, modularity, and performance.

### Key Design Patterns

- **Entity-Component System (ECS)**: Entities are containers for components
- **Scene Graph**: Hierarchical organization of entities with parent-child relationships
- **Resource Caching**: Automatic memory management for textures, shaders, and materials
- **Factory Pattern**: Convenient creation functions for common objects (lights, cameras, primitives)

---

## Scene Management

The scene management system organizes your game world into logical units.

### Scene

**Purpose**: Container for all entities, cameras, and environmental settings in your game world.

**When to use**:
- Creating different game levels or menu screens
- Organizing complex 3D environments
- Managing multiple cameras and lighting setups

**Key Features**:
- Entity lifecycle management (create, destroy, retrieve)
- Parent-child hierarchical relationships between entities
- Camera management for multiple viewpoints
- Environmental settings (ambient lighting, skybox)
- Serialization for saving/loading scenes

**Basic Usage**:
```cpp
#include <hellfire/scene/Scene.h>

// Create a new scene
Scene myScene("MainLevel");

// Add entities
Entity* player = myScene.createEntity("Player");
Entity* enemy = myScene.createEntity("Enemy");

// Create parent-child relationships
myScene.setParent(enemy, player); // Enemy follows player

// Update scene (propagates transforms)
myScene.update(deltaTime);
```

**Internal Structure**:
- Uses `std::unordered_map` for fast entity lookup by name/ID
- Maintains parent-child relationships with hash maps
- Size: 464 bytes

### SceneManager

**Purpose**: Manages multiple scenes and handles switching between them.

**When to use**:
- Managing multiple levels or screens in your game
- Loading scenes from JSON files
- Switching between game states (menu, gameplay, pause)

**Basic Usage**:
```cpp
#include <hellfire/scene/SceneManager.h>

SceneManager sceneManager;

// Create and add scenes
Scene* mainMenu = sceneManager.createScene("MainMenu");
Scene* level1 = sceneManager.createScene("Level1");

// Switch active scene
sceneManager.setActiveScene("Level1");

// Load scene from file
sceneManager.loadScene("assets/scenes/level2.json");

// Access active scene
Scene* current = sceneManager.getActiveScene();
```

**Internal Structure**:
- Uses `std::vector<Scene*>` for scene storage
- Size: 40 bytes

### SceneEnvironment

**Purpose**: Stores environmental settings like ambient lighting and skybox.

**When to use**:
- Setting up atmospheric lighting
- Adding skyboxes to your scenes
- Creating different environmental moods

---

## Entity-Component System

Entities are the building blocks of your game. Components define their behavior and appearance.

### Entity

**Purpose**: Base container for components, representing any object in your game world.

**When to use**:
- Every game object (players, enemies, props, UI elements)
- Organizing complex objects with multiple behaviors
- Creating reusable game object templates

**Key Features**:
- Add/remove components dynamically
- Type-safe component access
- Event broadcasting system
- Script component support for custom logic

**Basic Usage**:
```cpp
// Create entity (usually through Scene)
Entity* player = scene.createEntity("Player");

// Add components
auto* transform = player->addComponent<TransformComponent>();
auto* mesh = player->addComponent<RenderableComponent>();
auto* camera = player->addComponent<CameraComponent>();

// Retrieve components
auto* existingTransform = player->getComponent<TransformComponent>();

// Check for components
if (player->hasComponent<CameraComponent>()) {
    // Do camera-specific logic
}

// Remove components
player->removeComponent<CameraComponent>();
```

**Internal Structure**:
- Components stored in `std::unordered_map<std::type_index, std::unique_ptr<Component>>`
- Enables fast O(1) component lookup by type

### Core Components

#### TransformComponent

**Purpose**: Defines position, rotation, and scale in 3D space.

**When to use**: Almost every visible object needs a transform.

```cpp
auto* transform = entity->addComponent<TransformComponent>();
transform->getTransform().setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
transform->getTransform().setRotation(glm::vec3(0.0f, 45.0f, 0.0f));
transform->getTransform().setScale(glm::vec3(2.0f, 2.0f, 2.0f));
```

#### CameraComponent

**Purpose**: Defines viewpoint for rendering the scene.

**When to use**:
- Player cameras (first-person, third-person)
- Security cameras or secondary viewports
- Cinematic camera sequences

**Camera Types**:
- **Perspective**: Realistic 3D view with depth perception
- **Orthographic**: Parallel projection (no depth distortion), useful for UI and 2D games

```cpp
auto* camera = entity->addComponent<CameraComponent>();
camera->setPerspective(45.0f, aspectRatio, 0.1f, 100.0f);
// OR
camera->setOrthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);

// Get camera matrices for rendering
glm::mat4 viewMatrix = camera->getViewMatrix();
glm::mat4 projMatrix = camera->getProjectionMatrix();
```

#### LightComponent

**Purpose**: Adds lighting to your scene.

**When to use**: Creating realistic lighting and shadows.

**Light Types**:
1. **Directional Light**: Sunlight, affects entire scene uniformly
2. **Point Light**: Light bulb, radiates in all directions
3. **Spot Light**: Flashlight, cone-shaped illumination

```cpp
auto* light = entity->addComponent<LightComponent>();

// Directional light (sun)
light->setType(LightComponent::LightType::DIRECTIONAL);
light->setDirection(glm::vec3(-1.0f, -1.0f, 0.0f));
light->setColor(glm::vec3(1.0f, 0.95f, 0.8f));
light->setIntensity(1.5f);
light->setCastShadows(true);

// Point light (lamp)
light->setType(LightComponent::LightType::POINT);
light->setPosition(glm::vec3(5.0f, 3.0f, 2.0f));
light->setRange(10.0f);

// Spot light (flashlight)
light->setType(LightComponent::LightType::SPOT);
light->setInnerConeAngle(12.5f);
light->setOuterConeAngle(17.5f);
```

#### RenderableComponent

**Purpose**: Attaches a mesh to an entity for rendering.

**When to use**: Any visible object that needs to be drawn.

```cpp
auto* renderable = entity->addComponent<RenderableComponent>();
renderable->setMesh(myMesh);
renderable->setMaterial(myMaterial);
renderable->setCastShadows(true);
```

#### InstancedRenderableComponent

**Purpose**: Efficiently render many copies of the same object.

**When to use**:
- Crowds, forests, particle effects
- Rendering hundreds/thousands of similar objects
- Performance-critical scenarios

**Performance**: Can render 10,000+ objects with minimal overhead.

```cpp
auto* instanced = entity->addComponent<InstancedRenderableComponent>();
instanced->setMesh(treeMesh);
instanced->setMaterial(treeMaterial);

// Add instances
for (int i = 0; i < 1000; i++) {
    InstanceData instance;
    instance.transform = glm::translate(glm::mat4(1.0f), randomPosition());
    instance.color = glm::vec4(1.0f);
    instance.scale = randomFloat(0.8f, 1.2f);
    instanced->addInstance(instance);
}

instanced->updateBuffers(); // Upload to GPU
```

**Internal Structure**:
- Uses `std::vector<InstanceData>` for CPU-side storage
- Dedicated GPU buffers (VBO) for efficient rendering

#### ScriptComponent

**Purpose**: Base class for custom game logic and behavior.

**When to use**:
- Implementing game-specific behavior (player controller, AI, collectibles)
- Creating reusable gameplay components
- Adding properties that can be edited in tools

```cpp
class PlayerController : public ScriptComponent {
public:
    void init() override {
        // Called when component is added
        registerProperty("moveSpeed", speed);
        registerProperty("jumpForce", jumpForce);
    }

    void update(float deltaTime) override {
        // Called every frame
        // Implement movement logic
    }

    void onEvent(const std::string& eventName, void* data) override {
        // Handle events
    }

private:
    float speed = 5.0f;
    float jumpForce = 10.0f;
};

// Usage
entity->addComponent<PlayerController>();
```

---

## Rendering & Graphics

These structures handle visual representation and GPU resources.

### Mesh

**Purpose**: Stores vertex data and indices for 3D geometry.

**When to use**:
- Loading 3D models
- Creating procedural geometry
- Custom mesh generation

**Key Features**:
- Automatic OpenGL buffer management (VAO, VBO, IBO)
- Support for indexed drawing
- Instanced rendering support

**Basic Usage**:
```cpp
#include <hellfire/graphics/Mesh.h>

// Create custom mesh
std::vector<Vertex> vertices;
std::vector<unsigned int> indices;

// Define vertices
vertices.push_back({
    .position = glm::vec3(0.0f, 1.0f, 0.0f),
    .normal = glm::vec3(0.0f, 1.0f, 0.0f),
    .color = glm::vec3(1.0f, 0.0f, 0.0f),
    .texCoords = glm::vec2(0.5f, 1.0f)
});

// Create mesh
Mesh myMesh(vertices, indices);

// Draw
myMesh.draw();

// Instanced draw (for many copies)
myMesh.drawInstanced(instanceCount);
```

**Internal Structure**:
- `std::vector<Vertex>` for vertex data
- `std::vector<unsigned int>` for indices
- Size: 64 bytes (excluding dynamic arrays)

### Vertex

**Purpose**: Defines a single vertex with all rendering attributes.

**Structure**:
```cpp
struct Vertex {
    glm::vec3 position;    // 3D position
    glm::vec3 normal;      // Surface normal (for lighting)
    glm::vec3 color;       // Vertex color
    glm::vec2 texCoords;   // Texture coordinates (UV)
    glm::vec3 tangent;     // Tangent (for normal mapping)
    glm::vec3 bitangent;   // Bitangent (for normal mapping)
};
```

**Size**: 44 bytes per vertex

### Material

**Purpose**: Defines surface appearance (colors, textures, shader properties).

**When to use**:
- Controlling object appearance
- Creating different surface types (metal, wood, plastic)
- Customizing shaders and rendering

**Key Features**:
- Property-based system using `std::variant` for flexibility
- Support for multiple texture types
- Material instancing with per-instance overrides
- Custom shader integration

**Basic Usage**:
```cpp
#include <hellfire/graphics/material/Material.h>

Material material;

// Set color properties
material.setProperty("baseColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
material.setProperty("metalness", 0.5f);
material.setProperty("roughness", 0.3f);

// Add textures
material.setTexture("diffuse", diffuseTexture);
material.setTexture("normal", normalTexture);

// Use custom shader
ShaderInfo shaderInfo;
shaderInfo.vertexPath = "shaders/custom.vert";
shaderInfo.fragmentPath = "shaders/custom.frag";
material.setCustomShader(shaderInfo);

// Bind for rendering
material.bind(shader);
```

**Property Types Supported**:
- `bool`, `int`, `float`
- `glm::vec2`, `glm::vec3`, `glm::vec4`
- `glm::mat3`, `glm::mat4`
- Custom types via `std::variant`

**Internal Structure**:
- Properties stored in `std::map<std::string, Property>`
- Each property maps to shader uniforms

### Texture

**Purpose**: Image data for surface details and effects.

**When to use**:
- Adding images to surfaces
- Creating realistic materials
- Implementing special effects

**Texture Types**:
```cpp
enum class TextureType {
    DIFFUSE,    // Base color
    SPECULAR,   // Shininess/reflectivity
    NORMAL,     // Surface detail (bump mapping)
    AO,         // Ambient occlusion (shadows in crevices)
    ROUGHNESS,  // Surface roughness (PBR)
    METALNESS,  // Metallic properties (PBR)
    EMISSIVE,   // Self-illumination
    HEIGHT,     // Displacement/parallax
    OPACITY     // Transparency
};
```

**Basic Usage**:
```cpp
#include <hellfire/graphics/texture/Texture.h>

// Load texture (automatically cached)
auto diffuse = Texture::load("textures/wood_diffuse.png", TextureType::DIFFUSE);
auto normal = Texture::load("textures/wood_normal.png", TextureType::NORMAL);

// Configure settings
TextureSettings settings;
settings.filterMin = TextureFilter::LINEAR_MIPMAP_LINEAR;
settings.filterMag = TextureFilter::LINEAR;
settings.wrapS = TextureWrap::REPEAT;
settings.wrapT = TextureWrap::REPEAT;

auto customTexture = Texture::load("path/to/texture.png", TextureType::DIFFUSE, settings);

// Bind for rendering
diffuse->bind(0); // Texture unit 0
```

**Key Features**:
- Automatic caching prevents duplicate loads
- Configurable filtering and wrapping modes
- Mipmapping support for distant objects

### Transform3D

**Purpose**: Represents position, rotation, and scale in 3D space.

**When to use**:
- Positioning objects in the world
- Animating object movement
- Building hierarchical transformations

**Basic Usage**:
```cpp
#include <hellfire/graphics/Transform3D.h>

Transform3D transform;

// Position
transform.setPosition(glm::vec3(5.0f, 0.0f, -3.0f));
transform.translate(glm::vec3(1.0f, 0.0f, 0.0f)); // Relative movement

// Rotation (Euler angles in degrees)
transform.setRotation(glm::vec3(0.0f, 45.0f, 0.0f));
transform.rotate(glm::vec3(0.0f, 90.0f, 0.0f)); // Relative rotation

// Scale
transform.setScale(glm::vec3(2.0f, 2.0f, 2.0f));

// Get matrices for rendering
glm::mat4 localMatrix = transform.getMatrix();
glm::mat4 worldMatrix = transform.getWorldMatrix();
```

**Advanced Features**:
```cpp
// Quaternion rotation (no gimbal lock)
transform.setRotationQuat(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

// Custom matrix override
glm::mat4 customMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
transform.setMatrix(customMatrix);

// Serialization
transform.toJson(); // Save to JSON
```

### Geometry Primitives

**Purpose**: Quick creation of common 3D shapes.

**Available Primitives**:
- **Cube**: Box with configurable size
- **Sphere**: Sphere with configurable detail (rings/sectors)
- **Quad**: Flat rectangle/plane

**Basic Usage**:
```cpp
#include <hellfire/graphics/geometry/Cube.h>
#include <hellfire/graphics/geometry/Sphere.h>
#include <hellfire/graphics/geometry/Quad.h>

// Create cube entity with default settings
Entity* cube = Cube::create(scene);

// Create sphere with custom detail
Sphere::Config sphereConfig;
sphereConfig.rings = 32;
sphereConfig.sectors = 64;
Entity* sphere = Sphere::create(scene, sphereConfig);

// Create quad
Entity* floor = Quad::create(scene);

// Or create just the mesh
Mesh* cubeMesh = Cube::createMesh();
```

### OpenGL Buffer Objects

Low-level GPU buffer wrappers (advanced usage):

- **VA (Vertex Array)**: Manages vertex attribute configuration
- **VB (Vertex Buffer)**: Stores vertex data on GPU
- **IB (Index Buffer)**: Stores indices for indexed drawing
- **Framebuffer**: Off-screen rendering target

---

## Asset Management

These structures handle loading, caching, and organizing game assets.

### AssetRegistry

**Purpose**: Central database for tracking all game assets (models, textures, materials, scenes).

**When to use**:
- Managing large projects with many assets
- Implementing asset hot-reloading
- Building content browsers/editors
- Tracking asset dependencies

**Key Features**:
- UUID-based asset identification
- Asset metadata (type, path, name, timestamps)
- Bidirectional lookup (UUID ↔ path)
- Serialization for asset databases

**Basic Usage**:
```cpp
#include <hellfire/assets/AssetRegistry.h>

AssetRegistry registry;

// Register new asset
AssetID textureID = registry.registerAsset(
    "assets/textures/brick.png",
    AssetType::TEXTURE,
    "Brick Texture"
);

// Retrieve asset info
if (registry.hasAsset(textureID)) {
    AssetMetadata meta = registry.getAsset(textureID);
    std::cout << "Asset: " << meta.name << std::endl;
    std::cout << "Path: " << meta.filepath << std::endl;
}

// Find by path
if (auto id = registry.findAssetByPath("assets/textures/brick.png")) {
    // Asset exists
}

// List all textures
auto textures = registry.getAssetsByType(AssetType::TEXTURE);

// Save/load registry
registry.saveToFile("assets/registry.json");
AssetRegistry loaded = AssetRegistry::loadFromFile("assets/registry.json");
```

**Asset Types**:
```cpp
enum class AssetType {
    MODEL,
    TEXTURE,
    MATERIAL,
    SCENE,
    SHADER,
    UNKNOWN
};
```

**Internal Structure**:
- Dual hash maps for O(1) lookup:
  - `std::unordered_map<AssetID, AssetMetadata>` (by UUID)
  - `std::unordered_map<std::filesystem::path, AssetID>` (by path)

### Project

**Purpose**: Container for project metadata, scenes, and asset registry.

**When to use**:
- Organizing complete game projects
- Managing project settings and metadata
- Implementing save/load systems

**Basic Usage**:
```cpp
#include <hellfire/core/Project.h>

// Create new project
Project project("MyGame", "1.0.0", "path/to/project");

// Access integrated systems
SceneManager& sceneManager = project.getSceneManager();
AssetRegistry& assetRegistry = project.getAssetRegistry();

// Set metadata
auto& meta = project.getMetadata();
meta.name = "My Awesome Game";
meta.version = "1.2.0";

// Save project
project.save();

// Load project
Project loadedProject = Project::load("path/to/project");
```

### Resource Managers

#### TextureManager

**Purpose**: Singleton for caching and reusing textures.

**When to use**: Automatic - textures are cached by default.

```cpp
// Textures are automatically cached when loaded
auto tex1 = Texture::load("texture.png"); // Loads from disk
auto tex2 = Texture::load("texture.png"); // Returns cached copy
```

#### ShaderManager

**Purpose**: Compiles and caches shaders with preprocessor support.

**Key Features**:
- Include file processing
- Shader variants with #define macros
- Automatic recompilation on changes

```cpp
#include <hellfire/graphics/managers/ShaderManager.h>

// Compile shader
ShaderManager::compile("myshader", "vertex.glsl", "fragment.glsl");

// Create shader variant with defines
ShaderVariant variant;
variant.defines["USE_NORMAL_MAP"] = "";
variant.defines["MAX_LIGHTS"] = "8";
ShaderManager::compileVariant("myshader_variant", "vertex.glsl", "fragment.glsl", variant);
```

#### MaterialManager

**Purpose**: Utility for binding materials to shaders.

```cpp
MaterialManager::bindMaterial(material, shader);
```

---

## Specialized Systems

### AnimationSystem

**Purpose**: Callback-based animation system for procedural animations.

**When to use**:
- Creating simple animations without keyframe data
- Procedural motion (rotation, oscillation, circular paths)
- UI animations and effects

**Basic Usage**:
```cpp
#include <hellfire/assets/AnimationSystem.h>

AnimationSystem animSystem;

// Rotate object continuously
animSystem.addRotationAnimation(entity, glm::vec3(0.0f, 1.0f, 0.0f), 2.0f, true);

// Pulsing scale effect
animSystem.addPulsingAnimation(entity, 1.0f, 1.5f, 1.0f, true);

// Circular path
animSystem.addCircularPathAnimation(entity, 5.0f, 3.0f, true);

// Custom animation with callback
Animation custom;
custom.name = "CustomAnim";
custom.target = entity;
custom.duration = 2.0f;
custom.loop = true;
custom.callback = [](Entity* e, float t) {
    // t goes from 0.0 to 1.0 over duration
    float height = std::sin(t * 2.0f * 3.14159f) * 5.0f;
    auto* transform = e->getComponent<TransformComponent>();
    transform->getTransform().setPosition(glm::vec3(0.0f, height, 0.0f));
};
animSystem.addAnimation(custom);

// Update system each frame
animSystem.update(deltaTime);
```

### Factory Functions

Convenient creation functions for common objects:

#### Lights
```cpp
#include <hellfire/graphics/lighting/DirectionalLight.h>
#include <hellfire/graphics/lighting/PointLight.h>
#include <hellfire/graphics/lighting/SpotLight.h>

Entity* sun = DirectionalLight::create(scene);
Entity* lamp = PointLight::create(scene);
Entity* flashlight = SpotLight::create(scene);
```

#### Cameras
```cpp
#include <hellfire/scene/Cameras.h>

Entity* mainCam = PerspectiveCamera::create(scene);
Entity* uiCam = OrthographicCamera::create(scene);
```

---

## Quick Reference Table

| Category | Structure | Size (bytes) | Primary Use Case |
|:---------|:----------|:------------:|:-----------------|
| **Scene** | Scene | 464 | World container with entities and settings |
| | SceneManager | 40 | Managing multiple scenes |
| | SceneEnvironment | - | Environmental settings (lighting, skybox) |
| **Entity** | Entity | - | Base game object container |
| | TransformComponent | - | Position, rotation, scale |
| | CameraComponent | - | Viewpoint for rendering |
| | LightComponent | - | Lighting (directional, point, spot) |
| | RenderableComponent | - | Single mesh rendering |
| | InstancedRenderableComponent | - | Efficient rendering of many objects |
| | ScriptComponent | - | Custom game logic |
| **Graphics** | Mesh | 64 | 3D geometry with vertices and indices |
| | Vertex | 44 | Single vertex with all attributes |
| | Material | - | Surface appearance and properties |
| | Texture | - | Image data for surfaces |
| | Transform3D | - | 3D transformation math |
| | Shader | - | GPU programs for rendering |
| | Framebuffer | - | Off-screen rendering |
| **Assets** | AssetRegistry | - | Asset tracking and metadata |
| | Project | - | Project container with metadata |
| | TextureManager | - | Texture caching |
| | ShaderManager | - | Shader compilation and caching |
| **Animation** | AnimationSystem | - | Callback-based animations |
| **Primitives** | Cube | - | Box geometry factory |
| | Sphere | - | Sphere geometry factory |
| | Quad | - | Plane/rectangle factory |

---

## Best Practices

### Performance Tips

1. **Use Instancing**: For multiple similar objects (trees, rocks, grass), use `InstancedRenderableComponent` instead of individual entities
2. **Cache Resources**: Reuse textures, materials, and meshes instead of creating duplicates
3. **Minimize State Changes**: Group objects by material to reduce GPU state changes
4. **LOD**: Use simpler meshes for distant objects

### Memory Management

1. **Smart Pointers**: The engine uses `unique_ptr` and `shared_ptr` automatically
2. **Scene Cleanup**: Destroying a scene automatically cleans up its entities
3. **Resource Lifetime**: Textures and shaders are reference-counted

### Organization

1. **Component Composition**: Build complex objects from simple components
2. **Scene Hierarchy**: Use parent-child relationships for logical grouping
3. **Asset Registry**: Register all assets for easy tracking and management
4. **Separate Concerns**: Use ScriptComponents for game logic, keep rendering components focused on visuals

---

## Common Patterns

### Creating a Game Object
```cpp
// Create entity
Entity* player = scene.createEntity("Player");

// Add transform
auto* transform = player->addComponent<TransformComponent>();
transform->getTransform().setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

// Add visual representation
auto* renderable = player->addComponent<RenderableComponent>();
renderable->setMesh(playerMesh);
renderable->setMaterial(playerMaterial);

// Add custom logic
auto* controller = player->addComponent<PlayerController>();

// Add camera
auto* camera = player->addComponent<CameraComponent>();
camera->setPerspective(45.0f, aspectRatio, 0.1f, 100.0f);
```

### Loading a Scene
```cpp
SceneManager manager;

// Load from file
manager.loadScene("assets/scenes/level1.json");

// Get active scene
Scene* scene = manager.getActiveScene();

// Access entities
Entity* player = scene->getEntityByName("Player");
```

### Setting Up Lighting
```cpp
// Sun (directional light)
Entity* sun = DirectionalLight::create(scene);
auto* sunLight = sun->getComponent<LightComponent>();
sunLight->setDirection(glm::vec3(-1.0f, -1.0f, -0.5f));
sunLight->setIntensity(1.2f);
sunLight->setCastShadows(true);

// Lamp (point light)
Entity* lamp = PointLight::create(scene);
auto* lampLight = lamp->getComponent<LightComponent>();
lampLight->setPosition(glm::vec3(5.0f, 3.0f, 0.0f));
lampLight->setRange(15.0f);
lampLight->setColor(glm::vec3(1.0f, 0.8f, 0.6f));
```

---

## Next Steps

- See `scene_management.md` for detailed scene system documentation
- Check individual header files for complete API reference
- Explore example projects in the `examples/` directory

For questions or contributions, visit the project repository.
