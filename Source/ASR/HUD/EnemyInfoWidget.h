// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API UEnemyInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	UFUNCTION(BlueprintCallable)
	void UpdateHealthBar();

	UFUNCTION(BlueprintCallable)
	void UpdateStaminaBar();

	UFUNCTION(BlueprintCallable)
	void UpdatePostHealthBar();

	UFUNCTION(BlueprintCallable)
	void UpdatePostStaminaBar();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ABaseEnemy* Owner;


	UPROPERTY(meta = (BindWidget))
	class UTextBlock* EnemyNameText;


	UPROPERTY(meta = (BindWidget))
	class UProgressBar* EnemyHealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* EnemyPostHealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* EnemyStaminaBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* EnemyPostStaminaBar;

	void SetOwner(ABaseEnemy* NewOwner);

	float PostBarLerpSpeed = 1.5f;
	float PostHealth;
	float PostStamina;
	
};
