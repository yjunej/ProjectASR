// Fill out your copyright notice in the Description page of Project Settings.


#include "FindTarget.h"
#include "ASR/Character/TargetingComponent.h"
#include "ASR/Character/ASRCharacter.h"
#include "MotionWarpingComponent.h"

void UFindTarget::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null Owner"));
		return;
	}
	AASRCharacter* ASRCharacter = Cast<AASRCharacter>(Owner);
	if (ASRCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null ASRCharacter"));
		return;
	}

	UTargetingComponent* TargetingComp = ASRCharacter->GetTargetingComponent();

	if (TargetingComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null TargetingComponent"));
		return;
	}

	FTransform WarpTransform;

	// Limit Warp Transform
	UAnimInstance* AnimInstance;
	FName SectionName("Default");
	FName TargetName("Forward");


	AnimInstance =  MeshComp->GetAnimInstance();
	if (AnimInstance != nullptr)
	{
		UAnimMontage* AnimMontage = AnimInstance->GetCurrentActiveMontage();
		if (AnimMontage != nullptr)
		{
			SectionName = AnimInstance->Montage_GetCurrentSection(AnimMontage);
		}
	}

	float MaxWarpDistance;
	if (SectionName == "DashAttack")
	{
		MaxWarpDistance = ASRCharacter->GetDashAttackWarpDistance();
		TargetName = FName("DashAttack");
	}
	else if (SectionName == "Execution")
	{
		MaxWarpDistance = ASRCharacter->GetExecutionDistance();
		TargetName = FName("Execution");
	}
	else if (SectionName == "FirstSkill")
	{
		MaxWarpDistance = ASRCharacter->GetFirstSkillWarpDistance();
		TargetName = FName("FirstSkill");
	}
	else
	{
		MaxWarpDistance = ASRCharacter->GetNormalAttackWarpDistance();
	}

	


	if (TargetingComp->IsTargeting())
	{
		UE_LOG(LogTemp, Warning, TEXT("LockOn Mode"));

		// Use LockOn Target Transform
		FTransform TargetTransform = TargetingComp->GetTargetTransform();
		float WarpDistance = MaxWarpDistance > TargetTransform.GetLocation().Length() ? TargetTransform.GetLocation().Length() : MaxWarpDistance;
		

		WarpTransform.SetLocation(ASRCharacter->GetActorLocation() + TargetTransform.GetLocation().GetSafeNormal() * WarpDistance);
		WarpTransform.SetRotation(TargetTransform.GetRotation());
	}
	else
	{
		// Find SubTarget
		bool bFind = TargetingComp->FindSubTarget();
		if (bFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("SubTarget Mode"));

			FTransform TargetTransform = TargetingComp->GetTargetTransform();
			float WarpDistance = MaxWarpDistance > TargetTransform.GetLocation().Length() ? TargetTransform.GetLocation().Length() : MaxWarpDistance;

			WarpTransform.SetLocation(ASRCharacter->GetActorLocation() + TargetTransform.GetLocation().GetSafeNormal() * WarpDistance);
			WarpTransform.SetRotation(TargetTransform.GetRotation());
		}
		else
		{
			if (TargetingComp->GetLastSubTargetActor() != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("LastSubTarget Mode"));
				FTransform TargetTransform = TargetingComp->GetLastSubTargetTransform();
				float WarpDistance = MaxWarpDistance > TargetTransform.GetLocation().Length() ? TargetTransform.GetLocation().Length() : MaxWarpDistance;
				WarpTransform.SetLocation(ASRCharacter->GetActorLocation() + TargetTransform.GetLocation().GetSafeNormal() * WarpDistance);
				WarpTransform.SetRotation(TargetTransform.GetRotation());

			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Forward Mode"));
				//WarpTransform.SetLocation(Blader->GetActorLocation() + Blader->GetActorForwardVector() * WarpDistance);
				WarpTransform.SetLocation(ASRCharacter->GetActorLocation() + ASRCharacter->GetActorForwardVector() * MaxWarpDistance);
				WarpTransform.SetRotation(FQuat(ASRCharacter->GetActorRotation()));
			}
			// Use Forward Vector
				
		}


	}	
	WarpTransform.SetScale3D(FVector(1.f, 1.f, 1.f));

	if (ASRCharacter->GetMotionWarpingComponent() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null MotionWarpingComponent"));
		return;
	}
	

	ASRCharacter->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromTransform(TargetName, WarpTransform);
}