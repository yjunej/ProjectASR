// Fill out your copyright notice in the Description page of Project Settings.


#include "RangerHUD.h"

#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"


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
