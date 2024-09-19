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
	ACharacter* Character = Cast<ACharacter>(Owner);
	AASRCharacter* ASRCharacter = Cast<AASRCharacter>(Owner);
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Not Character"));
		return;
	}

	if (Character != nullptr && ASRCharacter == nullptr)
	{
		Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
		return;
	}

	UTargetingComponent* TargetingComp = ASRCharacter->GetTargetingComponent();
	if (TargetingComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Null TargetingComponent"));
		return;
	}

	// Rotate To Target
	if ((TargetingComp->IsTargeting() || TargetingComp->GetSubTargetActor()) && !bIgnoreTarget)
	{
		return;
	}

	// Disable Strafe, Ignore Target
	if (TargetingComp->IsTargeting() && bIgnoreTarget)
	{
		Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
		return;
	}

	Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
	return;
	
}

void UUnlockRootRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Null Owner"));
		return;
	}
	ACharacter* Character = Cast<ACharacter>(Owner);
	AASRCharacter* ASRCharacter = Cast<AASRCharacter>(Owner);
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Null AASRCharacter"));
		return;
	}

	if (Character != nullptr && ASRCharacter == nullptr)
	{
		Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
		return;
	}

	UTargetingComponent* TargetingComp = ASRCharacter->GetTargetingComponent();
	if (TargetingComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnLockRR AnimNotify Null TargetingComponent"));
		return;
	}

	Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	return;
}