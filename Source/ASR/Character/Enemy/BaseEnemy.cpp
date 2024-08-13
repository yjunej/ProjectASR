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

void ABaseEnemy::Landed(const FHitResult& HitResult)
{
	// Falling -> Hit Ground
	Super::Landed(HitResult);
	ResetState();
	UE_LOG(LogTemp, Warning, TEXT("LANDED!"));

	if (Health <= 0.f)
	{
		HandleDeath();
	}
	else
	{
		PlayAnimMontage(StandUpMontage);

	}


}

void ABaseEnemy::ResetState()
{
	if (CharacterState == EASRCharacterState::ECS_Death)
	{
		return;
	}
	CharacterState = EASRCharacterState::ECS_None;
	
	EMovementMode MovementMode = GetCharacterMovement()->MovementMode;
	if (MovementMode == EMovementMode::MOVE_Flying)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	 
		return;
	}
	bIsAirSmash = false;
	bIsLevitating = false;

}

void ABaseEnemy::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("HandleDeath!"));

	//if (!GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
	//{
	CharacterState = EASRCharacterState::ECS_Death;

	
	if (GetCharacterMovement()->IsFalling())
	{
		PlayAnimMontage(FallingDeathMontage);
	}
	else
	{
		PlayAnimMontage(StandingDeathMontage);
	}


	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	//}


}

void ABaseEnemy::HandleTimelineUpdate(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("TIMELINE AIRSmash: %f"), AirSmashCurve->GetFloatValue(TimelineComponent->GetPlaybackPosition()));
	//UE_LOG(LogTemp, Warning, TEXT("TIMELINE Levitate: %f"), LevitateCurve->GetFloatValue(TimelineComponent->GetPlaybackPosition()));
	//UE_LOG(LogTemp, Warning, TEXT("TIMELINE Slightly Knockback: %f"), KnockbackCurve->GetFloatValue(TimelineComponent->GetPlaybackPosition()));

	

	if (GetWorld() != nullptr)
	{
		if (bIsLevitating)
		{
			FVector NewLocation = UKismetMathLibrary::VLerp(StartLocation, StartLocation + FVector(0.f, 0.f, LevitateHeight), Value);
			SetActorLocation(NewLocation, true);
		}
		else if (bIsAirSmash)
		{
			FVector DirectionVector = GetActorForwardVector() * -1;
			FVector NewLocation = UKismetMathLibrary::VLerp(StartLocation, StartLocation + DirectionVector * AirSmashDistance - FVector(0.f, 0.f, StartLocation.Z), Value);
			SetActorLocation(NewLocation, true);
		}

		else // Slightly Knockback
		{
			FVector DirectionVector = GetActorForwardVector() * -1;
			FVector NewLocation = StartLocation + DirectionVector * KnockbackDistance * Value;
			SetActorLocation(NewLocation, true);
		}

	}
}

void ABaseEnemy::HandleTimelineFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("TimelineFIN!"));
	if (bIsLevitating)
	{
		// Ensure to reset the state after levitation
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		ResetState();
		bIsLevitating = false;

	}
	if (bIsAirSmash)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		ResetState();
		bIsAirSmash = false;
	}
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

void ABaseEnemy::AerialKnockdown()
{
	bIsAirSmash = true;
	StopTimeline();
	StartLocation = GetActorLocation();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	TimelineComponent->PlayFromStart();
}

void ABaseEnemy::InitializeTimeline()
{
	FOnTimelineFloat TimelineCallback;
	TimelineCallback.BindUFunction(this, FName("HandleTimelineUpdate"));
	TimelineComponent->AddInterpFloat(KnockbackCurve, TimelineCallback);
	TimelineComponent->AddInterpFloat(LevitateCurve, TimelineCallback);
	TimelineComponent->AddInterpFloat(AirSmashCurve, TimelineCallback);


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
	

	if (CharacterState == EASRCharacterState::ECS_Death)
	{ 
		return;
	}
	//
	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("HEALTH: %f"), Health);


	if (Health <= 0 && !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
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

			
			// Manage State

			if (TimelineComponent != nullptr)
			{
				if (DamageType == EASRDamageType::EDT_AerialStart)
				{
					UE_LOG(LogTemp, Warning, TEXT("Levitate!!"));

					Levitate();
				}
				else if (DamageType == EASRDamageType::EDT_AerialKnockDown)
				{
					UE_LOG(LogTemp, Warning, TEXT("AerialKnockDown!!"));
					AerialKnockdown();
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

				// Grab Enemy in Air
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				//

				switch (DamageType)
				{
				case EASRDamageType::EDT_Default:
				case EASRDamageType::EDT_Die:
				case EASRDamageType::EDT_FrontSmall:
				case EASRDamageType::EDT_BackSmall:
				case EASRDamageType::EDT_LeftSmall:
				case EASRDamageType::EDT_RightSmall:
				case EASRDamageType::EDT_FrontDown:
				case EASRDamageType::EDT_BackDown:
				case EASRDamageType::EDT_FrontBig:
				case EASRDamageType::EDT_KnockDownBack:
				case EASRDamageType::EDT_KnockDownBackBig:
				case EASRDamageType::EDT_KnockDownBackSmash:
				case EASRDamageType::EDT_KnockDownFront:
				case EASRDamageType::EDT_KnockDownFrontSpear:
				case EASRDamageType::EDT_KnockDownFrontSmashDouble:
				case EASRDamageType::EDT_KnockDownFrontBig:
				case EASRDamageType::EDT_AerialHit:
					Mapping = DamageTypeMappings.Find(EASRDamageType::EDT_AerialHit);
					break;
				case EASRDamageType::EDT_AerialStart:
					Mapping = DamageTypeMappings.Find(EASRDamageType::EDT_AerialStart);
					break;


				case EASRDamageType::EDT_AerialKnockDown:
					Mapping = DamageTypeMappings.Find(EASRDamageType::EDT_AerialKnockDown);
					UE_LOG(LogTemp, Warning, TEXT("AerialKnockDownAnim!!"));

					break;
				case EASRDamageType::EDT_MAX:
					UE_LOG(LogTemp, Warning, TEXT("EDT_MAX No Mapping!"));
					Mapping = DamageTypeMappings.Find(EASRDamageType::EDT_Default);
					break;
				default:
					Mapping = DamageTypeMappings.Find(EASRDamageType::EDT_Default);
					UE_LOG(LogTemp, Warning, TEXT("EDT_MAX No Mapping!"));
					break;
				}

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

