// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyLockOnWidget.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API UEnemyLockOnWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SelectMarker();


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ABaseEnemy* Owner;

	UPROPERTY(meta = (BindWidget))
	class UImage* LockOnMarker;

	UPROPERTY(meta = (BindWidget))
	class UImage* ExecutionMarker;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* CanExecute;

};
