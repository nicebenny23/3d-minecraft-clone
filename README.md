# STN Game Engine

A **C++ game engine** built from scratch featuring a **custom Entity Component System (ECS)**, dynamic arrays, and a Minecraft implmentation built on top of it.

---

## Features

- **Custom Entity Component System (ECS)**  
  - Modular design for flexible game objects  
  - Enables efficient interactions between entities
  - Uses a hyper-optimized sparse-page aproach for minimal memory usage.
  - Delayed Event pattern.
     
- **std from scratch**  
  - much safer due to bounds checking by default  
  - Supports move-only and non-default types  
  - Extra utilities including a type erased allocator, and a more ergonomic std::ranges
  
- **Voxel / Grid System**  
  - World representation inspired by Minecraft  
  - Efficient memory layout for dynamic grids  

- **Mid Level Renderer**
- Built on opengl and serves as a much higher level api.
- Supports the usage of renderables
