# SHAHDAG

Realistic single-player tactical FPS — continuous project.

## Current milestone
**MAP 01 — Residential Operation / Foundation v0.2**

The project now contains a denser runtime-generated residential operation footprint: two-storey house shell, deliberate door/window openings, interior room partitions, stair route, basic furniture proxies, garage/service area, natural perimeter limits and a layered lighting foundation.

This is still a foundation/greybox-plus milestone. Final AAA meshes, authored PBR materials, animation assets, VFX, final audio recordings, AI behavior and production navigation data are subsequent passes.

## Engine target
Unreal Engine 5.6.x.

## Open
1. Install Unreal Engine 5.6.x.
2. Open `SHAHDAG.uproject`.
3. Allow Unreal to generate project files.
4. Build the `SHAHDAG` Editor target.
5. Open the project and press Play.

## Controls in this milestone
- WASD: Move
- Mouse: Look
- Left Shift: Sprint
- Left Ctrl / C: Crouch

## Validation note
The source has not been compiled in this environment. Final rendering, collision, build, performance and visual validation must be performed in Unreal Editor on a real Windows development machine.

## Project rule
There is exactly one continuous SHAHDAG project. Future milestones must extend this same folder/project instead of creating disconnected projects.


## Current milestone
Map 01 has advanced to **Atmosphere / Art-Direction Foundation**: procedural primitive look differentiation, sky atmosphere, layered post-process, expanded exterior dressing, glass/door proxies, and documented lighting/audio contracts. Final authored AAA assets still require UE5 runtime validation.

## Mobile Runtime

SHAHDAG now includes `Mobile/`, a native Android/OpenGL ES runtime slice intended for real-device feedback on the Poco X3 Pro class. This branch is the current playable feedback target. It preserves the same game concepts—menu, mission selection, loadout, FPS controls, residential operation map—while using a mobile-specific renderer/performance budget.

The mobile source is intentionally separate from the legacy UE5 foundation files inside this single SHAHDAG archive; the archive remains the single project package exchanged between iterations.

## Android testing status
The Mobile module is a native Android/OpenGL ES prototype target for Poco X3 Pro class devices.
The repository includes a GitHub Actions workflow that produces a real `app-debug.apk` on a hosted x86_64 build runner.
The assistant environment does not have an Android/Gradle build toolchain or a hardware Android emulator, so the APK binary itself is not claimed as locally tested here.
