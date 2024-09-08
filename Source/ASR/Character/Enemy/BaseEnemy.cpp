// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "ASR/HUD/EnemyInfoWidget.h"
#include "ASR/HUD/EnemyLockOnWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "ASR/Character/Enemy/BaseAIController.h"
#include "Sound/SoundCue.h"
#include "ASR/Character/ASRCharacter.h"
#include "ASR/Character/Slayer.h"
#include "ASR/Character/Enemy/AI/PatrolRoute.h"



ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	InfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidgetComponent"));
	InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InfoWidgetComponent->SetDrawSize(FVector2D(250.f, 250.f));
	InfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 190.f));
	InfoWidgetComponent->SetupAttachment(GetMesh());

	LockOnWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidgetComponent"));
	LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComponent->SetDrawSize(FVector2D(10.f, 10.f));
	LockOnWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 130.f));
	LockOnWidgetComponent->SetupAttachment(GetMesh());

	// Set this for Enemy that spanwned by spanwer
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate.Yaw = 250.f;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComponent->SetupAttachment(GetMesh(), FName("RightHandKatanaSocket")); 

	// [Blueprint]
	// - Enable bUseAccelerationForPaths
	// - Setting BehaviorTree

	// AI
}


void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (InfoWidgetClass != nullptr)
	{
		InfoWidgetComponent->SetWidgetClass(InfoWidgetClass);
		InfoWidget = Cast<UEnemyInfoWidget>(InfoWidgetComponent->GetUserWidgetObject());
		if (InfoWidget != nullptr)
		{
			InfoWidget->Owner = this;
			InfoWidget->UpdateHealthBar();
		}
	}

	if (LockOnWidgetClass != nullptr)
	{
		LockOnWidgetComponent->SetWidgetClass(LockOnWidgetClass);
		LockOnWidget = Cast<UEnemyLockOnWidget>(LockOnWidgetComponent->GetUserWidgetObject());
		LockOnWidgetComponent->SetVisibility(true);
		if (LockOnWidget != nullptr)
		{
			LockOnWidget->Owner = this;
			LockOnWidget->SelectMarker();
			LockOnWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	SetHealth(MaxHealth);

	if (KnockbackCurve != nullptr && LevitateCurve != nullptr)
	{
		InitializeTimeline();
	}
}


void ABaseEnemy::NotifyAttackEnd()
{
	OnAttackEnd.Broadcast();
}

void ABaseEnemy::SetHealth(float NewHealth)
{

	NewHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);
	if (Health != NewHealth)
	{
		Health = NewHealth;
		OnHealthChanged.Broadcast();
	}

}

void ABaseEnemy::OnTargeting()
{
	if (LockOnWidget != nullptr)
	{
		LockOnWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABaseEnemy::OnUnTargeting()
{
	LockOnWidget->SetVisibility(ESlateVisibility::Hidden);
}

bool ABaseEnemy::CanBeExecuted() const
{
	return Health <= ExecutionThresholdHealth;
}

void ABaseEnemy::Executed()
{
	if (ExecutionMontage != nullptr)
	{
		// TODO Get Attacker
		APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
		if (Pawn != nullptr)
		{
			// TODO - Intergrate on GetHit
			RotateToAttacker(Pawn, false);
			
			HandleHitTransform(Pawn, EASRDamageType::EDT_Default, 1000.f);
			SetCharacterState(EASRCharacterState::ECS_Death);
			SetHealth(0.f);
			if (Cast<ASlayer>(Pawn) != nullptr)
			{
				PlayAnimMontage(SmashExecutionMontage);
			}
			else
			{
				PlayAnimMontage(ExecutionMontage);
			}
		}
	} 
}

bool ABaseEnemy::NormalAttack()
{
	float AnimDuration = 0.f;
	if (CanAttack())
	{
		return ExecuteNormalAttack();
	}
	return false;
}

bool ABaseEnemy::ExecuteNormalAttack()
{
	if (NormalAttackMontages.Num() > 0)
	{
		CharacterState = EASRCharacterState::ECS_Attack;
		if (NormalAttackMontages.Num() <= NormalAttackIndex)
		{
			NormalAttackIndex = 0;
		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(
				NormalAttackMontages[NormalAttackIndex], 1.0f, EMontagePlayReturnType::Duration,
				0.0f, true
			);
			FOnMontageEnded LMontageEnded;
			LMontageEnded.BindUObject(this, &ABaseEnemy::OnMontageEnded);
			AnimInstance->Montage_SetEndDelegate(LMontageEnded, NormalAttackMontages[NormalAttackIndex]);

			NormalAttackIndex++;
			return true;
		}


	}
	return false;
}

bool ABaseEnemy::CanAttack()
{
	if (CharacterState != EASRCharacterState::ECS_Attack && CharacterState != EASRCharacterState::ECS_Dodge
		&& CharacterState != EASRCharacterState::ECS_Death && CharacterState != EASRCharacterState::ECS_Flinching &&
		CharacterState != EASRCharacterState::ECS_KnockDown && !GetCharacterMovement()->IsFalling() && !bIsLevitating && !GetCharacterMovement()->IsFlying())
	{
		return true;
	}
	return false;
}

void ABaseEnemy::Landed(const FHitResult& HitResult)
{
	// Falling -> Hit Ground
	Super::Landed(HitResult);
	ResetState();

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
	
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		return;
	}
	bIsAirSmash = false;
	bIsLevitating = false;
}

void ABaseEnemy::HandleDeath()
{
	// Not Playing Death Montage for already Died enemy
	if (CharacterState != EASRCharacterState::ECS_Death)
	{
		CharacterState = EASRCharacterState::ECS_Death;
		if (GetCharacterMovement()->IsFalling())
		{
			PlayAnimMontage(FallingDeathMontage);
		}
		else
		{
			PlayAnimMontage(StandingDeathMontage);
		}
		CharacterState = EASRCharacterState::ECS_Death;

	}
	DisableCollision();
}


void ABaseEnemy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (NormalAttackMontages.Contains(Montage))
	{
		NotifyAttackEnd();
	}
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
	if (bIsLevitating)
	{
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
	KnockbackDistance = BaseKnockbackDistance;
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

void ABaseEnemy::RotateToAttacker(AActor* Attacker, bool bIsRunFromAttacker)
{
	FRotator LookAtAttackerRotator = GetActorRotation();
	LookAtAttackerRotator.Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Attacker->GetActorLocation()).Yaw;
	if (bIsRunFromAttacker)
	{
		LookAtAttackerRotator.Yaw += 180.0f;
	}
	SetActorRotation(LookAtAttackerRotator);
}

void ABaseEnemy::StepBackFromAttacker(AActor* Attacker, float Distance)
{
	FVector BackwardVector = -GetActorForwardVector() * Distance; 
	AddActorWorldOffset(BackwardVector, true);
}


void ABaseEnemy::HandleHitTransform(AActor* Attacker, EASRDamageType DamageType, float Damage)
{

	if (TimelineComponent != nullptr)
	{
		// Select Timeline Curve
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
		else if (DamageType == EASRDamageType::EDT_AerialHit)
		{
			FVector ZMatching = GetActorLocation();
			ZMatching.Z = Attacker->GetActorLocation().Z - 25.f >= 0.f ? Attacker->GetActorLocation().Z - 25.f : 0.f;
			SetActorLocation(ZMatching);
		}
		else if (DamageType == EASRDamageType::EDT_KnockDownFrontBig)
		{
			KnockbackDistance = Damage;
			ApplyKnockback();
		}
		else // Knockback
		{
			UE_LOG(LogTemp, Warning, TEXT("Normal KnockBack"));
			ApplyKnockback();
		}
	}
}

void ABaseEnemy::AerialHitAnimMapping(AActor* Attacker, FDamageTypeMapping* Mapping, EASRDamageType DamageType)
{
	UE_LOG(LogTemp, Warning, TEXT("AerialAttack!!"));

	// Grab Enemy in Air
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

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


void ABaseEnemy::DisableCollision()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
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

void ABaseEnemy::GetHit(const FHitResult& HitResult, AActor* Attacker, const FHitData& HitData)
{
	// [TODO] Block Handling Before Take Damage
	
	if (CharacterState == EASRCharacterState::ECS_Death)
	{ 
		return;
	}
	
	if (Cast<AASRCharacter>(Attacker) != nullptr)
	{
		ApplyHitStop(HitStopDuration, HitStopTimeDilation);
		AASRCharacter* ASRCharacter = Cast<AASRCharacter>(Attacker);
		ASRCharacter->ApplyHitStop(HitStopDuration, HitStopTimeDilation);
	}


	SetHealth(Health - HitData.Damage);

	UE_LOG(LogTemp, Warning, TEXT("HEALTH: %f"), Health);

	// Effects
	if (HitData.HitSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitData.HitSound, GetActorLocation());
	}
	if (HitData.HitEffect != nullptr)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HitData.HitEffect,
			HitResult.ImpactPoint,
			GetActorRotation(),
			FVector(1.f)
		);
	}

	// Death
	if (Health <= 0 && !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
	{
		HandleDeath();
		return;
	}

	// Animation
	FDamageTypeMapping* Mapping;
	Mapping = DamageTypeMappings.Find(HitData.DamageType);
	if (Mapping != nullptr)
	{

		CharacterState = Mapping->CharacterState;			
		RotateToAttacker(Attacker, false);
		HandleHitTransform(Attacker, HitData.DamageType, HitData.Damage);


		if (GetCharacterMovement()->IsFalling() || GetCharacterMovement()->IsFlying())
		{
			AerialHitAnimMapping(Attacker, Mapping, HitData.DamageType);
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

APatrolRoute* ABaseEnemy::GetPatrolRoute_Implementation() const
{
	return PatrolRoute;
}

float ABaseEnemy::SetMovementSpeed_Implementation(EEnemyMovementSpeed EnemyMovementSpeed)
{
	float NewSpeed;

	switch (EnemyMovementSpeed)
	{
	case EEnemyMovementSpeed::EMS_Idle:
		NewSpeed = 0.f;
		break;
	case EEnemyMovementSpeed::EMS_Walk:
		NewSpeed = 100.f;
		break;
	case EEnemyMovementSpeed::EMS_Jog:
		NewSpeed = 300.f;
		break;
	case EEnemyMovementSpeed::EMS_Run:
		NewSpeed = 500.f;
		break;
	case EEnemyMovementSpeed::EMS_Sprint:
		NewSpeed = 700.f;
		break;
	case EEnemyMovementSpeed::EMS_MAX:
	default:
		NewSpeed = 0.f;
		break;
	}
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	return NewSpeed;
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void ABaseEnemy::SphereTrace(float TraceDistance, float TraceRadius, const FHitData& HitData, ECollisionChannel CollisionChannel, bool bDrawDebugTrace)
{
	HitActors.Empty();
	TArray<FHitResult> HitResults;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * TraceDistance;


	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(CollisionChannel));


	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this, GetActorLocation(), TraceEnd, TraceRadius, ObjectTypes, false, TArray<AActor*>(),
		bDrawDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			// Check Duplicated Hit
			if (HitActor != nullptr && !HitActors.Contains(HitActor))
			{
				IHitInterface* HitInterface = Cast<IHitInterface>(HitActor);
				if (HitInterface != nullptr)
				{
					//UGameplayStatics::PlaySoundAtLocation(this, HitSoundCue, HitActor->GetActorLocation());
					HitInterface->GetHit(HitResult, this, HitData);
					HitActors.AddUnique(HitActor);
				}
			}
		}
	}
}

void ABaseEnemy::ApplyHitStop(float Duration, float TimeDilation)
{
	CustomTimeDilation = 0.f;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseEnemy::ResetTimeDilation, Duration, false);
}

void ABaseEnemy::ResetTimeDilation()
{
	CustomTimeDilation = 1.0f;
}
