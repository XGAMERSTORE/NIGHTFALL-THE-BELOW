# Unreal Engine 5.8 — oprava UHT chyby AllowedClasses

**Pozor:** jde o dokumentovanou opravnou záplatu, ne potvrzení, že celý projekt už jde zkompilovat.

Podle výpisu z Windows se UnrealHeaderTool zastaví na:
`Source/HorrorGame/DZSabrinaCompanion.h(46): MetaData AllowedClasses has a value 'SkeletalMesh' which is not a valid long path name`.

V hlavičce `DZSabrinaCompanion.h` změnit:

```diff
- UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sabrina | Cinematic", meta=(AllowedClasses="SkeletalMesh"))
+ UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sabrina | Cinematic")
  TSoftObjectPtr<USkeletalMesh> OptionalCinematicMesh;
```

`TSoftObjectPtr<USkeletalMesh>` už určuje správný typ assetu, takže `AllowedClasses` není potřeba.

**Validace:** textová kontrola, syntaxe Python importních skriptů a ZIP integrity. Úplná kompilace Windows/UE 5.8 zatím neověřena.

**Test:** zavřít editor, opravit soubor, spustit `SESTAVIT_A_ZJISTIT_CHYBU.bat`, a v případě další chyby zkontrolovat `BUILD_ERRORS.txt`.
