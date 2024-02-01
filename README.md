# Sparrow
COMP5530 Group Project


The primary goal of this project was to develop a sophisticated game engine, capable of creating immersive scenes with intricate gameplay elements and appealing features. 

The objective of this project is to build up a robust software with appropriate complexity. It contains a basic engine core and, functional GUI editor.

Main features include:

**Render**: shows the scenes and models. In addition to handling basic PBR rendering, we have implemented several screen-space global illumination techniques in our engine, such as SSAO (Screen Space Ambient Occlusion), SSR (Screen Space Reflections), and improved PCSS (Percentage-Closer Soft Shadows). Furthermore, we have developed a custom rendering RenderGraph.

**Animation**: enabling more dynamic effects to the game graphics and scenes. We have achieved real-time and efficient rendering of skeletal animations through optimizations utilizing GPU computing and data preloading.

**Physics**: Making the game objects behave more realistically. Based on PhysX, we have implemented real-time simulation of rigid body motion and binding of multiple types of colliders.

**Audio**: enriching the gameplay experience by playing sound. FMOD has brought a high-quality audio experience to the Sparrow engine, while also providing multiple sound sources and 3D audio within the in-game scenes.

**Event**: handling user inputs that allow runtime interactivity. We have implemented a traversal listener method for keyboard and mouse events, allowing the creation of complex control behaviors.

**ECS architecture** is also introduced for its memory-friendly feature and easy-to-manage codes, which allows all the members to develop different modules at higher quality. What is more, it would be easier to stitch all the modules together as a core system

To establish a consistent and streamlined approach for resource management, we developed a **filesystem** for providing a sandbox directory, an asset manager for data baking and arrangements, and a resource manager for file IO.

To establish the event framework, we initially constructed the foundation by introducing the concepts of event responder and event chain. From there, we inherit and extend these concepts to build the UI responder and Button components.

Last but not least, a **Lua binding module** is required as the **game-play development language** which consists of the ‘Application’ and ECS framework.

![image](https://github.com/NANAnoo/Sparrow/assets/110170509/5298825e-f096-4ba4-a5bd-54eb6efee598)



## How to build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug # or Release
cmake --build ./ --target SPWTestApp -j 16
```
### environment required

| Library | Version |
|---------|---------|
| OpenGL  | >=4.5   |
