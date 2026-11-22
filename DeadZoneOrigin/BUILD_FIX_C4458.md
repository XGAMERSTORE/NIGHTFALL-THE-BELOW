# DEAD ZONE — UE 5.8 MSVC error C4458 (22 November 2026)

Po odstranění předchozí UHT chyby nastala při kompilaci pod Unreal Engine 5.8 další chyba:

`error C4458: declaration hides class member`

Čitelný výpis ukázal:
- `HorrorPlayer.cpp:597` — lokální proměnná `Mesh` skrývá `ACharacter::Mesh` (stejný název i v přepínání zbraní).
- `HorrorWorldDirector.cpp:346` — parametr lambda `Role` skrývá `AActor::Role`.
- Funkce `TakeDamage` — parametr `Instigator` skrývá `AActor::Instigator`.

## Oprava

- `Mesh` -> `AvatarPartMesh`, `RifleAsset`, `PistolAsset`, `SabrinaPartMesh`
- `Role` -> `BuildingKind`
- `Instigator` (parametry v projektových zdrojích) -> `EventInstigator`
- Doplňkové přejmenování lokálních ukazatelů v `DZAdultCompanion.cpp`.

Celkem bylo změněno 15 projektových C++ souborů. Kód byl zkontrolován staticky a vznikl ZIP s relativní cestou `Source/HorrorGame/...`, určený k rozbalení do adresáře obsahujícího `HorrorGame.uproject`.

## Pozor

**Nejde o potvrzenou úspěšnou kompilaci.** Tento asistent nemá nainstalovaný Windows Unreal Engine 5.8. Uživatel má spustit `SESTAVIT_A_ZJISTIT_CHYBU.bat` a případné další chyby poskytnout z `BUILD_ERRORS.txt` nebo `BUILD_FULL_LOG.txt`.

Veřejná pracovní větev prozatím obsahuje dokumentaci/opravné skripty, ne kompletní Unreal projekt a nelicencované assety.
