// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (KnockbackCurve != nullptr && LevitateCurve != nullptr)
	{
		InitializeTimeline();
	}
}

void ABaseEnemy::ResetState()
{
	CharacterState = EASRCharacterState::ECS_None;
	
	EMovementMode MovementMode = GetCharacterMovement()->MovementMode;
	if (MovementMode == EMovementMode::MOVE_Flying || MovementMode == EMovementMode::MOVE_Falling)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	
	bIsLevitating = false;

}

void ABaseEnemy::HandleDeath()
{
	CharacterState = EASRCharacterState::ECS_Death;
	PlayAnimMontage(DeathMontage); 
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

}

UAnimMontage* ABaseEnemy::GetHitReactionMontage(EASRDamageType DamageType)
{
	return nullptr;
}


void ABaseEnemy::HandleTimelineUpdate(float Value)
{
	if (GetWorld() != nullptr)
	{
		if (bIsLevitating)
		{
			FVector NewLocation = UKismetMathLibrary::VLerp(StartLocation, StartLocation + FVector(0.f, 0.f, LevitateHeight), Value);
			SetActorLocation(NewLocation, true);
		}
		else if (bIsSmashing)
		{
			// TODO
		}

		else // Slightly Knockback
		{
			FVector DirectionVector = GetActorForwardVector() * -1;
			FVector NewLocation = StartLocation + DirectionVector * KnockbackDistance * Value;
			SetActorLocation(NewLocation);
		}

	}
}

void ABaseEnemy::HandleTimelineFinished()
{
	if (bIsLevitating)
	{
		// Ensure to reset the state after levitation
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	bIsLevitating = false;
}

void ABaseEnemy::StopTimeline()
{
	if (TimelineComponent != nullptr)
	{
		TimelineComponent->Stop();
	}
}

void ABaseEnemy::ApplyKnockback()
{
	StopTimeline();
	StartLocation = GetActorLocation();
	TimelineComponent->PlayFromStart();
}

void ABaseEnemy::Levitate()
{
	bIsLevitating = true;
	StopTimeline();
	StartLocation = GetActorLocation();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	TimelineComponent->PlayFromStart();
}

void ABaseEnemy::InitializeTimeline()
{
	FOnTimelineFloat TimelineCallback;
	TimelineCallback.BindUFunction(this, FName("HandleTimelineUpdate"));
	TimelineComponent->AddInterpFloat(KnockbackCurve, TimelineCallback);
	TimelineComponent->AddInterpFloat(LevitateCurve, TimelineCallback);

	FOnTimelineEvent TimelineFinishedCallback;
	TimelineFinishedCallback.BindUFunction(this, FName("HandleTimelineFinished"));
	TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
}

void ABaseEnemy::SetCharacterState(EASRCharacterState InCharacterState)
{
	{
		if (InCharacterState != CharacterState && CharacterState != EASRCharacterState::ECS_Death)
		{
			CharacterState = InCharacterState;
		}
	}
}

void ABaseEnemy::GetHit(const FHitResult& HitResult, AActor* Attacker, float Damage, EASRDamageType DamageType)
{
	// [TODO] Block Handling Before Take Damage
	
	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("HEALTH: %f"), Health);

	if (CharacterState == EASRCharacterState::ECS_Death)
	{ 
		return;
	}

	if (Health <= 0)
	{
		HandleDeath();
		return;
	}
	else
	{
		FDamageTypeMapping* Mapping;
		Mapping = DamageTypeMappings.Find(DamageType);
		if (Mapping != nullptr)
		{
			CharacterState = Mapping->CharacterState;


			FRotator LookAtAttackerRotator = GetActorRotation();
			LookAtAttackerRotator.Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Attacker->GetActorLocation()).Yaw;
			SetActorRotation(LookAtAttackerRotator);



			if (TimelineComponent != nullptr)
			{
				if (DamageType == EASRDamageType::EDT_FrontHighKnockDown)
				{
					UE_LOG(LogTemp, Warning, TEXT("Levitate!!"));

					Levitate();
				}
				else
				{
					// Disable Knockback In Air
					if (!GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
					{
						UE_LOG(LogTemp, Warning, TEXT("KnockBack!!"));

						ApplyKnockback();
					}
					else // Matching Z Location
					{
						FVector ZMatching = GetActorLocation();
						ZMatching.Z = Attacker->GetActorLocation().Z - 25.f >= 0.f ? Attacker->GetActorLocation().Z - 25.f : 0.f;
						SetActorLocation(ZMatching);
					}
				}
			}

			// Aerial Hit!
			if (GetCharacterMovement()->IsFalling() || GetCharacterMovement()->IsFlying())
			{
				UE_LOG(LogTemp, Warning, TEXT("AerialAttack!!"));

				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				Mapping = DamageTypeMappings.Find(EASRDamageType::EDT_FrontHighKnockDownHit);
			}

			if (Mapping->HitReactionMontage != nullptr)
			{
				PlayAnimMontage(Mapping->HitReactionMontage);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("NULL HitReact Montage!"));

			}


		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NULL DamageType Mapping!"));
		}
	}
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

