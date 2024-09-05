// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RangerHUD.generated.h"


/**
 * 
 */
UCLASS()
class ASR_API URangerHUD : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(meta=(BindWidget))
	class UBorder* CrosshairCenter;

	UPROPERTY(meta = (BindWidget))
	class UBorder* CrosshairTop;

	UPROPERTY(meta = (BindWidget))
	class UBorder* CrosshairBottom;

	UPROPERTY(meta = (BindWidget))
	class UBorder* CrosshairLeft;

	UPROPERTY(meta = (BindWidget))
	class UBorder* CrosshairRight;

	UPROPERTY(meta = (BindWidget))
	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY(EditAnywhere)
	float CrosshairInterval = 30.f;

	float CrosshairSpreadRate = 1.f;
	
	void SetCrosshairVisibility(ESlateVisibility InVisibility);
	void SetCrosshairColor(FLinearColor Color);

	void SetCrosshair();
};
