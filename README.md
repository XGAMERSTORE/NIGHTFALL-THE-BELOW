# NIGHTFALL: THE WITCHING HOUR — Android

A landscape 3D horror-survival prototype set across Greendale, Riverdale, and the breach below. The game keeps scavenging, crafting, building, survival meters, and co-op, with a witchcraft story, named NPC encounters, broom flight, Witchfire, a grapple charm, and protective runic wards.

The world uses bundled Three.js and original 1024 × 1024 PBR texture sets for wet forest soil, cedar bark, and weathered cabin wood. It also includes separate normal and roughness maps, a generated overcast/night sky, higher mobile render resolution, and anisotropic filtering. The game does not need a graphics-library download; internet access is used only when a player chooses co-op.

This is still a lightweight prototype. Characters, monsters, buildings, and trees are procedural low-poly models; it does not contain photorealistic character models, facial animation, or a full commercial-game physics simulation. The current gameplay includes momentum-based movement, collisions with trees, gravity and landing damage, and a fear system that reacts to nearby monsters and eases near allies, wards, and fire.

## Build

Requires JDK 17 and Android SDK Platform 35 / Build Tools 35.0.0.

```sh
./gradlew assembleDebug bundleRelease
```

The installable debug APK is written to `app/build/outputs/apk/debug/`. The Play Console AAB is written to `app/build/outputs/bundle/release/`; configure an upload key before publishing.

GitHub Actions builds both files on pushes to `main` and `codex/**`, on pull requests to `main`, and when started manually. It also verifies the APK with Android's `apksigner`. Download the APK from the completed `Android build` run under **Actions → Artifacts**. The debug APK can be installed directly; Android may require uninstalling an older copy if it was signed with a different key.

## Controls

- Hold the directional pad to move; drag across the world to look.
- Use the right side controls to interact, strike, cast Witchfire, build, fly, and grapple.
- Keyboard: `WASD`, `E`, `R`, `V`, `G`, `B`, `C`, `I`, `J`, `M`, `Space`, `Ctrl`, and `Shift`.
- Focus fuels Witchfire and broom flight; it restores while grounded.
- Gather supplies to craft a Witchglass compass, build shelter, and place runic wards that repel monsters.

## Story

The Czech-language story sends Sabrina, the Spellman family, Salem, and the Riverdale group through a red-eclipse breach beneath Greendale. Craft the Witchglass compass, find three clues, then cross the Ninth Gate into a separate Hell arena. Speak with the gathered allies, recover three seals, defeat the Gate Prisoner with Witchfire, and return through the portal. Realm changes, seals, and the ending synchronize in co-op and save locally.
