"""Run this in Unreal Editor via Tools > Execute Python Script.
Reports missing production skeletal meshes; never downloads third-party content.
"""
import unreal

ASSETS = {
    "Survivor": "/Game/Production/Characters/Survivor/SK_Survivor",
    "Sabrina": "/Game/Production/Characters/Sabrina/SK_Sabrina",
    "ZombieHuman": "/Game/Production/Characters/ZombieHuman/SK_ZombieHuman",
    "ZombieDog": "/Game/Production/Characters/ZombieDog/SK_ZombieDog",
    "ZombieCrow": "/Game/Production/Characters/ZombieCrow/SK_ZombieCrow",
    "ZombieDeer": "/Game/Production/Characters/ZombieDeer/SK_ZombieDeer",
    "Mutant": "/Game/Production/Characters/Mutant/SK_Mutant",
}
found = 0
for kind, path in ASSETS.items():
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset is None:
        unreal.log_warning(f"[DEAD ZONE ASSET] MISSING {kind}: {path}")
    elif not isinstance(asset, unreal.SkeletalMesh):
        unreal.log_error(f"[DEAD ZONE ASSET] WRONG TYPE {kind}: {path}")
    else:
        found += 1
        unreal.log(f"[DEAD ZONE ASSET] OK {kind}: {path}")
unreal.log(f"[DEAD ZONE ASSET] Rigged meshes found: {found}/{len(ASSETS)}")
