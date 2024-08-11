// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ASRCharacter.h"
#include "Enemy/BaseEnemy.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsTargeting = false;
	CameraOffset = FVector(0.f, 0.f, 35.f);
}


void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Owner == nullptr)
	{
		Owner = Cast<AASRCharacter>(GetOwner());
	}
}

void UTargetingComponent::FindTarget()
{
	//UKismetSystemLibrary::SphereTraceMultiForObjects(
	if (Owner == nullptr)
	{
		Owner = Cast<AASRCharacter>(GetOwner());
	}

	if (Owner != nullptr && Owner->GetFollowCamera() != nullptr)
	{
		UCameraComponent* FollowCam = Owner->GetFollowCamera();
		FVector TraceEnd;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TraceEnd = FollowCam->GetComponentLocation() + FollowCam->GetForwardVector() * TargetingDistance;
		FHitResult HitResult;

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(Cast<AActor>(Owner));

		
		bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(),
			FollowCam->GetComponentLocation(),
			TraceEnd,
			100.f,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			HitResult,
			true
		);

		if (bHit)
		{
			if (HitResult.GetActor() != nullptr)
			{
				ABaseEnemy* Enemy = Cast<ABaseEnemy>(HitResult.GetActor());
				if (Enemy != nullptr && Enemy->GetCharacterState() != EASRCharacterState::ECS_Death)
				{
					TargetActor = Enemy;
					bIsTargeting = true;
					return;
				}
				else
				{
					ClearTarget();
				}
			}
			
		}
		else
		{
			// Handle No Hit
		}

		// No Hit or Hit actor is not enemy
		ClearTarget();
	}

}

void UTargetingComponent::ClearTarget()
{
	bIsTargeting = false;
	TargetActor = nullptr;
}

void UTargetingComponent::PlaceDecalActor()
{
	if (TargetActor != nullptr && GetWorld())
	{
		FVector TargetLocation = TargetActor->GetActorLocation();
		FRotator TargetRotation = FRotator::ZeroRotator; // Decal의 회전 값
		float DecalSize = 10.f; // Decal의 크기 설정

		ADecalActor* Decal = GetWorld()->SpawnActor<ADecalActor>(TargetLocation, TargetRotation);
		if (Decal != nullptr)
		{
			UDecalComponent* DecalComponent = Decal->FindComponentByClass<UDecalComponent>();
			if (DecalComponent != nullptr)
			{
				 //Decal의 크기 및 재질 설정
				DecalComponent->SetDecalMaterial(DecalMaterial); // 빨간색 재질을 설정
				DecalComponent->DecalSize = FVector(DecalSize); // 크기 설정
			}
		}
	}
}

FTransform UTargetingComponent::GetTargetTransform()
{
	if (TargetActor != nullptr)
	{

		FVector OwnerLocation = Owner->GetActorLocation();
		FVector TargetLocation = TargetActor->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocation);

		FRotator OwnerRotation = Owner->GetActorRotation();
		OwnerRotation.Yaw = LookAtRotation.Yaw;

		FTransform TargetTransform;

		TargetTransform.SetLocation(TargetLocation - OwnerLocation);
		TargetTransform.SetRotation(OwnerRotation.Quaternion()); // 회전을 Quat으로 설정

		return TargetTransform;
	}
	return FTransform::Identity;
}


void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsTargeting && TargetActor != nullptr && Owner != nullptr)
	{
		if (TargetActor->GetDistanceTo(GetOwner()) < ClearTargetDistance)
		{
			ABaseEnemy* BaseEnemy = Cast<ABaseEnemy>(TargetActor);
			if (BaseEnemy != nullptr && BaseEnemy->GetCharacterState() != EASRCharacterState::ECS_Death)
			{
				// LockOn Target
				FRotator LookRotator;
				FRotator NewControlRotator;

				LookRotator = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), TargetActor->GetActorLocation() - CameraOffset);
				NewControlRotator = FMath::RInterpTo(Owner->GetController()->GetControlRotation(), LookRotator, DeltaTime, CameraRotationSpeed);
				Owner->GetController()->SetControlRotation(NewControlRotator);
				
				// TODO: Targeting Point UI
				//PlaceDecalActor();

			}
			else
			{
				ClearTarget();
			}

		}
		else
		{
			ClearTarget();
		}
	}
}

