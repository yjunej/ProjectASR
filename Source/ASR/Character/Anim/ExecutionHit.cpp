// Fill out your copyright notice in the Description page of Project Settings.


#include "ExecutionHit.h"
#include "ASR/Character/ASRCharacter.h"
#include "ASR/Character/TargetingComponent.h"
#include "ASR/Character/Enemy/BaseEnemy.h"

void UExecutionHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null Owner"));
		return;
	}
	AASRCharacter* Character = Cast<AASRCharacter>(Owner);
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null Character"));
		return;
	}

	UTargetingComponent* TargetingComponent = Character->GetTargetingComponent();

	if (TargetingComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null TargetingComponent"));
		return;
	}

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(TargetingComponent->GetTargetActor());
	if (Enemy != nullptr)
	{
		Enemy->Executed();
		TargetingComponent->ClearTarget();
	}

}
