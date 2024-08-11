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
#include "GameFramework/CharacterMovementComponent.h"

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
			TargetRadius,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			HitResult,
			true
		);

		if (bHit)
		{
			LockOnTarget(HitResult);
		}
		else
		{
			// Broad Sphere Trace Again
			FindNearestTarget();
		}

		// No Hit or Hit actor is not enemy
	}

}

void UTargetingComponent::FindNearestTarget()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Cast<AActor>(Owner));
	FHitResult HitResult;

	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		Owner->GetActorLocation(),
		Owner->GetActorLocation(),
		500.f,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true
	);
	if (bHit)
	{
		LockOnTarget(HitResult);
	}
	else
	{
		ClearTarget();
	}
}

bool UTargetingComponent::FindSubTarget()
{
	if (bIsTargeting)
	{
		return false;
	}
	if (Owner == nullptr)
	{
		Owner = Cast<AASRCharacter>(GetOwner());
	}

	if (Owner != nullptr)
	{
		FVector LastInputVector = Owner->GetCharacterMovement()->GetLastInputVector();

		// If Player not intend to Attack Something, use default motion warping values
		if (LastInputVector.IsNearlyZero(0.05))
		{
			UE_LOG(LogTemp, Warning, TEXT("Not Enough Input: %s, Skip Trace"), *LastInputVector.ToString());
			return false;
		}
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
		FHitResult HitResult;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(Cast<AActor>(Owner));

		bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(),
			Owner->GetActorLocation(),
			Owner->GetActorLocation() + Owner->GetCharacterMovement()->GetLastInputVector() * SubTargetingDistance,
			SubTargetingRadius,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			HitResult,
			true,
			FLinearColor::Blue
		);
		if (bHit)
		{
			if (HitResult.GetActor() != nullptr)
			{
				ABaseEnemy* Enemy = Cast<ABaseEnemy>(HitResult.GetActor());
				if (Enemy != nullptr && Enemy->GetCharacterState() != EASRCharacterState::ECS_Death)
				{
					if (SubTargetActor != HitResult.GetActor())
					{
						SubTargetActor = HitResult.GetActor();
						return true;	
					}
				}
				else
				{
					// Not Enemy or Dead Emeny
					SubTargetActor = nullptr;
					return false;
				}
			}
			else
			{
				// Collider with no Owner
				SubTargetActor = nullptr;
				return false;
			}
		}
		else
		{
			// Not Hit
			SubTargetActor = nullptr;
			return false;

		}
	}
	return false;
}

void UTargetingComponent::LockOnTarget(const FHitResult& HitResult)
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
	else
	{
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
	AActor* Target;
	if (TargetActor != nullptr)
	{
		Target = TargetActor;
	}
	else if (SubTargetActor != nullptr)
	{
		Target = SubTargetActor;
	}
	else
	{
		return FTransform::Identity;
	}

	FVector OwnerLocation = Owner->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocation);

	FRotator OwnerRotation = Owner->GetActorRotation();
	OwnerRotation.Yaw = LookAtRotation.Yaw;

	FTransform TargetTransform;

	TargetTransform.SetLocation(TargetLocation - OwnerLocation);
	TargetTransform.SetRotation(OwnerRotation.Quaternion()); // 회전을 Quat으로 설정

	return TargetTransform;
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

