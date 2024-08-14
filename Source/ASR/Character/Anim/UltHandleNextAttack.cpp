// Fill out your copyright notice in the Description page of Project Settings.


#include "UltHandleNextAttack.h"
#include "ASR/Character/Blader.h"
#include "MotionWarpingComponent.h"

void UUltHandleNextAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UltNextAttack AnimNotify Null Owner"));
		return;
	}
	ABlader* Blader = Cast<ABlader>(Owner);
	if (Blader == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UltNextAttack AnimNotify Null Blader"));
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UltNextAttack AnimNotify Null AnimInstance"));
		return;
	}

	TArray<AActor*> UltTargets = Blader->GetUltTargets();
	int32 UltIndex = Blader->GetUltTargetIndex();

	Blader->SetUltTargetIndex(UltIndex + 1);


	if (UltIndex < UltTargets.Num())
	{
		const AActor* Target = UltTargets[UltIndex];

		UE_LOG(LogTemp, Warning, TEXT("Target %d: %s"), UltIndex, *Target->GetName());

		if (Blader->GetMotionWarpingComponent() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("UltNextAttack AnimNotify Null MotionWarpingComponent"));
			return;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Target Location: %s"), *Target->GetActorLocation().ToString());


		Blader->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(
		FName("Ult"), Target->GetActorLocation(), Target->GetActorRotation()
		);

		Blader->PlayUltAttackMontage();
		
	}
	else
	{
		Blader->UltEnd();
	}
}
