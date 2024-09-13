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
#include "Perception/AISense_Damage.h"
#include "ASR/Weapons/MeleeWeapon.h"
#include "BehaviorTree/BehaviorTree.h"



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

	if (MeleeWeaponClass != nullptr)
	{
		MeleeWeapon = GetWorld()->SpawnActor<AMeleeWeapon>(MeleeWeaponClass);

		if (MeleeWeapon != nullptr)
		{
			FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
			MeleeWeapon->AttachToComponent(GetMesh(), AttachRules, WeaponSocketName);
			MeleeWeapon->WeaponOwner = this;
			MeleeWeapon->GetWeaponMesh()->SetVisibility(!bIsWeaponHidden);
		}
	}

}


void ABaseEnemy::NotifyAttackEnd(AActor* AttackTarget)
{
	OnAttackEnd.Broadcast(AttackTarget);
}

void ABaseEnemy::NotifyGuardEnd()
{
	OnGuardEnd.Broadcast();
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
			SetCombatState(ECombatState::ECS_Death);
			SetHealth(0.f);
			HandleDeath();
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

bool ABaseEnemy::NormalAttack(AActor* AttackTarget)
{
	if (CanAttack())
	{
		CachedAttackTarget = AttackTarget;
		return ExecuteNormalAttack(AttackTarget);
	}
	return false;
}

bool ABaseEnemy::ExecuteNormalAttack(AActor* AttackTarget)
{
	if (NormalAttackMontages.Num() > 0)
	{
		SetCombatState(ECombatState::ECS_Attack);
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

bool ABaseEnemy::Guard()
{
	if (CanGuard() && FMath::RandRange(0.f, 1.f) <= ReactStateGuardRate)
	{
		SetCombatState(ECombatState::ECS_Guard);
		PlayAnimMontage(GuardMontage);
		return true;
	}
	return false;
}

bool ABaseEnemy::CanGuard() const
{
	if (GetCombatState() != ECombatState::ECS_Attack && GetCombatState() != ECombatState::ECS_Guard
		&& GetCombatState() != ECombatState::ECS_Death && !GetCharacterMovement()->IsFalling())
	{
		return true;
	}
	return false;
}

bool ABaseEnemy::CanAttack() const
{

	if (GetCombatState() != ECombatState::ECS_Attack && GetCombatState() != ECombatState::ECS_Dodge
		&& GetCombatState() != ECombatState::ECS_Death && GetCombatState() != ECombatState::ECS_Flinching &&
		GetCombatState() != ECombatState::ECS_KnockDown && !GetCharacterMovement()->IsFalling() && !bIsLevitating && !GetCharacterMovement()->IsFlying())
	{
		return true;
	}
	return false;
}


void ABaseEnemy::Landed(const FHitResult& HitResult)
{
	// Falling -> Hit Ground
	Super::Landed(HitResult);
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
	if (GetCombatState() == ECombatState::ECS_Death)
	{
		return;
	}
	SetCombatState(ECombatState::ECS_None);

	
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
	ECombatState PrevCharacterState = GetCombatState();

	// Set State
	SetCombatState(ECombatState::ECS_Death);
	InfoWidgetComponent->SetVisibility(false);
	
	// Stop AI Logic
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController != nullptr)
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		UBrainComponent* BrainComponent = AIController->GetBrainComponent();
		if (BrainComponent != nullptr)
		{
			BrainComponent->StopLogic("Die");
		}
	}
		
	// Play Animation, Not to Play Dead Animation more than once (Execution Case)
	if (PrevCharacterState != ECombatState::ECS_Death)
	{
		if (FallingDeathMontage == nullptr || StandingDeathMontage == nullptr)
		{
			GetMesh()->SetSimulatePhysics(true);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			return;
		}
		else if (GetCharacterMovement()->IsFalling())
		{
			PlayAnimMontage(FallingDeathMontage);
		}
		else
		{
			PlayAnimMontage(StandingDeathMontage);
		}
	}
	DisableCollision();
	
	for (const TPair<AActor*, int32>& Pair : ReservedAttackTokensMap)
	{
		AActor* AttackTarget = Pair.Key;
		int32 Tokens = Pair.Value;
		ICombatInterface* CombatInterface = Cast<ICombatInterface>(AttackTarget);
		if (CombatInterface != nullptr)
		{
			CombatInterface->ReturnAttackTokens(Tokens);
		}
	}
}


void ABaseEnemy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (NormalAttackMontages.Contains(Montage))
	{
		NotifyAttackEnd(CachedAttackTarget);
	}
	else if (Montage == GuardMontage || Montage == GuardRevengeMontage)
	{
		NotifyGuardEnd();
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
		//ResetState();
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

void ABaseEnemy::AerialHitAnimMapping(AActor* Attacker, FDamageTypeMapping* DamageMapping, EASRDamageType DamageType)
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
		DamageMapping = FindDamageDTRow(EASRDamageType::EDT_AerialHit);
		break;
	case EASRDamageType::EDT_AerialStart:
		DamageMapping = FindDamageDTRow(EASRDamageType::EDT_AerialStart);
		break;

	case EASRDamageType::EDT_AerialKnockDown:
		DamageMapping = FindDamageDTRow(EASRDamageType::EDT_AerialKnockDown);
		UE_LOG(LogTemp, Warning, TEXT("AerialKnockDownAnim!!"));

		break;
	case EASRDamageType::EDT_MAX:
		UE_LOG(LogTemp, Warning, TEXT("EDT_MAX No Mapping!"));
		DamageMapping = FindDamageDTRow(EASRDamageType::EDT_Default);
		break;
	default:
		DamageMapping = FindDamageDTRow(EASRDamageType::EDT_Default);
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

void ABaseEnemy::SetCombatState(ECombatState InCombatState)
{
	if (InCombatState != GetCombatState() && GetCombatState() != ECombatState::ECS_Death)
	{
		CombatState = InCombatState;
		OnCombatStateChanged.Broadcast(InCombatState);
	}
}

void ABaseEnemy::SetHitReactionState(EHitReactionState NewState)
{
	if (NewState != GetHitReactionState())
	{
		HitReactionState = NewState;
		OnHitReactionStateChanged.Broadcast(NewState);
	}
}

bool ABaseEnemy::ReserveAttackTokens(int32 Amount)
{
	if (AttackTokensCount >= Amount)
	{
		AttackTokensCount -= Amount;
		return true;
	}
	return false;
}

void ABaseEnemy::ReturnAttackTokens(int32 Amount)
{
	AttackTokensCount += Amount;
}

UDataTable* ABaseEnemy::GetAttackDataTable() const
{
	return AttackDataTable;
}

bool ABaseEnemy::IsAttackFromFront(const FHitResult& HitResult) const
{
	FVector AttackDirection = (HitResult.TraceStart - HitResult.TraceEnd).GetSafeNormal();
	FVector CharacterForward = GetActorForwardVector();
	FVector NormalizedAttackDirection = AttackDirection.GetSafeNormal();

	float DotProduct = FVector::DotProduct(CharacterForward, NormalizedAttackDirection);

	// forward 120 degrees
	return DotProduct > 0.5f;
}

void ABaseEnemy::GetHit(const FHitResult& HitResult, AActor* Attacker, const FHitData& HitData)
{
	// [TODO] Block Handling Before Take Damage
	

	if (GetCombatState() == ECombatState::ECS_Death)
	{ 
		return;
	}
	
	if (Cast<APawn>(Attacker) == nullptr)
	{
		return;
	}


	//if (GetCombatState() == ECombatState::ECS_Guard && IsAttackFromFront(HitResult))
	if (GetCombatState() == ECombatState::ECS_Guard && 
		HitReactionState == EHitReactionState::EHR_Parry && 
		IsAttackFromFront(HitResult))
	{
		SetHitReactionState(EHitReactionState::EHR_SuperArmor);
		//FVector KnockbackForce = -GetActorForwardVector() * HitData.Damage * 10;
		//LaunchCharacter(KnockbackForce, true, true);
		PlayAnimMontage(GuardRevengeMontage);	
		return;
	}

	// TODO - Improvement
	if (FMath::RandRange(0.f, 1.f) < AutoGuardRate &&
		!GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
	{
		SetCombatState(ECombatState::ECS_Guard);
		SetHitReactionState(EHitReactionState::EHR_SuperArmor);
		PlayAnimMontage(GuardRevengeMontage);
		return;
	}



	if (Cast<AASRCharacter>(Attacker) != nullptr)
	{
		ApplyHitStop(HitStopDuration, HitStopTimeDilation);
		AASRCharacter* ASRCharacter = Cast<AASRCharacter>(Attacker);
		ASRCharacter->ApplyHitStop(HitStopDuration, HitStopTimeDilation);
		UAISense_Damage::ReportDamageEvent(
			GetWorld(),
			this,
			ASRCharacter,
			HitData.Damage,
			GetActorLocation(),
			FVector::ZeroVector
		);
	}


	SetHealth(Health - HitData.Damage);
	UE_LOG(LogTemp, Warning, TEXT("HEALTH: %f"), Health);

	// Effects
	if (HitData.HitSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this,
			HitData.HitSound, GetActorLocation());
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
	else if (HitData.HitParticleEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitData.HitParticleEffect,
			HitResult.ImpactPoint
		);
	}

	// Death
	if (Health <= 0 && !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
	{
		HandleDeath();
		return;
	}

	// Animation
	FDamageTypeMapping* DamageMapping;
	UE_LOG(LogTemp, Warning, TEXT("Find DT Row Check"));
	DamageMapping = FindDamageDTRow(HitData.DamageType);
	if (DamageMapping != nullptr)
	{
		if (DamageMapping->CombatState != ECombatState::ECS_Death && GetHitReactionState() == EHitReactionState::EHR_SuperArmor)
		{
			return;
		}
		SetCombatState(DamageMapping->CombatState);
		if (DamageMapping->CombatState == ECombatState::ECS_Flinching || DamageMapping->CombatState == ECombatState::ECS_KnockDown)
		{
			GetCharacterMovement()->StopMovementImmediately();
		}
		RotateToAttacker(Attacker, false);
		HandleHitTransform(Attacker, HitData.DamageType, HitData.Damage);

		if (GetCharacterMovement()->IsFalling() || GetCharacterMovement()->IsFlying())
		{
			AerialHitAnimMapping(Attacker, DamageMapping, HitData.DamageType);
		}

		UAnimMontage* LoadedMontage = DamageMapping->HitReactionMontage;
		if (LoadedMontage != nullptr)
		{
			PlayAnimMontage(LoadedMontage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to Load Animation Montage"));
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL DamageType Mapping!"));
	}
}

bool ABaseEnemy::IsDead() const
{
	return GetCombatState() == ECombatState::ECS_Death;
}

ECombatState ABaseEnemy::GetCombatState() const
{
	return CombatState;
}

EHitReactionState ABaseEnemy::GetHitReactionState() const
{
	return HitReactionState;
}

APatrolRoute* ABaseEnemy::GetPatrolRoute() const
{
	return PatrolRoute;
}

float ABaseEnemy::SetMovementSpeed(EEnemyMovementSpeed EnemyMovementSpeed)
{
	float NewSpeed;

	switch (EnemyMovementSpeed)
	{
	case EEnemyMovementSpeed::EMS_Idle:
		NewSpeed = 0.f;
		break;
	case EEnemyMovementSpeed::EMS_Walk:
		NewSpeed = 150.f;
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

float ABaseEnemy::GetCurrentHealth() const
{
	return Health;
}

float ABaseEnemy::GetMaxHealth() const
{
	return MaxHealth;
}

bool ABaseEnemy::AttackBegin(AActor* AttackTarget, int32 RequiredTokens)
{
	// Check if the required number of tokens is available
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(AttackTarget);
	if (CombatInterface != nullptr)
	{
		bool bSuccess = CombatInterface->ReserveAttackTokens(RequiredTokens);
		if (bSuccess)
		{
			StoreAttackTokens(AttackTarget, RequiredTokens);
			CachedLastUsedTokensCount = RequiredTokens;
			return true;
		}
	}
	return false;
}

void ABaseEnemy::Attack(AActor* AttackTarget)
{
	// Attack State True
	
	//
}

void ABaseEnemy::AttackEnd(AActor* AttackTarget)
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(AttackTarget);
	if (CombatInterface != nullptr)
	{
		CombatInterface->ReturnAttackTokens(CachedLastUsedTokensCount);
	}
	StoreAttackTokens(AttackTarget, -CachedLastUsedTokensCount);
	// Attack State Off
}

void ABaseEnemy::StoreAttackTokens(AActor* AttackTarget, int32 Amount)
{
	if (ReservedAttackTokensMap.Contains(AttackTarget))
	{
		int32& ExistingAmount = ReservedAttackTokensMap[AttackTarget];
		ExistingAmount += Amount;
	}
	else
	{
		ReservedAttackTokensMap.Add({ AttackTarget, Amount });
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
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(HitActor);
				if (CombatInterface != nullptr)
				{
					//UGameplayStatics::PlaySoundAtLocation(this, HitSoundCue, HitActor->GetActorLocation());
					CombatInterface->GetHit(HitResult, this, HitData);
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

FDamageTypeMapping* ABaseEnemy::FindDamageDTRow(EASRDamageType DamageType) const
{
	FDamageInfoData* DamageInfoData = nullptr;
	FText RowText;
	UEnum::GetDisplayValueAsText(DamageType, RowText);

	UE_LOG(LogTemp, Warning, TEXT("Find DT Row TEXT: %s"), *RowText.ToString());


	if (DamageDataTable != nullptr)
	{
		DamageInfoData = DamageDataTable->FindRow<FDamageInfoData>(*RowText.ToString(), FString::Printf(TEXT("Failed to Find: [%s] %s"), *GetName(), *RowText.ToString()));
		if (DamageInfoData == nullptr)
		{
			DamageInfoData = DamageDataTable->FindRow<FDamageInfoData>(FName("Default"), FString::Printf(TEXT("Failed to Find Default: [%s] %s"), *GetName(), *RowText.ToString()));
		}
		return &DamageInfoData->DamageReaction;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL DT!"));

	}
	return nullptr;
}
