// Brandon Hillig 2026


#include "ALS/Character/ALSCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "RestartThirdPerson/RestartThirdPerson.h"

static TAutoConsoleVariable CVar_DebugGateSettings(TEXT("Debug.GateSettings"), false, TEXT("Debug gate setting movement variables"));

AALSCharacter::AALSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and setup components
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(SpringArm);
}

void AALSCharacter::BeginPlay()
{
	Super::BeginPlay();

	SwitchGate(EGate::Jogging);
	SwitchWeapon(EWeapon::Unarmed);
}

void AALSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CVar_DebugGateSettings.GetValueOnGameThread())
	{
		rs::LogTick(FString::Printf(TEXT("Max Walk Speed: %f"), GetCharacterMovement()->MaxWalkSpeed), 1, FColor::Emerald);
		rs::LogTick(FString::Printf(TEXT("Max Acceleration: %f"), GetCharacterMovement()->MaxAcceleration), 2, FColor::Green);
		rs::LogTick(FString::Printf(TEXT("Braking Deceleration: %f"), GetCharacterMovement()->BrakingDecelerationWalking), 3, FColor::Orange);
		rs::LogTick(FString::Printf(TEXT("Braking Friction Factor: %f"), GetCharacterMovement()->BrakingFrictionFactor), 4, FColor::Yellow);
		rs::LogTick(FString::Printf(TEXT("Braking Friction: %f"), GetCharacterMovement()->BrakingFriction), 5, FColor::White);
		rs::LogTick(FString::Printf(TEXT("Use Separate Braking Friction: %s"), GetCharacterMovement()->bUseSeparateBrakingFriction ? TEXT("TRUE") : TEXT("FALSE")), 6, FColor::Red);
	}


	// Calculate Distance From Ground (if we are falling)
	if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			const FVector StartFeetLocation = GetActorLocation() - FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight());
			const FVector EndLocation = StartFeetLocation + FVector::DownVector * 50'000.f;

			// Perform Line Trace
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);

			FCollisionShape CollisionShape;
			CollisionShape.SetSphere(30.f);

			FHitResult OutResult;
			GetWorld()->SweepSingleByChannel(OutResult, StartFeetLocation, EndLocation, FQuat::Identity, ECC_Visibility, CollisionShape, QueryParams);

			if (OutResult.bBlockingHit)
			{
				DistanceFromGround = OutResult.Distance;
			}
			else
			{
				DistanceFromGround = -1.f;
			}
		}

	}
	else
	{
		DistanceFromGround = 0.f;
	}
}

void AALSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* APC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInput = APC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			EnhancedInput->AddMappingContext(ALSInputMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Walking and looking around
		EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AALSCharacter::OnMoveTriggered);
		EnhancedInputComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &AALSCharacter::OnLookTriggered);

		// Aiming
		EnhancedInputComp->BindAction(AimAction, ETriggerEvent::Started, this, &AALSCharacter::OnAimStarted);
		EnhancedInputComp->BindAction(AimAction, ETriggerEvent::Completed, this, &AALSCharacter::OnAimCompleted);

		// Jumping
		EnhancedInputComp->BindAction(JumpAction, ETriggerEvent::Started, this, &AALSCharacter::Jump);
		EnhancedInputComp->BindAction(JumpAction, ETriggerEvent::Completed, this, &AALSCharacter::StopJumping);

		// Crouching
		EnhancedInputComp->BindAction(ToggleCrouchAction, ETriggerEvent::Started, this, &AALSCharacter::OnCrouchToggled);

		// Equipping weapons
		EnhancedInputComp->BindAction(UnequipWeaponAction, ETriggerEvent::Started, this, &AALSCharacter::OnUnequipWeaponPressed);
		EnhancedInputComp->BindAction(EquipPrimaryWeaponAction, ETriggerEvent::Started, this, &AALSCharacter::OnPrimaryWeaponEquippedPressed);
		EnhancedInputComp->BindAction(EquipSecondaryWeaponAction, ETriggerEvent::Started, this, &AALSCharacter::OnSecondaryWeaponEquippedPressed);

		// Toggle slow motion
		EnhancedInputComp->BindAction(ToggleSlowMotionAction, ETriggerEvent::Started, this, &AALSCharacter::ToggleSlowMotion);
	}

}

void AALSCharacter::OnMoveTriggered(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	const FRotator ControlYawRot = FRotator(0, GetControlRotation().Yaw, 0);
	const FRotationMatrix Mat(ControlYawRot);

	AddMovementInput(Mat.GetUnitAxis(EAxis::X), Axis.Y); // Forward
	AddMovementInput(Mat.GetUnitAxis(EAxis::Y), Axis.X); // Right
}

void AALSCharacter::OnLookTriggered(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AALSCharacter::OnAimStarted()
{
	// Enter walking state
	SwitchGate(EGate::Walking);
}

void AALSCharacter::OnAimCompleted()
{
	// Enter jogging state
	SwitchGate(EGate::Jogging);
}

void AALSCharacter::OnCrouchToggled()
{
	if (CurrentGate == EGate::Crouching)
	{
		// Enter jogging state
		SwitchGate(EGate::Jogging);

		// Uncrouch the character
		UnCrouch();
		return;
	}

	// Enter crouching state
	SwitchGate(EGate::Crouching);

	// Crouch the character
	Crouch();
}

void AALSCharacter::SwitchGate(EGate Gate)
{
	CurrentGate = Gate;
	OnGateSwitched.Broadcast(Gate);

	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();
	if (!CharacterMovementComp)
	{
		return;
	}

	CharacterMovementComp->MaxWalkSpeed = GateSettingsMap[Gate].MaxWalkSpeed;
	CharacterMovementComp->MaxAcceleration = GateSettingsMap[Gate].MaxAcceleration;
	CharacterMovementComp->BrakingDecelerationWalking = GateSettingsMap[Gate].BrakingDeceleration;
	CharacterMovementComp->BrakingFrictionFactor = GateSettingsMap[Gate].BrakingFrictionFactor;
	CharacterMovementComp->BrakingFriction = GateSettingsMap[Gate].BrakingFriction;
	CharacterMovementComp->bUseSeparateBrakingFriction = GateSettingsMap[Gate].bUseSeparateBrakingFriction;
}

void AALSCharacter::OnUnequipWeaponPressed()
{
	SwitchWeapon(EWeapon::Unarmed);
}

void AALSCharacter::OnPrimaryWeaponEquippedPressed()
{
	SwitchWeapon(EWeapon::Rifle);
}

void AALSCharacter::OnSecondaryWeaponEquippedPressed()
{
	SwitchWeapon(EWeapon::Pistol);
}

void AALSCharacter::ToggleSlowMotion()
{
	const float TimeDilation = UGameplayStatics::GetGlobalTimeDilation(this);
	if (TimeDilation <= 0.9f)
	{
		// Turn off slow motion
		UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
	}
	else
	{
		// Turn on slow motion
		UGameplayStatics::SetGlobalTimeDilation(this, 0.1f);
	}
}

void AALSCharacter::SwitchWeapon(EWeapon Weapon)
{
	CurrentWeapon = Weapon;
	OnWeaponSwitched.Broadcast(Weapon);
	UpdateAnimInstanceForWeapon(Weapon);
}

void AALSCharacter::UpdateAnimInstanceForWeapon(EWeapon Weapon)
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (!CharacterMesh)
	{
		return;
	}

	TSubclassOf<UAnimInstance> LayerClass = nullptr;

	switch (Weapon)
	{
	case EWeapon::Pistol:
		LayerClass = PistolAnimInstance;
		break;
	case EWeapon::Rifle:
		LayerClass = RifleAnimInstance;
		break;
	case EWeapon::Unarmed:
	default:
		LayerClass = UnarmedAnimInstance;
		break;
	}

	CharacterMesh->LinkAnimClassLayers(LayerClass);
}

