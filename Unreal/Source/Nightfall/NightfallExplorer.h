#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NightfallExplorer.generated.h"

class UCameraComponent;

UCLASS()
class NIGHTFALL_API ANightfallExplorer : public ACharacter
{
	GENERATED_BODY()

public:
	ANightfallExplorer();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> PlayerCamera;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookYaw(float Value);
	void LookPitch(float Value);
};
