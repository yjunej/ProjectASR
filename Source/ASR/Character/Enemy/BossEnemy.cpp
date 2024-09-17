// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "Components/WidgetComponent.h"
#include "ASR/HUD/EnemyInfoWidget.h"


void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetInfoWidgetComponent()->SetWidgetClass(nullptr);	
	BossInfoWidget = Cast<UEnemyInfoWidget>(CreateWidget(this, BossInfoWidgetClass));
}

UEnemyInfoWidget* ABossEnemy::GetBossInfoWidget() const
{
	return BossInfoWidget;
}
