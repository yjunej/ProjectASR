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
	if (Owner != nullptr)
	{
		bIsCrouched = Owner->bIsCrouched;
	}

	if (MovementComponent != nullptr)
	{
		FVector Velocity = MovementComponent->Velocity;
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
		bIsAccelerating = MovementComponent->GetCurrentAcceleration().Size() > 0.f ? true : false;
		bIsFalling = MovementComponent->IsFalling();
	}

}
