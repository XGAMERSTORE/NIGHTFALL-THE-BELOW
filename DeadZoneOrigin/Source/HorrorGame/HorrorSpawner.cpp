#include "HorrorSpawner.h"
#include "HorrorPulse.h"
#include "DeadZoneVisuals.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"

AHorrorSpawner::AHorrorSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("NestRoot"));
    Base = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InfectedRoots"));
    Base->SetupAttachment(RootComponent);
    Base->SetStaticMesh(DeadZoneVisuals::Shape(TEXT("Cylinder")));
    Base->SetRelativeScale3D(FVector(3.5f,3.5f,3.f));
    Tumor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Heart"));
    Tumor->SetupAttachment(RootComponent);
    Tumor->SetStaticMesh(DeadZoneVisuals::Shape(TEXT("Sphere")));
    Tumor->SetRelativeLocation(FVector(0,0,200));
    Tumor->SetRelativeScale3D(FVector(2.5f));
    Glow = CreateDefaultSubobject<UPointLightComponent>(TEXT("CorruptedLight"));
    Glow->SetupAttachment(RootComponent);
    Glow->SetRelativeLocation(FVector(0,0,240));
    Glow->SetIntensity(16000.f);
    Glow->SetAttenuationRadius(2200.f);
    Glow->SetLightColor(FLinearColor(1.f, .012f, .01f));
}
void AHorrorSpawner::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
    Base->SetMaterial(0, DeadZoneVisuals::ColorMaterial(this, FLinearColor(.16f,.09f,.08f)));
    UMaterialInterface* Infection=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Generated/Materials/M_Infection.M_Infection"));
    Tumor->SetMaterial(0,Infection?Infection:DeadZoneVisuals::ColorMaterial(this,FLinearColor(.8f,.025f,.03f)));
    if (SpeciesPool.IsEmpty()) SpeciesPool = { EEnemySpecies::ZombieHuman, EEnemySpecies::ZombieHuman, EEnemySpecies::ZombieDog, EEnemySpecies::ZombieDeer, EEnemySpecies::ZombieCrow, EEnemySpecies::Mutant };
    GetWorldTimerManager().SetTimer(SpawnTimer, this, &AHorrorSpawner::SpawnTick, SpawnInterval, true, FMath::FRandRange(0.2f,3.f));
}
void AHorrorSpawner::SpawnTick()
{
    if (Alive >= MaxAlive) return;
    APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!Player || FVector::DistSquared2D(Player->GetActorLocation(), GetActorLocation()) > ActivationRange*ActivationRange) return;
    const FVector2D Off = FMath::RandPointInCircle(SpawnRadius);
    const FTransform SpawnTransform(FRotator(0, FMath::FRandRange(0.f, 360.f),0), GetActorLocation() + FVector(Off.X,Off.Y,250));
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    // Keep the class pointer type consistent: UClass* vs. TSubclassOf would make ?: ambiguous on MSVC.
    UClass* ClassToSpawn = AHorrorEnemy::StaticClass();
    if (!EnemyClasses.IsEmpty())
    {
        const int32 ChosenIndex = FMath::RandRange(0, EnemyClasses.Num() - 1);
        if (UClass* CustomEnemyClass = EnemyClasses[ChosenIndex].Get())
        {
            ClassToSpawn = CustomEnemyClass;
        }
    }
    AHorrorEnemy* Enemy = GetWorld()->SpawnActorDeferred<AHorrorEnemy>(ClassToSpawn, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
    if (Enemy)
    {
        Enemy->Species = SpeciesPool[FMath::RandRange(0, SpeciesPool.Num()-1)];
        Enemy->DetectionRange = 5500.f;
        UGameplayStatics::FinishSpawningActor(Enemy, SpawnTransform);
        ++Alive;
        Enemy->OnEnemyDied.AddDynamic(this, &AHorrorSpawner::HandleEnemyDied);
    }
}
void AHorrorSpawner::HandleEnemyDied(AHorrorEnemy*) { Alive = FMath::Max(0,Alive-1); }
float AHorrorSpawner::TakeDamage(float Damage, FDamageEvent const& Event, AController* EventInstigator, AActor* Causer)
{
    if (Damage <= 0) return 0;
    float Applied = Super::TakeDamage(Damage, Event, EventInstigator, Causer);
    Health -= Applied;
    if (Health <= 0) DestroySpawner();
    return Applied;
}
void AHorrorSpawner::DestroySpawner()
{
    GetWorldTimerManager().ClearTimer(SpawnTimer);
    OnSpawnerDestroyed.Broadcast(this);
    if (AHorrorPulse* Pulse = GetWorld()->SpawnActor<AHorrorPulse>(GetActorLocation()+FVector(0,0,200), FRotator::ZeroRotator)) Pulse->SetBlastColor(FLinearColor(1.f,.02f,.01f));
    Destroy();
}
