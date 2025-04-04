## DCraft Datastructures

These are the datastructures that are available in the framework.

| Struct			    | Size(Bytes) | 
|:-------------|:-----------:| 
| Scene                 |     464     |
| Object3D			  |    400		    | 
| Mesh				     |     64	     | 
| Vertex			    |     44      | 
| Texture2D			 |    IMPL     |
| Model				    |     80      | 
| SceneManager     |     40      |

## Object3D

Object3D is the base class from which all objects in the framework inherit. This base class holds the transform,
childrens, model if applicable, and also holds its own world identity matrix.

Code example:

```cpp
Object3D* cube = new Object3D();
cube->model.add_mesh(DCRAFT_CUBE_MESH);

Scene scene = Scene("Example scene");
SceneManager* scenemanager = new SceneManager();
scenemanager.add(scene);
scenemanager.load("Example scene");
// or for projects with multiple scene
scenemanager.load("examplescene.json");
// or 
scenemanager.load(scene);
```

