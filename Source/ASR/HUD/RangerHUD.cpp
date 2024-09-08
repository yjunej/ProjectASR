// Fill out your copyright notice in the Description page of Project Settings.


#include "RangerHUD.h"

#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"


void URangerHUD::SetCrosshairVisibility(ESlateVisibility InVisibility)
{
	CrosshairCenter->SetVisibility(InVisibility);
	CrosshairTop->SetVisibility(InVisibility);
	CrosshairBottom->SetVisibility(InVisibility);
	CrosshairLeft->SetVisibility(InVisibility);
	CrosshairRight->SetVisibility(InVisibility);
}

void URangerHUD::SetCrosshairColor(FLinearColor Color)
{
	CrosshairCenter->Background.TintColor = Color;
	CrosshairTop->Background.TintColor = Color;
	CrosshairBottom->Background.TintColor = Color;
	CrosshairLeft->Background.TintColor = Color;
	CrosshairRight->Background.TintColor = Color;
}
	


void URangerHUD::SetCrosshair()
{
	UCanvasPanelSlot* CanvasPanelSlot;
	FVector2D NewPosition;

	CanvasPanelSlot = Cast<UCanvasPanelSlot>(CrosshairTop->Slot);
	NewPosition = CanvasPanelSlot->GetPosition();
	NewPosition.Y = -CrosshairInterval * CrosshairSpreadRate;
	CanvasPanelSlot->SetPosition(NewPosition);

	CanvasPanelSlot = Cast<UCanvasPanelSlot>(CrosshairBottom->Slot);
	NewPosition = CanvasPanelSlot->GetPosition();
	NewPosition.Y = CrosshairInterval * CrosshairSpreadRate;
	CanvasPanelSlot->SetPosition(NewPosition);

	CanvasPanelSlot = Cast<UCanvasPanelSlot>(CrosshairLeft->Slot);
	NewPosition = CanvasPanelSlot->GetPosition();
	NewPosition.X = -CrosshairInterval * CrosshairSpreadRate;
	CanvasPanelSlot->SetPosition(NewPosition);

	CanvasPanelSlot = Cast<UCanvasPanelSlot>(CrosshairRight->Slot);
	NewPosition = CanvasPanelSlot->GetPosition();
	NewPosition.X = CrosshairInterval * CrosshairSpreadRate;
	CanvasPanelSlot->SetPosition(NewPosition);
}

void URangerHUD::SetUltOverlay(bool IsEnable)
{
	if (IsEnable)
	{
		UltOverlayImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		AmmoProgressBar->SetFillColorAndOpacity(FLinearColor::Blue);
		AmmoProgressBar->SetPercent(1.f);
		SetCrosshairColor(FLinearColor::Blue);
		PlayAnimation(UltOverlayImageAnim, 0.0f, 0.f, EUMGSequencePlayMode::PingPong);
	}
	else
	{
		UltOverlayImage->SetVisibility(ESlateVisibility::Hidden);
		AmmoProgressBar->SetFillColorAndOpacity(FLinearColor::Green);
		AmmoProgressBar->SetPercent(1.f);
		SetCrosshairColor(FLinearColor::Green);
		StopAnimation(UltOverlayImageAnim);
	}
}

void URangerHUD::SetAmmoProgressBarPercent(float AmmoPercent)
{
	if (AmmoProgressBar != nullptr)
	{
		AmmoProgressBar->SetPercent(AmmoPercent);
	}
}
