# Building the SHAHDAG mobile APK without a PC

The Android toolchain is not installed in the current build container, so this ZIP includes the Android source and an automated GitHub Actions build path.

## Easiest phone-only route
1. Put the SHAHDAG project in a GitHub repository from the phone.
2. Open **Actions** → **Build SHAHDAG Android APK** → run the workflow.
3. After it finishes, download the `SHAHDAG-mobile-debug` artifact and install the APK on the phone.

## Local Android build
Use an Android build environment that provides:
- JDK 17
- Gradle
- Android SDK Platform 35
- Android Build Tools 35.0.0

Then from `SHAHDAG/Mobile` run:

```bash
gradle assembleDebug --no-daemon
```

APK output:

```text
app/build/outputs/apk/debug/app-debug.apk
```

## Test feedback
Please report:
- average FPS and dips/stutter
- touch sensitivity
- movement feel
- aiming feel
- visual readability
- collisions / places you get stuck
- enemy behavior
- overheating or battery drain
- crashes and exact reproduction steps
