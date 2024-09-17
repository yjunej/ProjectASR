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

public:
	UFUNCTION(BlueprintCallable)
	void UpdateHealthBar();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ABaseEnemy* Owner;


	UPROPERTY(meta = (BindWidget))
	class UTextBlock* EnemyNameText;


	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	
};
