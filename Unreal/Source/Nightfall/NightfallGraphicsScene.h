#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NightfallGraphicsScene.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;
class UStaticMeshComponent;
class UActorComponent;

UCLASS()
class NIGHTFALL_API ANightfallGraphicsScene : public AActor
{
	GENERATED_BODY()

public:
	ANightfallGraphicsScene();
	static float HeightAt(float X, float Y);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category="World")
	TObjectPtr<UProceduralMeshComponent> Terrain;

	UPROPERTY()
	TArray<TObjectPtr<UActorComponent>> GeneratedComponents;

	void BuildTerrain();
	void BuildForest();
	void BuildCabin();
	void BuildAtmosphere();
	UStaticMeshComponent* AddBlock(const FVector& LocalLocation, const FVector& Size, const FRotator& Rotation, const FLinearColor& Tint, bool bCollide = true);
	UMaterialInterface* MakeTintedMaterial(const FLinearColor& Tint) const;
};
