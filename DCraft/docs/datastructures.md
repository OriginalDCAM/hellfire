## DCraft Datastructures
These are the datastructures that are available in the framework.

| Struct			| Size(Bytes)     | Price |
| :----------------	| :------:  | ----: |
| Object3D			|   236		| 23.99 |
| Mesh				|   64	    | 23.99 |
| Vertex			|   44      | 19.99 |
| Texture2D			|   IMPL    | 42.99 |
| Model				|  80       | 42.99 |


## Object3D
Object3D is the base class from which all objects in the framework inherit. This base class holds the transform, childrens, model if applicable, and also holds its own world identity matrix.

Code example:
```cpp
Object3D* cube = new Object3D();
cube->model.add_mesh(DCRAFT_CUBE_MESH);

Scene scene = Scene("Example scene");
SceneManager scenemanager = SceneManager();
scenemanager.add(scene);
scenemanager.load("Example scene");
// or for projects with multiple scene
scenemanager.load("examplescene.json");
// or 
scenemanager.load(scene);
```

