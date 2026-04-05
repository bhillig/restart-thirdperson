// Brandon Hillig 2026


#include "Character/PlayerCharacter/RestartPlayerCharacter.h"


#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "RestartThirdPerson/RestartThirdPerson.h"

ARestartPlayerCharacter::ARestartPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and attach components
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(SpringArm);
}

void ARestartPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set input mapping context
	if (APlayerController* APC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(APC->GetLocalPlayer()))
		{
			if (DefaultInputMappingContext)
			{
				EnhancedInputSubsystem->AddMappingContext(DefaultInputMappingContext, 0);
			}
			else
			{
				rs::LogOnce(FString::Printf(TEXT("DefaultIMC is not set!")), FColor::Red);
			}
		}
	}

	// Bind enhanced input actions
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARestartPlayerCharacter::OnMove);
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ARestartPlayerCharacter::OnTurn);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARestartPlayerCharacter::OnJump);
	}
}

void ARestartPlayerCharacter::OnMove(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	const FRotator ControlRot = GetControlRotation();
	const FRotator ControlRotYaw = FRotator(0, ControlRot.Yaw, 0);

	const FRotationMatrix RotationMat(ControlRotYaw);

	const FVector CameraForward = RotationMat.GetUnitAxis(EAxis::Type::X);
	const FVector CameraRight = RotationMat.GetUnitAxis(EAxis::Type::Y);

	// Move Forward/Back
	AddMovementInput(CameraForward, Axis.X);

	// Move Right/Left
	AddMovementInput(CameraRight, Axis.Y);
}

void ARestartPlayerCharacter::OnTurn(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	AddControllerPitchInput(Axis.Y);
	AddControllerYawInput(Axis.X);
}

void ARestartPlayerCharacter::OnJump(const FInputActionValue& Value)
{
	Jump();
}

