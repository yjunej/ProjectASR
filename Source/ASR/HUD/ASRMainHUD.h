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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION()
	void UpdateHealthBar();

	UFUNCTION()
	void UpdateStaminaBar();

	UFUNCTION()
	void UpdatePostHealthBar();

	UFUNCTION()
	void UpdatePostStaminaBar();
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AASRCharacter* Owner;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PostHealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PostStaminaBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* BossInfoCanvas;

	float PostBarLerpSpeed = 1.5f;
	float PostHealth;
	float PostStamina;

};
