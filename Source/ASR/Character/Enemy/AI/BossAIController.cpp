// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAIController.h"
#include "ASR/Character/Enemy/BossEnemy.h"
#include "ASR/Character/ASRCharacter.h"
#include "Components/VerticalBox.h"
#include "ASR/HUD/EnemyInfoWidget.h"
#include "ASR/HUD/ASRMainHUD.h"

void ABossAIController::SwitchToAttackState(AActor* TargetActor)
{
	bool bIsFirstTarget = AttackTarget == nullptr;
	Super::SwitchToAttackState(TargetActor);

	// FirstTime Get Attack Target
	if (bIsFirstTarget)
	{
		AASRCharacter* ASRCharacter = Cast<AASRCharacter>(TargetActor);
		if (ASRCharacter != nullptr)
		{
			ABossEnemy* BossEnemy = Cast<ABossEnemy>(GetPawn());
			if (BossEnemy != nullptr)
			{
				// TODO
				if (ASRCharacter->MainHUDWidget != nullptr)
				{
					ASRCharacter->MainHUDWidget->BossInfoVBox->AddChildToVerticalBox(Cast<UUserWidget>(BossEnemy->GetBossInfoWidget()));
				}

			}
		}
	}
}
