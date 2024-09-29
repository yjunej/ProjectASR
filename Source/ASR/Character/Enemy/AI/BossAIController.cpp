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

	AASRCharacter* ASRCharacter = Cast<AASRCharacter>(TargetActor);
	bool bIsFirstTarget = false;
	if (ASRCharacter != nullptr)
	{
		bIsFirstTarget = true;
	}


	UE_LOG(LogTemp, Warning, TEXT("Boss Info Widget Added -1"));
	if (AttackTarget != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATTACK TARGET: %s"), *AttackTarget->GetName());
	}
	if (TargetActor != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("TARGET ACTOR: %s"), *TargetActor->GetName());
	}


	Super::SwitchToAttackState(TargetActor);
	UE_LOG(LogTemp, Warning, TEXT("Boss Info Widget Added 0"));

	// FirstTime Get Attack Target
	if (bIsFirstTarget)
	{
		if (ASRCharacter != nullptr)
		{
			ABossEnemy* BossEnemy = Cast<ABossEnemy>(GetPawn());
			UE_LOG(LogTemp, Warning, TEXT("Boss Info Widget Added 1"));
			if (BossEnemy != nullptr)
			{
				if (ASRCharacter->MainHUDWidget != nullptr && ASRCharacter->MainHUDWidget->BossInfoCanvas->GetChildrenCount() == 0)
				{
					UCanvasPanelSlot* NewSlot = ASRCharacter->MainHUDWidget->BossInfoCanvas->AddChildToCanvas
						(Cast<UUserWidget>(BossEnemy->GetBossInfoWidget()));
					UE_LOG(LogTemp, Warning, TEXT("Boss Info Widget Added"))
					if (NewSlot != nullptr)	
					{
						NewSlot->SetAlignment(FVector2D(0.5f, 0.f));
						NewSlot->SetAnchors(FAnchors(0.5f, 0.f, 0.5f, 0.f));
						UE_LOG(LogTemp, Warning, TEXT("Boss Info Widget Added!"))

					}
				}

			}
		}
	}
}
