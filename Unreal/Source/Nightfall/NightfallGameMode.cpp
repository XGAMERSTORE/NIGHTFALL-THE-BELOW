#include "NightfallGameMode.h"
#include "NightfallExplorer.h"
#include "NightfallGraphicsScene.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"

ANightfallGameMode::ANightfallGameMode()
{
	DefaultPawnClass = ANightfallExplorer::StaticClass();
}

void ANightfallGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	if (!GetWorld())
	{
		return;
	}

	GetWorld()->SpawnActor<ANightfallGraphicsScene>(FVector::ZeroVector, FRotator::ZeroRotator);
	const float StartX = -7600.0f;
	const float StartY = -7000.0f;
	const float StartZ = ANightfallGraphicsScene::HeightAt(StartX, StartY) + 180.0f;
	GetWorld()->SpawnActor<APlayerStart>(FVector(StartX, StartY, StartZ), FRotator(0.0f, 38.0f, 0.0f));
}
