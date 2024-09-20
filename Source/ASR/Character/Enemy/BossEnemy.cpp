// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "Components/WidgetComponent.h"
#include "ASR/Character/Enemy/BaseAIController.h"
#include "ASR/HUD/EnemyInfoWidget.h"
#include "Kismet/GameplayStatics.h"


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
			FVector OffsetVector =  (SourceLocation - TargetLocation).GetSafeNormal() * 300.f;
			if (FVector::Dist(SourceLocation, TargetLocation) < 700.f)
			{
				OffsetVector /= 3.f;
			}
			UGameplayStatics::SuggestProjectileVelocity_MovingTarget(this, LaunchVelocity, SourceLocation, AttackTarget, OffsetVector, 0.f, TimeToTarget);			
		}
		LaunchCharacter(LaunchVelocity, true, true);
	}
	return false;

		// TODO
	
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

UEnemyInfoWidget* ABossEnemy::GetBossInfoWidget() const
{
	return BossInfoWidget;
}
