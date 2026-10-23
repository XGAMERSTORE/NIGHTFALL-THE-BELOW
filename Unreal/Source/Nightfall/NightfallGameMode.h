#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NightfallGameMode.generated.h"

UCLASS()
class NIGHTFALL_API ANightfallGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANightfallGameMode();

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};
