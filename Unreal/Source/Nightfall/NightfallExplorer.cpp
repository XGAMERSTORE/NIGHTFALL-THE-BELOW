#include "NightfallExplorer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "InputCoreTypes.h"

ANightfallExplorer::ANightfallExplorer()
{
	GetCapsuleComponent()->InitCapsuleSize(38.0f, 96.0f);
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(GetCapsuleComponent());
	PlayerCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 68.0f));
	PlayerCamera->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = 430.0f;
	Movement->MaxWalkSpeedCrouched = 170.0f;
	Movement->JumpZVelocity = 420.0f;
	Movement->AirControl = 0.28f;
	Movement->BrakingDecelerationWalking = 1100.0f;
	Movement->bUseFlatBaseForFloorChecks = true;
}

void ANightfallExplorer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ANightfallExplorer::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ANightfallExplorer::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookYaw"), this, &ANightfallExplorer::LookYaw);
	PlayerInputComponent->BindAxis(TEXT("LookPitch"), this, &ANightfallExplorer::LookPitch);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
}

void ANightfallExplorer::MoveForward(float Value)
{
	if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		const FRotator YawOnly(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawOnly).GetUnitAxis(EAxis::X), Value);
	}
}

void ANightfallExplorer::MoveRight(float Value)
{
	if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		const FRotator YawOnly(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawOnly).GetUnitAxis(EAxis::Y), Value);
	}
}

void ANightfallExplorer::LookYaw(float Value)
{
	AddControllerYawInput(Value);
}

void ANightfallExplorer::LookPitch(float Value)
{
	AddControllerPitchInput(Value);
}
