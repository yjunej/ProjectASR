// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ASRMainHUD.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API UASRMainHUD : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void UpdateHealthBar();

	UFUNCTION()
	void UpdateStaminaBar();
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AASRCharacter* Owner;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UVerticalBox* BossInfoVBox;
};
