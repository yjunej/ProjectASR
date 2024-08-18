// Fill out your copyright notice in the Description page of Project Settings.


#include "ASRAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ASRCharacter.h"

void UASRAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner != nullptr)
	{
		MovementComponent = Owner->GetCharacterMovement();
	}
}

void UASRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MovementComponent != nullptr && Owner != nullptr)
	{
		FVector Velocity = MovementComponent->Velocity;
		Velocity.Z = 0.f;
		Speed = Velocity.Size();

		bIsCrouched = Owner->bIsCrouched;

		
		FVector Acceleration = MovementComponent->GetCurrentAcceleration();
		Acceleration2D = FVector2D(Acceleration.X, Acceleration.Y);

		DisplacementSinceLastUpdate = (Owner->GetActorLocation() - CurrentWorldLocation).Size2D();
		CurrentWorldLocation = Owner->GetActorLocation();
		DisplacementSpeed = DeltaSeconds != 0.f ? DisplacementSinceLastUpdate / DeltaSeconds : 0.f;

		bIsAccelerating = !Acceleration2D.IsNearlyZero();
		bIsFalling = MovementComponent->IsFalling();
	}

}
