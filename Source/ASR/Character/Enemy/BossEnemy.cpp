// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "Components/WidgetComponent.h"
#include "ASR/Character/Enemy/BaseAIController.h"
#include "ASR/HUD/EnemyInfoWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"


void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetInfoWidgetComponent()->SetWidgetClass(nullptr);	
	BossInfoWidget = Cast<UEnemyInfoWidget>(CreateWidget(GetWorld(), BossInfoWidgetClass));
	if (BossInfoWidget != nullptr)
	{
		BossInfoWidget->SetOwner(this);
	}
}

void ABossEnemy::Landed(const FHitResult& HitResult)
{
	ACharacter::Landed(HitResult);
}

void ABossEnemy::ProcessHitAnimation(const FHitData& HitData, AActor* Attacker)
{
	FDamageTypeMapping* DamageMapping;
	UE_LOG(LogTemp, Warning, TEXT("Find DT Row Check"));
	DamageMapping = FindDamageDTRow(HitData.DamageType);
	if (DamageMapping != nullptr)
	{
		//SetCombatState(DamageMapping->CombatState);
		//RotateToAttacker(Attacker, false);
		//HandleHitTransform(Attacker, HitData.DamageType, HitData.Damage);
		BossPlayHitAnimation(HitData, DamageMapping, Attacker);
	}
}
float ABossEnemy::SetMovementSpeed(EEnemyMovementSpeed EnemyMovementSpeed)
{
	float NewSpeed;

	switch (EnemyMovementSpeed)
	{
	case EEnemyMovementSpeed::EMS_Idle:
		NewSpeed = 0.f;
		break;
	case EEnemyMovementSpeed::EMS_Walk:
		NewSpeed = 180.f;
		break;
	case EEnemyMovementSpeed::EMS_Jog:
		NewSpeed = 300.f;
		break;
	case EEnemyMovementSpeed::EMS_Run:
		NewSpeed = 500.f;
		break;
	case EEnemyMovementSpeed::EMS_Sprint:
		NewSpeed = 600.f;
		break;
	case EEnemyMovementSpeed::EMS_MAX:
	default:
		NewSpeed = 0.f;
		break;
	}
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	return NewSpeed;
}

void ABossEnemy::GuardBroken()
{
	DamageMultiplier += 0.5f;
	Super::GuardBroken();
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABossEnemy::RefillStamina, 0.5f, false);
	OnStaminaChanged.Broadcast();
}


int32 ABossEnemy::ModifyAttackMontage(EAIAttack AIAttackType, int32 SelectedIndex)
{

	//TArray<UAnimMontage*> AttackMontages;
	//FAIAttackMontages* FoundMontages = AttackMontageMap.Find(AIAttackType);
	//if (FoundMontages != nullptr && FoundMontages->AIAttackMontages.Num() > 0)
	if (AIAttackType == EAIAttack::EAA_Default)
	{
		// if Last Default Attack
		int32 NumOfMontages = AttackMontageMap.Find(EAIAttack::EAA_Default)->AIAttackMontages.Num();
		if (SelectedIndex == NumOfMontages- 1)
		{
			if (IsUltAttackReady())
			{
				StartUltCoolDown();
				return SelectedIndex;
			}
			else
			{
				return FMath::RandRange(0, NumOfMontages - 2);
			}
		}
	}
	return SelectedIndex;
}

void ABossEnemy::BossPlayHitAnimation(const FHitData& HitData, FDamageTypeMapping* DamageMapping, AActor* Attacker)
{

	//if (HitData.bIsLethalAttack)
	//{
	//			SetCombatState(DamageMapping->CombatState);
	//	UAnimMontage* LoadedMontage = DamageMapping->HitReactionMontage;
	//	PlayAnimMontage(LoadedMontage, 1.f);
	//}
	// 
	// 

	if (HitData.bIsFatalAttack || HitData.bIsLethalAttack)
	{
		SetCombatState(DamageMapping->CombatState);
		UAnimMontage* LoadedMontage = DamageMapping->HitReactionMontage;
		if (LoadedMontage != nullptr)
		{
			EHitDirection HitDirection = GetHitDirection(Attacker->GetActorLocation());
			FName SectionName = "Front";
			switch (HitDirection)
			{
			case EHitDirection::EHD_Back:
				SectionName = "Back";
				break;
			case EHitDirection::EHD_Left:
				SectionName = "Left";
				break;
			case EHitDirection::EHD_Right:
				SectionName = "Right";
				break;
			case EHitDirection::EHD_Front:
			case EHitDirection::EHD_MAX:
			default:
				break;
			}
			if (LoadedMontage->IsValidSectionName(SectionName))
			{
				PlayAnimMontage(LoadedMontage, 1.f, SectionName);
			}
			else
			{
				PlayAnimMontage(LoadedMontage, 1.f);
			}
		}
	}

	if (Stamina > 0)
	{
		return;
	}

	
	// 1. If KnockDownFrontBig, Always Flinch, Although Super Armor
	// 2. Flinch Rate Check

	if (HitData.DamageType == EASRDamageType::EDT_KnockDownFrontBig || Stamina <= 0.f ||
		(FMath::RandRange(0.f, 1.f) < FlinchRate && GetHitReactionState() != EHitReactionState::EHR_None && GetCombatState() == ECombatState::ECS_None))
	{
		SetCombatState(DamageMapping->CombatState);
		UAnimMontage* LoadedMontage = DamageMapping->HitReactionMontage;
		PlayAnimMontage(LoadedMontage, 1.f);
	}
}

bool ABossEnemy::LaunchForJumpSmash(bool bIsJump, float Arc, float TimeToTarget)
{
	ABaseAIController* AIController = Cast<ABaseAIController>(GetController());
	if (AIController != nullptr)
	{
		AActor* AttackTarget = AIController->AttackTarget;
		FVector TargetLocation = AttackTarget->GetActorLocation();
		FVector SourceLocation = GetActorLocation();
		FVector LaunchVelocity = FVector::ZeroVector;
		if (bIsJump)
		{
			// If Distance is less than 700, Adjust Arc
			if (FVector::Dist(SourceLocation, TargetLocation) < 700.f)
			{
				Arc /= 2;
			}
			UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, LaunchVelocity, SourceLocation, TargetLocation, 0.f, Arc);

		}
		else
		{
			// Adjust Target Location
			FVector OffsetVector =  (SourceLocation - TargetLocation).GetSafeNormal() * 450.f;
			if (FVector::Dist(SourceLocation, TargetLocation) < 700.f)
			{
				OffsetVector /= 2.f;
			}
			UGameplayStatics::SuggestProjectileVelocity_MovingTarget(this, LaunchVelocity, SourceLocation, AttackTarget, OffsetVector, 0.f, TimeToTarget);			
		}
		LaunchCharacter(LaunchVelocity, true, true);
	}
	return false;

	
}


bool ABossEnemy::BossAIAttack(AActor* AttackTarget, EAIAttack BossAttackType)
{
	// No Token System
	if (CanAttack())
	{
		return ExecuteAIAttack(AttackTarget, BossAttackType);
	}
	return false;
}

bool ABossEnemy::IsUltAttackReady() const
{
	return !GetWorld()->GetTimerManager().IsTimerActive(BossUltTimerHandle);
}

// TODO - Sophisticated CoolDown System
void ABossEnemy::StartUltCoolDown()
{
	GetWorld()->GetTimerManager().SetTimer(BossUltTimerHandle, this, &ABossEnemy::ResetUltCoolDown, BossUltCooldown, false);
}

void ABossEnemy::ResetUltCoolDown()
{
	return;
}


void ABossEnemy::RefillStamina()
{
	SetStamina(MaxStamina);
}

UEnemyInfoWidget* ABossEnemy::GetBossInfoWidget() const
{
	return BossInfoWidget;
}
