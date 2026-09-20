# SHAHDAG Mobile Alpha

This is the first Android runtime branch for SHAHDAG, optimized as a lightweight native OpenGL ES prototype for the target Poco X3 Pro class device.

## Target
- Android 8.0+ (API 26+)
- Landscape
- 60 FPS baseline target, 90 FPS as a higher target where thermals/GPU allow
- Native OpenGL ES 2.0-compatible renderer for broad Adreno compatibility
- No server requirement

## Current playable slice
- Main menu: Play / Settings / Exit
- Mission/loadout screen
- Residential Operation graybox scene
- Mobile FPS movement and touch-look
- Fire control
- Basic targets with simple pursuit behavior
- Lightweight procedural environment

## Important
This is not the final photorealistic asset pass. It is the first mobile runtime slice intended for actual device feedback. The next iterations replace procedural graybox geometry with optimized modular meshes/materials while preserving the same runtime contract.

## Build
Requires Android SDK 35, Build Tools 35.0.0, JDK 17, and Gradle. The repository also contains a GitHub Actions workflow at `.github/workflows/android.yml` that builds a debug APK.
