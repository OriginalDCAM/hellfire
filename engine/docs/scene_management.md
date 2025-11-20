# SceneManager Documentation

## Overview

The `SceneManager` class is the central component for managing scenes in the Hellfire Engine. It handles scene lifecycle operations including creation, loading, saving, activation, and updates. The SceneManager maintains a collection of scenes and tracks which scene is currently active.

**Header:** `hellfire/scene/SceneManager.h`
**Namespace:** `hellfire`

## Key Features

- Create and manage multiple scenes
- Load scenes from JSON files
- Save scenes to disk
- Set active scene and receive activation callbacks
- Camera management across scenes
- Entity lookup by name
- Automatic prevention of duplicate scene loading

## Class Reference

### Constructor & Destructor

```cpp
SceneManager();
~SceneManager();
```

The constructor initializes the SceneManager with no active scene. The destructor automatically cleans up all managed scenes.

### Scene Creation and Loading

#### create_scene

```cpp
Scene* create_scene(const std::string& name = "GameScene");
```

Creates a new empty scene and adds it to the scene manager.

**Parameters:**
- `name` - The name for the new scene (default: "GameScene")

**Returns:** Pointer to the newly created scene

**Example:**
```cpp
hellfire::SceneManager scene_manager;

// Create a scene with default name
Scene* default_scene = scene_manager.create_scene();

// Create a scene with custom name
Scene* menu_scene = scene_manager.create_scene("MainMenu");
Scene* game_scene = scene_manager.create_scene("Level1");
```

---

#### load_scene

```cpp
Scene* load_scene(const std::string& filename);
```

Loads a scene from a JSON file. If the scene has already been loaded (based on filename), returns the existing scene instead of creating a duplicate.

**Parameters:**
- `filename` - Path to the scene file

**Returns:** Pointer to the loaded scene, or `nullptr` if loading failed

**Example:**
```cpp
Scene* loaded_scene = scene_manager.load_scene("assets/scenes/level1.json");
if (loaded_scene) {
    scene_manager.set_active_scene(loaded_scene);
}

// Attempting to load the same scene again returns the existing instance
Scene* same_scene = scene_manager.load_scene("assets/scenes/level1.json");
// same_scene == loaded_scene (no duplicate created)
```

**Note:** The SceneManager prevents duplicate scenes from being loaded. When importing the same scene file twice, it will return the previously loaded scene and won't store a duplicate.

---

#### save_scene

```cpp
bool save_scene(const std::string& filename, Scene* scene = nullptr) const;
```

Saves a scene to a JSON file.

**Parameters:**
- `filename` - Path where the scene should be saved
- `scene` - The scene to save (if `nullptr`, saves the active scene)

**Returns:** `true` if save was successful, `false` otherwise

**Example:**
```cpp
// Save the active scene
bool success = scene_manager.save_scene("assets/scenes/my_scene.json");

// Save a specific scene
Scene* specific_scene = scene_manager.create_scene("SpecialLevel");
success = scene_manager.save_scene("assets/scenes/special.json", specific_scene);

if (success) {
    std::cout << "Scene saved successfully!\n";
}
```

---

### Scene Management

#### set_active_scene

```cpp
void set_active_scene(Scene* scene);
```

Sets the currently active scene. This triggers the scene activation callback if one is registered.

**Parameters:**
- `scene` - Pointer to the scene to activate

**Example:**
```cpp
Scene* menu = scene_manager.create_scene("MainMenu");
Scene* game = scene_manager.create_scene("GameLevel");

// Switch to menu scene
scene_manager.set_active_scene(menu);

// Later, switch to game scene
scene_manager.set_active_scene(game);
```

---

#### get_active_scene

```cpp
Scene* get_active_scene() const;
```

Gets the currently active scene.

**Returns:** Pointer to the active scene, or `nullptr` if no scene is active

**Example:**
```cpp
Scene* current = scene_manager.get_active_scene();
if (current) {
    std::cout << "Current scene: " << current->get_name() << "\n";
}
```

---

#### get_scenes

```cpp
std::vector<Scene*> get_scenes();
```

Gets all scenes managed by this SceneManager.

**Returns:** Vector of pointers to all scenes

**Example:**
```cpp
auto all_scenes = scene_manager.get_scenes();
for (Scene* scene : all_scenes) {
    std::cout << "Scene: " << scene->get_name() << "\n";
}
```

---

#### update

```cpp
void update(float delta_time);
```

Updates the active scene. Should be called every frame from the main game loop.

**Parameters:**
- `delta_time` - Time elapsed since last frame (in seconds)

**Example:**
```cpp
// In your main game loop
while (running) {
    float delta = calculate_delta_time();
    scene_manager.update(delta);
}
```

---

#### clear

```cpp
void clear();
```

Removes and deletes all scenes, setting the active scene to `nullptr`.

**Example:**
```cpp
// Clean up all scenes
scene_manager.clear();
```

---

### Camera Management

#### set_active_camera

```cpp
void set_active_camera(EntityID camera) const;
```

Sets the active camera in the current scene.

**Parameters:**
- `camera` - EntityID of the camera entity

**Example:**
```cpp
Scene* scene = scene_manager.get_active_scene();
EntityID camera_entity = scene->create_entity("MainCamera");
// ... add camera component ...
scene_manager.set_active_camera(camera_entity);
```

---

#### get_active_camera

```cpp
CameraComponent* get_active_camera() const;
```

Gets the active camera component from the current scene.

**Returns:** Pointer to the active CameraComponent, or `nullptr` if none is set

**Example:**
```cpp
CameraComponent* camera = scene_manager.get_active_camera();
if (camera) {
    glm::mat4 view_matrix = camera->get_view_matrix();
}
```

---

#### get_camera_entities

```cpp
std::vector<EntityID> get_camera_entities() const;
```

Gets all entities with camera components in the active scene.

**Returns:** Vector of EntityIDs that have camera components

**Example:**
```cpp
auto cameras = scene_manager.get_camera_entities();
std::cout << "Found " << cameras.size() << " cameras in scene\n";

for (EntityID cam_id : cameras) {
    // Switch between cameras
    scene_manager.set_active_camera(cam_id);
}
```

---

### Entity Management

#### find_entity_by_name

```cpp
EntityID find_entity_by_name(const std::string& name);
```

Searches for an entity by name in the active scene.

**Parameters:**
- `name` - Name of the entity to find

**Returns:** EntityID of the found entity, or `0` if not found

**Example:**
```cpp
EntityID player = scene_manager.find_entity_by_name("Player");
if (player != 0) {
    Scene* scene = scene_manager.get_active_scene();
    Entity* player_entity = scene->get_entity(player);
    // ... work with player entity ...
}
```

---

### Callbacks

#### set_scene_activated_callback

```cpp
using SceneActivatedCallback = std::function<void(Scene*)>;
void set_scene_activated_callback(SceneActivatedCallback callback);
```

Registers a callback function that will be invoked whenever a scene is activated.

**Parameters:**
- `callback` - Function to call when a scene becomes active

**Example:**
```cpp
scene_manager.set_scene_activated_callback([](Scene* scene) {
    std::cout << "Scene activated: " << scene->get_name() << "\n";

    // Perform scene-specific initialization
    if (scene->get_name() == "GameLevel") {
        // Setup game-specific systems
    }
});

// The callback will be triggered when setting active scene
scene_manager.set_active_scene(my_scene);
```

---

## Usage Examples

### Basic Scene Setup

```cpp
#include "hellfire/scene/SceneManager.h"
#include "hellfire/scene/Scene.h"

int main() {
    hellfire::SceneManager scene_manager;

    // Create a new scene
    Scene* game_scene = scene_manager.create_scene("MainGame");

    // Create some entities
    EntityID player = game_scene->create_entity("Player");
    EntityID enemy = game_scene->create_entity("Enemy");

    // Set as active scene
    scene_manager.set_active_scene(game_scene);

    // Main game loop
    bool running = true;
    float delta_time = 0.016f; // ~60 FPS

    while (running) {
        scene_manager.update(delta_time);
    }

    return 0;
}
```

---

### Multi-Scene Application

```cpp
hellfire::SceneManager scene_manager;

// Create multiple scenes
Scene* main_menu = scene_manager.create_scene("MainMenu");
Scene* level1 = scene_manager.create_scene("Level1");
Scene* level2 = scene_manager.create_scene("Level2");

// Setup scene transition callback
scene_manager.set_scene_activated_callback([](Scene* scene) {
    std::cout << "Transitioning to: " << scene->get_name() << "\n";

    // Could trigger loading screen, music change, etc.
});

// Start with main menu
scene_manager.set_active_scene(main_menu);

// Later, transition to level 1
scene_manager.set_active_scene(level1);

// After completing level 1, go to level 2
scene_manager.set_active_scene(level2);
```

---

### Scene Persistence (Save/Load)

```cpp
hellfire::SceneManager scene_manager;

// Create and populate a scene
Scene* my_scene = scene_manager.create_scene("SavedLevel");
EntityID entity = my_scene->create_entity("ImportantObject");
// ... add components and configure scene ...

// Save the scene
if (scene_manager.save_scene("saves/level_state.json", my_scene)) {
    std::cout << "Scene saved successfully\n";
}

// Later, load the scene back
Scene* loaded = scene_manager.load_scene("saves/level_state.json");
if (loaded) {
    scene_manager.set_active_scene(loaded);
    std::cout << "Scene loaded: " << loaded->get_name() << "\n";
}
```

---

### Camera System Integration

```cpp
hellfire::SceneManager scene_manager;
Scene* scene = scene_manager.create_scene("CameraDemo");
scene_manager.set_active_scene(scene);

// Create a camera entity
EntityID camera_entity = scene->create_entity("MainCamera");
// ... add CameraComponent to the entity ...

// Set it as the active camera
scene_manager.set_active_camera(camera_entity);

// In your render loop
void render() {
    CameraComponent* camera = scene_manager.get_active_camera();
    if (camera) {
        glm::mat4 view = camera->get_view_matrix();
        glm::mat4 projection = camera->get_projection_matrix();

        // Use matrices for rendering
        shader.set_uniform("view", view);
        shader.set_uniform("projection", projection);
    }
}

// Switch between multiple cameras
auto all_cameras = scene_manager.get_camera_entities();
int current_camera_index = 0;

void switch_camera() {
    if (!all_cameras.empty()) {
        current_camera_index = (current_camera_index + 1) % all_cameras.size();
        scene_manager.set_active_camera(all_cameras[current_camera_index]);
    }
}
```

---

### Entity Search and Management

```cpp
hellfire::SceneManager scene_manager;
Scene* scene = scene_manager.create_scene("EntityDemo");

// Create some entities
scene->create_entity("Player");
scene->create_entity("Enemy_1");
scene->create_entity("Enemy_2");
scene->create_entity("PowerUp");

scene_manager.set_active_scene(scene);

// Find specific entities
EntityID player = scene_manager.find_entity_by_name("Player");
if (player != 0) {
    Entity* player_entity = scene->get_entity(player);
    std::cout << "Found player: " << player_entity->get_name() << "\n";
}

// Search for enemies (would need to implement pattern matching)
for (const auto& entity_id : scene->get_root_entities()) {
    Entity* entity = scene->get_entity(entity_id);
    if (entity->get_name().find("Enemy") != std::string::npos) {
        std::cout << "Found enemy: " << entity->get_name() << "\n";
    }
}
```

---

## Best Practices

### Memory Management
- The SceneManager owns all scenes created or loaded through it
- Scenes are automatically deleted when the SceneManager is destroyed or `clear()` is called
- Don't delete Scene pointers returned by the SceneManager manually

### Scene Loading
- Always check return values from `load_scene()` for `nullptr`
- Scene files must be valid JSON with "name" and "version" fields
- Loading the same file multiple times returns the existing scene (no duplicates)

### Active Scene
- Always ensure an active scene is set before calling `update()`
- Check `get_active_scene() != nullptr` before performing scene operations
- Use the scene activation callback for initialization logic

### Update Loop
- Call `update()` every frame with the delta time
- Delta time should be in seconds (e.g., 0.016 for ~60 FPS)

### Camera Management
- Verify camera entities have CameraComponent before setting as active
- Check `get_active_camera()` for `nullptr` before use
- Use `get_camera_entities()` to enumerate all available cameras

---

## Integration with ECS

The SceneManager works closely with the Entity Component System:

```cpp
// Scene manages entities
Scene* scene = scene_manager.get_active_scene();

// Create entities
EntityID entity = scene->create_entity("GameObject");

// Add components (example)
Entity* entity_ptr = scene->get_entity(entity);
// entity_ptr->add_component<TransformComponent>();
// entity_ptr->add_component<MeshComponent>();

// Scene update propagates to all entities and components
scene_manager.update(delta_time);
```

---

## Thread Safety

The SceneManager is **not thread-safe**. All operations should be performed on the main thread or properly synchronized if accessed from multiple threads.

---

## See Also

- `Scene` class - Individual scene management
- `Entity` class - Entity lifecycle and components
- `CameraComponent` - Camera functionality
- [Scene File Format](scene_file_format.md) _(if available)_
