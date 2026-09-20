# SHAHDAG — Build the APK from your phone

The repository is already configured with a GitHub Actions workflow at `.github/workflows/android.yml`.
It builds a real Android APK on a GitHub-hosted Linux runner.

## 1. Put this project on GitHub

Create a new GitHub repository, then upload the **contents of this `SHAHDAG` folder** (not the outer ZIP file).
Keep `.github/workflows/android.yml` in place.

## 2. Start the build

On GitHub open:

`Actions` → `Build SHAHDAG Android APK` → `Run workflow`

The workflow pins Gradle 8.9, installs Android API 35/build-tools 35.0.0, and runs:

`gradle assembleDebug`

## 3. Download the APK

When the run finishes successfully:

`Actions` → open the completed run → `Artifacts` → `SHAHDAG-mobile-debug`

Download and extract the artifact. The file is:

`app-debug.apk`

Install it on the Poco X3 Pro with Android's normal APK installer.

## 4. Feedback loop

After you test the APK, report exact problems such as:
- FPS drops / stutters
- touch controls
- camera sensitivity
- collision
- enemies
- weapon firing
- menus/settings
- crashes

Then return the **same updated `SHAHDAG.zip`** and development continues on that single project.
