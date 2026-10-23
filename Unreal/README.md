# NIGHTFALL: THE BELOW — Unreal graphics port

This is a separate Unreal Engine 5.8 C++ project for the Android graphics port. The existing Android/WebView project remains available at the repository root while this port is developed.

## Current slice

- A broad, smooth procedural county terrain with rolling ridges, hills, and a low valley.
- A forest pass using instanced engine meshes to keep the prototype within a mobile-friendly draw-call budget.
- A cabin with a walkable doorway, interior floor, roof, and collision on its structural pieces.
- A first-person explorer capsule with gravity, slope handling, jump, keyboard, mouse, and gamepad input.
- Cold directional lighting and atmospheric fog.
- Android packaging defaults for ARM64, Vulkan, and target SDK 35.

The project is intentionally limited to the requested graphics slice. It does not add missions, inventory, story, monsters, or online play.

## Open and run

1. Install Unreal Engine 5.8 and its Android SDK through Unreal Turnkey.
2. Open `NIGHTFALL_THE_BELOW.uproject` and let Unreal generate and compile the C++ project.
3. Press Play to inspect the terrain and enterable cabin. Package for Android from Platforms → Android.

## Visual asset status

The first scene uses engine primitive meshes and procedural terrain as a working greybox. It is **not yet photorealistic**, has no licensed character model or scanned environment assets, and does not ship 8K textures. Those require real production assets and a high-end platform build; texture resolution is also independent from the resolution a phone renders. The scene is structured for replacing the temporary meshes with Nanite-off mobile LOD assets and PBR materials.

The Android build profile targets API 35, but no Unreal Editor, Android SDK, or device is present in this workspace, so an Unreal APK has not been built or device-tested here.
