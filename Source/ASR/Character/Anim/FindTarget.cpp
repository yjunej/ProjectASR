// Fill out your copyright notice in the Description page of Project Settings.


#include "FindTarget.h"
#include "ASR/Character/Blader.h"
#include "ASR/Character/TargetingComponent.h"
#include "MotionWarpingComponent.h"

void UFindTarget::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null Owner"));
		return;
	}
	ABlader* Blader = Cast<ABlader>(Owner);
	if (Blader == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null Blader"));
		return;
	}

	UTargetingComponent* TargetingComponent = Blader->GetTargetingComponent();

	if (TargetingComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null TargetingComponent"));
		return;
	}

	FTransform WarpTransform;

	if (TargetingComponent->IsTargeting())
	{
		UE_LOG(LogTemp, Warning, TEXT("LockOn Mode"));

		// Use LockOn Target Transform
		FTransform TargetTransform = TargetingComponent->GetTargetTransform();
		float WarpDistance = Blader->LightAttackWarpDistance > TargetTransform.GetLocation().Length() ? TargetTransform.GetLocation().Length() : Blader->LightAttackWarpDistance;
		
		WarpTransform.SetLocation(Blader->GetActorLocation() + TargetTransform.GetLocation().GetSafeNormal() * WarpDistance);
		WarpTransform.SetRotation(TargetTransform.GetRotation());
	}
	else
	{
		// Find SubTarget
		bool bFind = TargetingComponent->FindSubTarget();
		if (bFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("SubTarget Mode"));


			FTransform TargetTransform = TargetingComponent->GetTargetTransform();
			float WarpDistance = Blader->LightAttackWarpDistance > TargetTransform.GetLocation().Length() ? TargetTransform.GetLocation().Length() : Blader->LightAttackWarpDistance;

			WarpTransform.SetLocation(Blader->GetActorLocation() + TargetTransform.GetLocation().GetSafeNormal() * WarpDistance);
			WarpTransform.SetRotation(TargetTransform.GetRotation());
		}
		else
		{
			// Use Forward Vector
			UE_LOG(LogTemp, Warning, TEXT("Forward Mode"));

			//WarpTransform.SetLocation(Blader->GetActorLocation() + Blader->GetActorForwardVector() * Blader->LightAttackWarpDistance);
			WarpTransform.SetLocation(Blader->GetActorLocation() + Blader->GetPendingMovementInputVector() * Blader->LightAttackWarpDistance);
			WarpTransform.SetRotation(FQuat(Blader->GetActorRotation()));
		}


	}	
	WarpTransform.SetScale3D(FVector(1.f, 1.f, 1.f));

	if (Blader->GetMotionWarpingComponent() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindTarget AnimNotify Null MotionWarpingComponent"));
		return;
	}
	

	Blader->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromTransform(FName("Forward"), WarpTransform);
}