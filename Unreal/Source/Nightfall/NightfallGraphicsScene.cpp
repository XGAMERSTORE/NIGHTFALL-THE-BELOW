#include "NightfallGraphicsScene.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"
#include "ProceduralMeshComponent.h"

namespace
{
	constexpr int32 TerrainQuads = 256;
	constexpr float TerrainWidth = 60000.0f;
	constexpr float CabinX = 7200.0f;
	constexpr float CabinY = 6500.0f;

	UMaterialInterface* BasicMaterial()
	{
		static UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		return Material ? Material : UMaterial::GetDefaultMaterial(MD_Surface);
	}

	UStaticMesh* EngineMesh(const TCHAR* Path)
	{
		return LoadObject<UStaticMesh>(nullptr, Path);
	}
}

ANightfallGraphicsScene::ANightfallGraphicsScene()
{
	PrimaryActorTick.bCanEverTick = false;
	Terrain = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RollingCountyTerrain"));
	SetRootComponent(Terrain);
	Terrain->bUseAsyncCooking = true;
	Terrain->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Terrain->SetCollisionObjectType(ECC_WorldStatic);
}

void ANightfallGraphicsScene::BeginPlay()
{
	Super::BeginPlay();
	BuildTerrain();
	BuildForest();
	BuildCabin();
	BuildAtmosphere();
}

float ANightfallGraphicsScene::HeightAt(float X, float Y)
{
	const float BroadNoise = FMath::PerlinNoise2D(FVector2D(X * 0.00011f, Y * 0.00011f));
	const float FineNoise = FMath::PerlinNoise2D(FVector2D(X * 0.00038f + 8.7f, Y * 0.00038f - 3.1f));
	const float Ridges = FMath::Max(0.0f, FMath::Abs(FMath::PerlinNoise2D(FVector2D(X * 0.00016f, Y * 0.00016f))) - 0.18f) * 2600.0f;
	const float Valley = -650.0f * FMath::Exp(-FMath::Square((Y + 6800.0f) / 5600.0f));
	return BroadNoise * 1500.0f + FineNoise * 220.0f + Ridges + Valley;
}

void ANightfallGraphicsScene::BuildTerrain()
{
	const int32 Side = TerrainQuads + 1;
	const float Step = TerrainWidth / TerrainQuads;
	const float Half = TerrainWidth * 0.5f;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> Colors;
	TArray<FProcMeshTangent> Tangents;
	Vertices.Reserve(Side * Side);
	Normals.Reserve(Side * Side);
	UVs.Reserve(Side * Side);
	Colors.Reserve(Side * Side);
	Tangents.Reserve(Side * Side);

	for (int32 Y = 0; Y < Side; ++Y)
	{
		for (int32 X = 0; X < Side; ++X)
		{
			const float LocalX = -Half + X * Step;
			const float LocalY = -Half + Y * Step;
			const float Z = HeightAt(LocalX, LocalY);
			Vertices.Emplace(LocalX, LocalY, Z);

			const FVector Dx(Step, 0.0f, HeightAt(LocalX + Step, LocalY) - HeightAt(LocalX - Step, LocalY));
			const FVector Dy(0.0f, Step, HeightAt(LocalX, LocalY + Step) - HeightAt(LocalX, LocalY - Step));
			Normals.Add(FVector::CrossProduct(Dx, Dy).GetSafeNormal());
			UVs.Emplace(LocalX / 3500.0f, LocalY / 3500.0f);
			const float HeightShade = FMath::GetMappedRangeValueClamped(FVector(-1000.0f, 2200.0f), FVector(0.0f, 1.0f), Z);
			Colors.Emplace(FMath::Lerp(0.18f, 0.42f, HeightShade), FMath::Lerp(0.22f, 0.34f, HeightShade), FMath::Lerp(0.16f, 0.30f, HeightShade), 1.0f);
			Tangents.Emplace(FVector(1.0f, 0.0f, (HeightAt(LocalX + Step, LocalY) - HeightAt(LocalX - Step, LocalY)) / (2.0f * Step)), false);
		}
	}

	Triangles.Reserve(TerrainQuads * TerrainQuads * 6);
	for (int32 Y = 0; Y < TerrainQuads; ++Y)
	{
		for (int32 X = 0; X < TerrainQuads; ++X)
		{
			const int32 A = Y * Side + X;
			const int32 B = A + 1;
			const int32 C = A + Side;
			const int32 D = C + 1;
			Triangles.Add(A);
			Triangles.Add(B);
			Triangles.Add(C);
			Triangles.Add(B);
			Triangles.Add(D);
			Triangles.Add(C);
		}
	}

	Terrain->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, Colors, Tangents, true, false);
	Terrain->SetMaterial(0, MakeTintedMaterial(FLinearColor(0.42f, 0.48f, 0.36f)));
}

UMaterialInterface* ANightfallGraphicsScene::MakeTintedMaterial(const FLinearColor& Tint) const
{
	UMaterialInterface* Base = BasicMaterial();
	UMaterialInstanceDynamic* Dynamic = UMaterialInstanceDynamic::Create(Base, const_cast<ANightfallGraphicsScene*>(this));
	if (Dynamic)
	{
		Dynamic->SetVectorParameterValue(TEXT("Color"), Tint);
		Dynamic->SetVectorParameterValue(TEXT("BaseColor"), Tint);
		Dynamic->SetScalarParameterValue(TEXT("Roughness"), 0.91f);
	}
	return Dynamic ? Dynamic : Base;
}

UStaticMeshComponent* ANightfallGraphicsScene::AddBlock(const FVector& LocalLocation, const FVector& Size, const FRotator& Rotation, const FLinearColor& Tint, bool bCollide)
{
	UStaticMeshComponent* Block = NewObject<UStaticMeshComponent>(this);
	Block->SetStaticMesh(EngineMesh(TEXT("/Engine/BasicShapes/Cube.Cube")));
	Block->SetupAttachment(RootComponent);
	Block->SetRelativeLocation(LocalLocation);
	Block->SetRelativeRotation(Rotation);
	Block->SetRelativeScale3D(Size / 100.0f);
	Block->SetCollisionEnabled(bCollide ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	Block->SetMobility(EComponentMobility::Static);
	Block->SetMaterial(0, MakeTintedMaterial(Tint));
	Block->RegisterComponent();
	GeneratedComponents.Add(Block);
	return Block;
}

void ANightfallGraphicsScene::BuildForest()
{
	UStaticMesh* Cube = EngineMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* Cone = EngineMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (!Cube || !Cone)
	{
		return;
	}

	UInstancedStaticMeshComponent* Trunks = NewObject<UInstancedStaticMeshComponent>(this, TEXT("ForestTrunks"));
	Trunks->SetupAttachment(RootComponent);
	Trunks->SetStaticMesh(Cube);
	Trunks->SetMaterial(0, MakeTintedMaterial(FLinearColor(0.16f, 0.12f, 0.10f)));
	Trunks->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Trunks->SetMobility(EComponentMobility::Static);
	Trunks->RegisterComponent();
	GeneratedComponents.Add(Trunks);

	UInstancedStaticMeshComponent* Canopies = NewObject<UInstancedStaticMeshComponent>(this, TEXT("ForestCanopies"));
	Canopies->SetupAttachment(RootComponent);
	Canopies->SetStaticMesh(Cone);
	Canopies->SetMaterial(0, MakeTintedMaterial(FLinearColor(0.075f, 0.12f, 0.095f)));
	Canopies->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Canopies->SetMobility(EComponentMobility::Static);
	Canopies->RegisterComponent();
	GeneratedComponents.Add(Canopies);

	FRandomStream Random(87119);
	for (int32 Index = 0; Index < 820; ++Index)
	{
		const float X = Random.FRandRange(-29000.0f, 29000.0f);
		const float Y = Random.FRandRange(-29000.0f, 29000.0f);
		if ((FVector2D(X, Y) - FVector2D(CabinX, CabinY)).Size() < 1700.0f || (X < -6200.0f && Y < -5600.0f))
		{
			continue;
		}
		const float Scale = Random.FRandRange(0.76f, 1.34f);
		const float Ground = HeightAt(X, Y);
		const FRotator Yaw(0.0f, Random.FRandRange(0.0f, 360.0f), 0.0f);
		Trunks->AddInstance(FTransform(Yaw, FVector(X, Y, Ground + 145.0f * Scale), FVector(0.22f * Scale, 0.22f * Scale, 2.9f * Scale)));
		Canopies->AddInstance(FTransform(Yaw, FVector(X, Y, Ground + 430.0f * Scale), FVector(1.85f * Scale, 1.85f * Scale, 4.0f * Scale)));
	}
}

void ANightfallGraphicsScene::BuildCabin()
{
	const float Ground = HeightAt(CabinX, CabinY);
	const FVector Origin(CabinX, CabinY, Ground);
	const FLinearColor Wood(0.23f, 0.19f, 0.15f);
	const FLinearColor DarkWood(0.13f, 0.12f, 0.11f);
	const FLinearColor Roof(0.10f, 0.11f, 0.12f);
	auto Add = [this, &Origin](const FVector& Offset, const FVector& Size, const FRotator& Rotation, const FLinearColor& Tint)
	{
		return AddBlock(Origin + Offset, Size, Rotation, Tint);
	};

	Add(FVector(0.0f, 0.0f, 12.0f), FVector(1040.0f, 900.0f, 24.0f), FRotator::ZeroRotator, DarkWood);
	Add(FVector(490.0f, 0.0f, 170.0f), FVector(24.0f, 900.0f, 320.0f), FRotator::ZeroRotator, Wood);
	Add(FVector(0.0f, -438.0f, 170.0f), FVector(1000.0f, 24.0f, 320.0f), FRotator::ZeroRotator, Wood);
	Add(FVector(0.0f, 438.0f, 170.0f), FVector(1000.0f, 24.0f, 320.0f), FRotator::ZeroRotator, Wood);

	// Leave a real opening in the front wall so the player can walk into the cabin.
	Add(FVector(-490.0f, -325.0f, 170.0f), FVector(24.0f, 250.0f, 320.0f), FRotator::ZeroRotator, Wood);
	Add(FVector(-490.0f, 325.0f, 170.0f), FVector(24.0f, 250.0f, 320.0f), FRotator::ZeroRotator, Wood);
	Add(FVector(-490.0f, 0.0f, 300.0f), FVector(24.0f, 400.0f, 60.0f), FRotator::ZeroRotator, Wood);
	Add(FVector(0.0f, -215.0f, 390.0f), FVector(1060.0f, 500.0f, 28.0f), FRotator(0.0f, 0.0f, 28.0f), Roof);
	Add(FVector(0.0f, 215.0f, 390.0f), FVector(1060.0f, 500.0f, 28.0f), FRotator(0.0f, 0.0f, -28.0f), Roof);
}

void ANightfallGraphicsScene::BuildAtmosphere()
{
	UDirectionalLightComponent* Sun = NewObject<UDirectionalLightComponent>(this, TEXT("ColdDuskSun"));
	Sun->SetupAttachment(RootComponent);
	Sun->SetMobility(EComponentMobility::Movable);
	Sun->SetIntensity(18000.0f);
	Sun->SetLightColor(FLinearColor(0.72f, 0.78f, 1.0f));
	Sun->SetRelativeRotation(FRotator(-8.0f, 28.0f, 0.0f));
	Sun->bCastShadows = true;
	Sun->RegisterComponent();
	GeneratedComponents.Add(Sun);

	USkyLightComponent* Sky = NewObject<USkyLightComponent>(this, TEXT("OvercastFill"));
	Sky->SetupAttachment(RootComponent);
	Sky->SetMobility(EComponentMobility::Movable);
	Sky->SourceType = ESkyLightSourceType::SLS_CapturedScene;
	Sky->SetIntensity(0.55f);
	Sky->RegisterComponent();
	Sky->RecaptureSky();
	GeneratedComponents.Add(Sky);

	UExponentialHeightFogComponent* Fog = NewObject<UExponentialHeightFogComponent>(this, TEXT("ColdCountyFog"));
	Fog->SetupAttachment(RootComponent);
	Fog->SetFogDensity(0.012f);
	Fog->SetFogHeightFalloff(0.16f);
	Fog->SetFogInscatteringColor(FLinearColor(0.22f, 0.27f, 0.32f));
	Fog->RegisterComponent();
	GeneratedComponents.Add(Fog);

	USkyAtmosphereComponent* SkyAtmosphere = NewObject<USkyAtmosphereComponent>(this, TEXT("CountySky"));
	SkyAtmosphere->SetupAttachment(RootComponent);
	SkyAtmosphere->RegisterComponent();
	GeneratedComponents.Add(SkyAtmosphere);
}
