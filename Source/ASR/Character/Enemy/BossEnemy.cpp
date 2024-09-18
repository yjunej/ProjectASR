// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "Components/WidgetComponent.h"
#include "ASR/HUD/EnemyInfoWidget.h"


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
