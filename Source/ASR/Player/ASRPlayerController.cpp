// Fill out your copyright notice in the Description page of Project Settings.


#include "ASRPlayerController.h"

#include "ASR/HUD/RangerHUD.h"
#include "ASR/HUD/CharacterOverlay.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

void AASRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (RangerHUDClass != nullptr)
	{
		RangerHUD = CreateWidget<URangerHUD>(GetWorld(), RangerHUDClass);
		RangerHUD->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		RangerHUD->AddToViewport();
	}
}

void AASRPlayerController::SetKillScore(float KillScore)
{
	if (RangerHUD != nullptr && RangerHUD->CharacterOverlay != nullptr)
	{
		FString KillScoreString = FString::Printf(TEXT("%d"), FMath::FloorToInt(KillScore));
		RangerHUD->CharacterOverlay->KillScoreText->SetText(FText::FromString(KillScoreString));
	}
}

void AASRPlayerController::SetRangerAmmo(int32 Ammo)
{
	if (RangerHUD != nullptr && RangerHUD->CharacterOverlay != nullptr)
	{
		FString AmmoString = FString::Printf(TEXT("%d"),Ammo);
		RangerHUD->CharacterOverlay->AmmoAmountText->SetText(FText::FromString(AmmoString));
	}
}
