# STN Game Engine
<img width="64" height="64" alt="crystaloreenhanced" src="https://github.com/user-attachments/assets/f34fcf3b-0cd7-4f03-a03d-4fb9cc710ce2" />

A **C++ game engine** built from scratch.

---

## Engine Features

- **Custom Entity Component System (ECS)**  
  - Modular design for flexible game objects  
  - Enables efficient interactions between entities
  - Uses a hyper-optimized sparse-page approach for minimal memory usage.
  - Delayed Event pattern.
  - Typed Refrences.
     
- **Std from scratch**  
  - much safer: due to bounds checking by default  
  - Supports move-only and non-default types  
  - Extra utilities including a more ergonomic std::ranges and an improved std::optional
     
- **Ccd physics engine**
- supports Relistic AABB collisions

- **High Level Renderer**
- bases on Meshes and Renderables
 
 
 ## Game
- **Voxel / Grid System**  
  - World representation inspired by Minecraft  
  - Efficient memory layout for dynamic grids
  - Lazy Loading
  - Efficient lighting system
  - Fluid simulation
   
