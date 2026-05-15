// Brandon Hillig 2026


#include "ALS/Character/ALSCharacterAnimInstance.h"

#include "AnimCharacterMovementLibrary.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RestartThirdPerson/RestartThirdPerson.h"

static TAutoConsoleVariable CVar_DebugALSAnimInstance(TEXT("Debug.ALS"), false, TEXT("Show debug info for ALS AnimInstance"));

namespace
{
	constexpr float kForwardMaximum = 50.f;
	constexpr float kForwardMinimum = -50.f;
	constexpr float kBackwardsMaximum = 130.f;
	constexpr float kBackwardsMinimum = -130.f;
}

// Called on main thread, used for observers
void UALSCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if (AALSCharacter* Character = Cast<AALSCharacter>(TryGetPawnOwner()))
	{
		Character->OnGateSwitched.AddDynamic(this, &UALSCharacterAnimInstance::OnGateSwitched);
	}
}

void UALSCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AALSCharacter* Character = Cast<AALSCharacter>(TryGetPawnOwner()))
	{
		ALSCharacter = Character;
		CharacterMovementComponent = Character->GetCharacterMovement();
	}
}

void UALSCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CVar_DebugALSAnimInstance.GetValueOnAnyThread()) 
	{
		// Debug variables
		rs::LogFloat("Velocity Direction Angle", VelocityDirectionAngle, FColor::Yellow);
		rs::LogFloat("Acceleration Direction Angle", AccelerationDirectionAngle, FColor::Green);
		rs::LogFloat("Root Yaw Offset", RootYawOffset, FColor::Orange);
		rs::LogFloat("Time Remaining To Jump Apex", TimeRemainingToJumpApex, FColor::Emerald);
		rs::LogFloat("Distance From Ground", DistanceFromGround, FColor::Red);

		if (const APawn* PawnOwner = TryGetPawnOwner())
		{
			const FVector ActorLoc = PawnOwner->GetActorLocation();
			const FVector ActorFeetLoc = FVector(ActorLoc.X, ActorLoc.Y, 0);

			// Draw Ground Velocity
			const FVector GroundVelocity = FVector(Velocity.X, Velocity.Y, 0);
			const FVector EndLocationVelocity = ActorFeetLoc + GroundVelocity;

			rs::DrawDebugArrowWithText(ActorFeetLoc, EndLocationVelocity, TEXT("Ground Velocity"), PawnOwner->GetWorld(), FColor::Yellow);

			// Draw Ground Acceleration
			const FVector EndLocationAcceleration = ActorFeetLoc + GroundAcceleration;
			rs::DrawDebugArrowWithText(ActorFeetLoc, EndLocationAcceleration, TEXT("Ground Acceleration"), PawnOwner->GetWorld(), FColor::Emerald);

			// Draw Predicted Stop Location
			const FVector EndLocationPredictedStop = ActorLoc + PredictedGroundStopLocation;
			if (EndLocationPredictedStop != ActorLoc)
			{
				UKismetSystemLibrary::DrawDebugCapsule(this, EndLocationPredictedStop, 20.f, 20.f, FRotator::ZeroRotator, FColor::Orange, 0.f, 2);
			}
		}
	}
}

void UALSCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (!CharacterMovementComponent)
	{
		return;
	}

	// Get Last Frame Location
	LastFrameLocation = Location;

	// Get Location
	Location = TryGetPawnOwner()->GetActorLocation();

	// Get Delta Distance
	DeltaDistance = (Location - LastFrameLocation).Length();

	// Get Velocity
	Velocity = CharacterMovementComponent->Velocity;

	// Get Acceleration
	Acceleration = CharacterMovementComponent->GetCurrentAcceleration();

	// Get Ground Acceleration
	GroundAcceleration = FVector(Acceleration.X, Acceleration.Y, 0);

	// Get Is Accelerating
	bIsAccelerating = Acceleration.Length() >= 0.1f;

	// Get Ground Speed
	const FVector GroundVelocity = FVector(Velocity.X, Velocity.Y, 0);
	GroundSpeed = GroundVelocity.Length();

	const FRotator ActorRotation = TryGetPawnOwner()->GetActorRotation();

	// Get VelocityDotAcceleration
	VelocityDotAcceleration = FVector::DotProduct(GroundVelocity.GetSafeNormal(), GroundAcceleration.GetSafeNormal());

	// Get Velocity Direction Angle
	VelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation);

	// Get Velocity Direction Angle With Offset
	VelocityDirectionAngleWithOffset = FRotator::NormalizeAxis(VelocityDirectionAngle - RootYawOffset);

	// Get Acceleration Direction Angle
	AccelerationDirectionAngle = UKismetAnimationLibrary::CalculateDirection(Acceleration, ActorRotation);

	// Get Last Frame Velocity Locomotion Direction
	LastFrameVelocityLocomotionDirection = VelocityLocomotionDirection;

	// Get Velocity Locomotion Direction
	VelocityLocomotionDirection = CalculateLocomotionDirection(VelocityDirectionAngle, 20.f, VelocityLocomotionDirection);

	// Get Changed Locomotion Direction
	bChangedLocomotionDirection = LastFrameVelocityLocomotionDirection != VelocityLocomotionDirection;

	// Get Acceleration Locomotion Direction
	AccelerationLocomotionDirection = CalculateLocomotionDirection(AccelerationDirectionAngle, 20.f, AccelerationLocomotionDirection);

	// Get Last Frame Actor Yaw
	LastFrameActorYaw = ActorYaw;
	
	// Get Actor Yaw
	ActorYaw = ActorRotation.Yaw;

	// Get Delta Actor Yaw
	DeltaActorYaw = ActorYaw - LastFrameActorYaw;

	// Get RootYawOffset
	UpdateRootYawOffset(DeltaSeconds);

	// Get Aim Pitch
	AimPitch = ALSCharacter->GetBaseAimRotation().Pitch;

	// Get Lean Angle
	LeanAngle = DeltaActorYaw / DeltaSeconds / 6.0f;
	LeanAngle = UKismetMathLibrary::ClampAngle(LeanAngle, -90.f, 90.f);
	if (VelocityLocomotionDirection == ELocomotionDirection::Backward)
	{
		LeanAngle *= -1.f;
	}

	// Get Predicted Stop Location
	PredictedGroundStopLocation = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(GroundVelocity,
		CharacterMovementComponent->bUseSeparateBrakingFriction,
		CharacterMovementComponent->BrakingFriction,
		CharacterMovementComponent->GroundFriction,
		CharacterMovementComponent->BrakingFrictionFactor,
		CharacterMovementComponent->BrakingDecelerationWalking);

	// Get Distance Until Stop
	DistanceUntilStop = PredictedGroundStopLocation.Length();

	// Get Last Frame Gate
	LastFrameGate = CurrentGate;

	// Get Current Gate
	CurrentGate = IncomingGate;

	// Get Changed Gate
	bChangedGate = LastFrameGate != CurrentGate;

	// Get Last Frame Is Crouching
	bLastFrameIsCrouching = bIsCrouching;

	// Get Is Crouching
	bIsCrouching = CurrentGate == EGate::Crouching;

	// Get Crouching Gate Changed
	bCrouchGateChanged = bLastFrameIsCrouching != bIsCrouching;

	// Get Is In Air
	bIsInAir = CharacterMovementComponent->MovementMode == MOVE_Falling;

	// Get Is Jumping
	bIsJumping = bIsInAir && Velocity.Z >= 0.1f;

	// Get Is Falling
	bIsFalling = bIsInAir && Velocity.Z <= -0.1f;

	// Get Time Remaining To Jump Apex
	TimeRemainingToJumpApex = bIsJumping ? -Velocity.Z / (CharacterMovementComponent->GetGravityZ() * CharacterMovementComponent->GravityScale) : 0.f;

	// Get Distance From Ground
	DistanceFromGround = ALSCharacter->GetDistanceFromGround();
	
}

void UALSCharacterAnimInstance::OnGateSwitched(EGate Gate)
{
	IncomingGate = Gate;
}

ELocomotionDirection UALSCharacterAnimInstance::CalculateLocomotionDirection(float Angle, float DeadZone, ELocomotionDirection CurrentDirection)
{
	// Check for small adjustments within our deadzone first to mitigate snapping between animations
	switch (CurrentDirection)
	{
	case ELocomotionDirection::Forward:
		if (UKismetMathLibrary::InRange_FloatFloat(Angle, kForwardMinimum - DeadZone, kForwardMaximum + DeadZone))
		{
			return CurrentDirection;
		}
		break;
	case ELocomotionDirection::Right:
		if (UKismetMathLibrary::InRange_FloatFloat(Angle, kForwardMaximum - DeadZone, kBackwardsMaximum + DeadZone))
		{
			return CurrentDirection;
		}
		break;
	case ELocomotionDirection::Left:
		if (UKismetMathLibrary::InRange_FloatFloat(Angle, kBackwardsMinimum - DeadZone, kForwardMinimum + DeadZone))
		{
			return CurrentDirection;
		}
		break;
	case ELocomotionDirection::Backward:
		if (UKismetMathLibrary::InRange_FloatFloat(Angle, kBackwardsMinimum + DeadZone, -180.f) 
			|| UKismetMathLibrary::InRange_FloatFloat(Angle, kBackwardsMaximum - DeadZone, 180.f))
		{
			return CurrentDirection;
		}
		break;
	}

	if (UKismetMathLibrary::InRange_FloatFloat(Angle, kForwardMinimum, kForwardMaximum))
	{
		return ELocomotionDirection::Forward;
	}
	if (UKismetMathLibrary::InRange_FloatFloat(Angle, kForwardMaximum, kBackwardsMaximum))
	{
		return ELocomotionDirection::Right;
	}
	if (UKismetMathLibrary::InRange_FloatFloat(Angle, kBackwardsMinimum, kForwardMinimum))
	{
		return ELocomotionDirection::Left;
	}

	return ELocomotionDirection::Backward;
}

void UALSCharacterAnimInstance::UpdateRootYawOffset(float DeltaSeconds)
{
	// Update RootYawOffset depending on the mode (e.g idle state overrides RootYawOffsetMode to Accumulate)
	switch (RootYawOffsetMode)
	{
	case ERootYawOffsetMode::Accumulate:
		RootYawOffset = FRotator::NormalizeAxis(RootYawOffset - DeltaActorYaw);
		break;
	case ERootYawOffsetMode::BlendOut:
		RootYawOffset = UKismetMathLibrary::FloatSpringInterp(RootYawOffset, 0.f, RootYawOffsetToZeroSpringState, 80.f, 1.f, DeltaSeconds, 1.f, 0.5f);
		break;
	case ERootYawOffsetMode::Hold:
		break;
	}

	// Set RootYawOffsetMode to BlendOut
	RootYawOffsetMode = ERootYawOffsetMode::BlendOut;
}
