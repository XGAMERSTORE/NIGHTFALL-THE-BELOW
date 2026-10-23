# NIGHTFALL: THE BELOW — Android

Landscape Android build of the Cedar Ridge survival prototype. The 3D engine and co-op client are bundled in `app/src/main/assets`, so the world loads without downloading graphics libraries. Internet access is used only when a player chooses co-op.

## Build

Requires JDK 17 and Android SDK Platform 35 / Build Tools 35.0.0.

```sh
./gradlew assembleDebug bundleRelease
```

The installable debug APK is written to `app/build/outputs/apk/debug/`. The Play Console AAB is written to `app/build/outputs/bundle/release/` and is unsigned until a developer upload key is configured.

## Mobile controls

- Hold the directional pad to move.
- Drag on the open world to look around.
- Use the right side buttons to scavenge, attack, and build.
- The hotbar selects equipment. Build, craft, pack, journal, and map controls remain available in the game UI.

## Graphics fix

The third-person camera previously read an unset `game.y` value. That made the camera position `NaN` after entering the world, so WebGL rendered an empty scene. The camera now follows the player's actual rig height. The Android host enables hardware acceleration and immersive landscape mode; the touch UI and render settings are tuned for phone screens.
