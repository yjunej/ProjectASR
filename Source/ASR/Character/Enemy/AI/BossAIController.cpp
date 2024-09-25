// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAIController.h"
#include "ASR/Character/Enemy/BossEnemy.h"
#include "ASR/Character/ASRCharacter.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "ASR/HUD/EnemyInfoWidget.h"
#include "ASR/HUD/ASRMainHUD.h"

ABossAIController::ABossAIController()
{
	bUseBaseInfoWidget = false;
}

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
					UCanvasPanelSlot* NewSlot = ASRCharacter->MainHUDWidget->BossInfoCanvas->AddChildToCanvas
						(Cast<UUserWidget>(BossEnemy->GetBossInfoWidget()));
					if (NewSlot != nullptr)	
					{
						NewSlot->SetAlignment(FVector2D(0.5f, 0.f));
						NewSlot->SetAnchors(FAnchors(0.5f, 0.f, 0.5f, 0.f));
					}
				}

			}
		}
	}
}
