// Brandon Hillig 2026


#include "ALS/Character/ALSCharacter.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ActorComponents/AttributesComponent.h"
#include "ActorComponents/RSDamageFeedbackComponent.h"
#include "ActorComponents/RSPlayerVoiceComponent.h"
#include "ActorComponents/WeaponsComponent.h"
#include "Interact/RSInteractComponent.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable CVar_DebugGateSettings(TEXT("Debug.GateSettings"), false, TEXT("Debug gate setting movement variables"));

AALSCharacter::AALSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and setup components
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(SpringArm);

	InteractComponent = CreateDefaultSubobject<URSInteractComponent>("InteractComponent");
	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>("AttributesComponent");
	WeaponsComponent = CreateDefaultSubobject<UWeaponsComponent>("WeaponsComponent");
	DamageFeedbackComponent = CreateDefaultSubobject<URSDamageFeedbackComponent>("DamageFeedbackComponent");

	VoiceComponent = CreateDefaultSubobject<URSPlayerVoiceComponent>("VoiceComponent");
	VoiceComponent->SetupAttachment(GetMesh(), FName("head"));

	// Set capsule and mesh to ignore the Ground Trace Channel. Used for IK Traces and should ignore pawns
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Ground, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Ground, ECR_Ignore);
}

void AALSCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsDead);
}

void AALSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributesComponent->OnHealthChanged.AddDynamic(this, &AALSCharacter::OnHealthChanged);
	AttributesComponent->OnDeath.AddDynamic(this, &AALSCharacter::OnDeath);

	WeaponsComponent->OnWeaponAdded.AddDynamic(this, &AALSCharacter::OnWeaponAdded);
	WeaponsComponent->OnWeaponEquipped.AddDynamic(this, &AALSCharacter::OnWeaponEquipped);
	WeaponsComponent->OnWeaponUnequipped.AddDynamic(this, &AALSCharacter::OnWeaponUnequipped);
	WeaponsComponent->OnWeaponAnimationRequested.AddDynamic(this, &AALSCharacter::OnWeaponAnimationsRequested);
}

void AALSCharacter::BeginPlay()
{
	Super::BeginPlay();

	SwitchGate(EGate::Jogging);
	CurrentSpringArmLength = JoggingSpringArmLength;

	RefreshWeaponVisuals();

	if (WeaponsComponent && StartingWeaponData)
	{
		// Add starter weapon
		WeaponsComponent->TryAddWeapon(StartingWeaponData);
	}
}

void AALSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsDead)
	{
		return;
	}

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
			if (GetWorld()->SweepSingleByChannel(OutResult, StartFeetLocation, EndLocation, FQuat::Identity, ECC_Ground, CollisionShape, QueryParams))
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

	// Lerp Spring Arm Length
	const float GoalSpringArmLength = CurrentGate == EGate::Walking ? AimingSpringArmLength : JoggingSpringArmLength;
	const float AimSpeed = CurrentGate == EGate::Walking ? AimZoomInSpeed : AimZoomOutSpeed;
	CurrentSpringArmLength = FMath::FInterpTo(CurrentSpringArmLength, GoalSpringArmLength, DeltaSeconds, AimSpeed);
	if (SpringArm)
	{
		SpringArm->TargetArmLength = CurrentSpringArmLength;
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

		// Interact
		EnhancedInputComp->BindAction(InteractAction, ETriggerEvent::Started, this, &AALSCharacter::OnInteractStarted);

		// Crouching
		EnhancedInputComp->BindAction(ToggleCrouchAction, ETriggerEvent::Started, this, &AALSCharacter::OnCrouchToggled);

		// Fire Weapon
		EnhancedInputComp->BindAction(FireWeaponAction, ETriggerEvent::Triggered, this, &AALSCharacter::OnFireTriggered);

		// Reload Weapon
		EnhancedInputComp->BindAction(ReloadWeaponAction, ETriggerEvent::Started, this, &AALSCharacter::OnWeaponReloadStarted);

		// Equipping Weapons
		EnhancedInputComp->BindAction(UnequipWeaponAction, ETriggerEvent::Started, this, &AALSCharacter::OnUnequipWeaponPressed);
		EnhancedInputComp->BindAction(EquipPrimaryWeaponAction, ETriggerEvent::Started, this, &AALSCharacter::OnPrimaryWeaponEquippedPressed);
		EnhancedInputComp->BindAction(EquipSecondaryWeaponAction, ETriggerEvent::Started, this, &AALSCharacter::OnSecondaryWeaponEquippedPressed);
		EnhancedInputComp->BindAction(EquipScrollAction, ETriggerEvent::Triggered, this, &AALSCharacter::OnEquipScrollTriggered);

		// Toggle slow motion
		EnhancedInputComp->BindAction(ToggleSlowMotionAction, ETriggerEvent::Started, this, &AALSCharacter::ToggleSlowMotion);
	}

}

void AALSCharacter::GetWeaponAimRay(FVector& OutOrigin, FVector& OutDirection) const
{
	OutOrigin = FollowCamera->GetComponentLocation();
	OutDirection = FollowCamera->GetForwardVector();
}

UWeaponsComponent* AALSCharacter::GetWeaponsComponent() const
{
	return WeaponsComponent;
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

void AALSCharacter::OnInteractStarted()
{
	InteractComponent->TryInteract();
	WeaponsComponent->TryPickupWeapon();
}

void AALSCharacter::OnAimStarted()
{
	// Can not aim if no weapon is equipped
	if (!WeaponsComponent || !WeaponsComponent->HasWeaponEquipped())
	{
		return;
	}

	if (CurrentGate == EGate::Jogging)
	{
		// Enter walking state
		SwitchGate(EGate::Walking);
	}
}

void AALSCharacter::OnAimCompleted()
{
	if (CurrentGate == EGate::Walking)
	{
		// Enter jogging state
		SwitchGate(EGate::Jogging);
	}
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

void AALSCharacter::OnFireTriggered()
{
	// Make sure we are aiming a weapon
	if (CurrentGate != EGate::Walking)
	{
		return;
	}

	if (WeaponsComponent)
	{
		WeaponsComponent->TryFireWeapon();
	}
}

void AALSCharacter::OnWeaponReloadStarted()
{
	if (WeaponsComponent)
	{
		WeaponsComponent->TryReloadEquippedWeapon();
	}
}

void AALSCharacter::SwitchGate(EGate Gate)
{
	CurrentGate = Gate;
	OnGateSwitched.Broadcast(Gate);

	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();
	FGateSettings* CurrentGateSettings = GateSettingsMap.Find(Gate);
	if (CharacterMovementComp && CurrentGateSettings)
	{
		CharacterMovementComp->MaxWalkSpeed = CurrentGateSettings->MaxWalkSpeed;
		CharacterMovementComp->MaxAcceleration = CurrentGateSettings->MaxAcceleration;
		CharacterMovementComp->BrakingDecelerationWalking = CurrentGateSettings->BrakingDeceleration;
		CharacterMovementComp->BrakingFrictionFactor = CurrentGateSettings->BrakingFrictionFactor;
		CharacterMovementComp->BrakingFriction = CurrentGateSettings->BrakingFriction;
		CharacterMovementComp->bUseSeparateBrakingFriction = CurrentGateSettings->bUseSeparateBrakingFriction;
	}
}

void AALSCharacter::OnUnequipWeaponPressed()
{
	if (WeaponsComponent)
	{
		WeaponsComponent->TryUnequipWeapon();
	}
}

void AALSCharacter::OnPrimaryWeaponEquippedPressed()
{
	if (WeaponsComponent)
	{
		WeaponsComponent->TryEquipPrimaryWeapon();
	}
}

void AALSCharacter::OnSecondaryWeaponEquippedPressed()
{
	if (WeaponsComponent)
	{
		WeaponsComponent->TryEquipSecondaryWeapon();
	}
}

void AALSCharacter::OnEquipScrollTriggered(const FInputActionValue& Value)
{
	if (FMath::IsNearlyZero(Value.Get<float>()))
	{
		return;
	}

	const EWeaponSlot WeaponSlot = WeaponsComponent->GetEquippedWeaponSlot();
	if (WeaponSlot == EWeaponSlot::Secondary)
	{
		WeaponsComponent->TryEquipPrimaryWeapon();
	}
	else if (WeaponSlot == EWeaponSlot::Primary)
	{
		WeaponsComponent->TryEquipSecondaryWeapon();
	}
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

void AALSCharacter::OnWeaponAdded(const FWeapon& Weapon)
{
	const FWeaponConfig& WeaponConfig = Weapon.Data->Config;
	ensureMsgf(WeaponConfig.Mesh, TEXT("Weapon mesh must be set"));

	// Spawn a SkeletalMeshComponent for this weapon
	USkeletalMeshComponent* WeaponMeshComponent = NewObject<USkeletalMeshComponent>(this);
	WeaponMeshComponent->SetSkeletalMesh(WeaponConfig.Mesh);
	WeaponMeshComponent->ComponentTags.Add(WeaponConfig.MeshTag);
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	WeaponMeshComponent->RegisterComponent();
	WeaponMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GetUnequippedSocketName(WeaponConfig.WeaponSlot));
	WeaponMeshes.Add(WeaponConfig.WeaponSlot, WeaponMeshComponent);
}

void AALSCharacter::OnWeaponAnimationsRequested(EWeaponSlot WeaponSlot, UAnimSequenceBase* WeaponAnimation, UAnimMontage* CharacterAnimation)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && CharacterAnimation)
	{
		AnimInstance->Montage_Play(CharacterAnimation);
	}

	if (USkeletalMeshComponent* WeaponMesh = WeaponMeshes.FindRef(WeaponSlot); WeaponMesh && WeaponAnimation)
	{
		WeaponMesh->PlayAnimation(WeaponAnimation, false);
	}
}

void AALSCharacter::RefreshWeaponVisuals()
{
	if (!GetMesh() || !GetMesh()->GetAnimInstance()) return;

	if (const FWeapon* EquippedWeapon = WeaponsComponent->GetEquippedWeapon())
	{
		UpdateAnimInstanceForWeapon(EquippedWeapon->Data->Config.WeaponSlot);
		DetachAllWeaponMeshes();
		UpdateMeshLocationForWeapon(EquippedWeapon->Data->Config.WeaponSlot);
	}
	else
	{
		UpdateAnimInstanceForUnarmed();
	}
}

void AALSCharacter::UpdateMeshLocationForWeapon(EWeaponSlot WeaponSlot)
{
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	USkeletalMeshComponent* MeshComp = GetMesh();

	// Attach newly equipped weapon
	USkeletalMeshComponent* EquippedWeaponMesh = WeaponMeshes.FindRef(WeaponSlot);
	if (EquippedWeaponMesh)
	{
		const FName EquippedSocketName = WeaponSlot == EWeaponSlot::Primary ? WeaponSocketLocations.PrimaryEquipped : WeaponSocketLocations.SecondaryEquipped;
		EquippedWeaponMesh->AttachToComponent(MeshComp, AttachmentRules, EquippedSocketName);
	}
}

void AALSCharacter::OnWeaponEquipped(const FWeapon& Weapon)
{
	RefreshWeaponVisuals();
}

void AALSCharacter::OnWeaponUnequipped()
{
	RefreshWeaponVisuals();
}

void AALSCharacter::DetachAllWeaponMeshes()
{
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	USkeletalMeshComponent* MeshComp = GetMesh();

	// Detach all weapons
	for (const auto& [Type, WeaponMesh] : WeaponMeshes)
	{
		WeaponMesh->AttachToComponent(MeshComp, AttachmentRules, GetUnequippedSocketName(Type));
	}
}


void AALSCharacter::UpdateAnimInstanceForWeapon(EWeaponSlot WeaponSlot)
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (!CharacterMesh)
	{
		return;
	}

	TSubclassOf<UAnimInstance> LayerClass = nullptr;

	switch (WeaponSlot)
	{
	case EWeaponSlot::Primary:
		LayerClass = RifleAnimInstance;
		break;
	case EWeaponSlot::Secondary:
		LayerClass = PistolAnimInstance;
		break;
	default:
		break;
	}

	if (LayerClass)
	{
		CharacterMesh->LinkAnimClassLayers(LayerClass);
	}
}

void AALSCharacter::UpdateAnimInstanceForUnarmed()
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (!CharacterMesh)
	{
		return;
	}

	CharacterMesh->LinkAnimClassLayers(UnarmedAnimInstance);
}

FName AALSCharacter::GetUnequippedSocketName(EWeaponSlot WeaponSlot) const
{
	switch (WeaponSlot)
	{
	case EWeaponSlot::Primary:
		return WeaponSocketLocations.PrimaryUnequipped;
	case EWeaponSlot::Secondary:
		return WeaponSocketLocations.SecondaryUnequipped;
	default:
		break;
	}

	return "";
}

void AALSCharacter::OnHealthChanged(float NewHealth, float MaxHealth, float Delta, AController* EventInstigator, AActor* DamageCauser)
{
	rs::LogFloat("New Health", NewHealth, FColor::Green, 2.0f);
}

void AALSCharacter::OnDeath(AController* EventInstigator, AActor* DamageCauser)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	// Set dead
	bIsDead = true;

	SetLifeSpan(1.5f);
	
	HandleDeath();
}

void AALSCharacter::HandleDeath()
{
	// Disable collision
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Stop character movement
	if (UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement())
	{
		CharacterMovementComp->StopMovementImmediately();
		CharacterMovementComp->SetMovementMode(MOVE_None);
	}

	// Play death montage
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float Duration = AnimInstance->Montage_Play(DeathMontage);
		if (Duration > 0.f)
		{
			FOnMontageEnded Delegate;
			Delegate.BindUObject(this, &AALSCharacter::OnDeathMontageEnded);
			AnimInstance->Montage_SetEndDelegate(Delegate);
		}
	}

	// Zoom camera out
	SpringArm->TargetArmLength = 600.f;
}

void AALSCharacter::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// Lock corpse in death pose
	if (USkeletalMeshComponent* CharacterMeshComp = GetMesh())
	{
		CharacterMeshComp->SetComponentTickEnabled(false);
	}
}

void AALSCharacter::OnRep_IsDead()
{
	HandleDeath();
}
