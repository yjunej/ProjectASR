// Fill out your copyright notice in the Description page of Project Settings.


#include "UnlockRootRotation.h"
#include "ASR/Character/ASRCharacter.h"
#include "ASR/Character/TargetingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UUnlockRootRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Null Owner"));
		return;
	}
	AASRCharacter* Character = Cast<AASRCharacter>(Owner);
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Null AASRCharacter"));
		return;
	}

	UTargetingComponent* TargetingComponent = Character->GetTargetingComponent();

	if (TargetingComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Null TargetingComponent"));
		return;
	}

	if (TargetingComponent->IsTargeting())
	{
		return;
	}
	else
	{
		Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
		return;
	}
	return;
}

void UUnlockRootRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	{
		AActor* Owner = MeshComp->GetOwner();
		if (Owner == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Null Owner"));
			return;
		}
		AASRCharacter* Character = Cast<AASRCharacter>(Owner);
		if (Character == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Null AASRCharacter"));
			return;
		}

		UTargetingComponent* TargetingComponent = Character->GetTargetingComponent();

		if (TargetingComponent == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Null TargetingComponent"));
			return;
		}

	
		Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;

		return;
	}
}