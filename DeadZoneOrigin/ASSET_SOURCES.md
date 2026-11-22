# DEAD ZONE — realističtější grafické assety (zdroje ověřené 2026-11-22)

**Tyto assety nejsou přibalené.** Služby MetaHuman, Fab a Mixamo vyžadují příslušné účty a přijetí licence. Ceny a dostupnost u jednotlivých nabídek se mohou měnit.

| Účel | Zdroj | Využití |
|---|---|---|
| Hlavní postava, dospělí spojenci | [MetaHuman Creator](https://dev.epicgames.com/documentation/en-us/metahuman/metahuman-creator) | Skutečně rigovaní lidé, pokožka, vlasy, mimika; UE 5.6+ |
| Zombie člověk, základ zdarma | [Zombie Female: Nurse](https://www.fab.com/listings/35246f73-70ca-41ac-816b-55ead87dbcbc) | Dodavatel uvádí bezplatný vzorek s idle, walk, attack; získat přes Fab |
| Zombie člověk, kvalitnější alternativa | [Zombie Custom MetaHuman](https://www.fab.com/listings/5c3db69f-b962-4d3b-9a62-0e939e9b25e8) | 8K PBR, rigovaný character; cenu a licence ověřit u nabídky |
| Zombie člověk, další alternativa | [Male MetaHuman Zombie Mad Patient 0](https://www.fab.com/listings/ccc3018c-3155-405d-857a-23252e811e2a) | MetaHuman zombie s variantami pro film a realtime |
| Humanoidní chůze, běh, zásahy | [Adobe Mixamo](https://www.mixamo.com/) | Animace pro humanoidy; ne pro čtyřnohá zvířata |
| Ruiny / zdi | [Poly Haven Damaged Plaster](https://polyhaven.com/a/damaged_plaster) | CC0 PBR |
| Fasády | [Poly Haven Cracked Concrete Wall](https://polyhaven.com/a/cracked_concrete_wall) | CC0 PBR |
| Dveře / rezavý kov | [Poly Haven Rusty Metal](https://polyhaven.com/a/rusty_metal) | CC0 PBR |
| Zničené ulice | [Poly Haven Rubble](https://polyhaven.com/a/rubble) | CC0 PBR |

## Doporučený postup
1. Sestav hráče a dospělou magickou spojenkyni jako vlastní postavy v MetaHuman Creator (nehledej neautorizovaný scan tváře herce či herečky).
2. Získej přes svůj účet na Fab humanoidní zombie, importuj do UE 5.8.
3. Použij IK Retargeter pro animace, Animation Blueprint pro idle/run/attack/death, vytvoř LOD varianty.
4. Pro zombie zvířata hledej vhodný *skeletal mesh s quadruped/bird rigem*. Mixamo je pro dvounohé humanoidy.
5. Pro exteriéry použij Base Color, Normal (DX), Roughness, AO/Metallic z CC0 textur Poly Haven.

## Cílové cesty (Content Browser)
- `/Game/Production/Characters/Survivor/SK_Survivor`
- `/Game/Production/Characters/Sabrina/SK_Sabrina`
- `/Game/Production/Characters/ZombieHuman/SK_ZombieHuman`
- `/Game/Production/Characters/ZombieDog/SK_ZombieDog`
- `/Game/Production/Characters/ZombieCrow/SK_ZombieCrow`
- `/Game/Production/Characters/ZombieDeer/SK_ZombieDeer`
- `/Game/Production/Characters/Mutant/SK_Mutant`

Nejde o downloadovací skript: Epic/Fab assety musí být získány oprávněně a nesmějí se bez souhlasu kopírovat do veřejného repozitáře. Pro skutečnou kvalitu MetaHuman nestačí přiřazení jednoho tělového Skeletal Meshe; je potřeba sestavený MetaHuman Blueprint s komponentami hlavy, vlasů a animací.
