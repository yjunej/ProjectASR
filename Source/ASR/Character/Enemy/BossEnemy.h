// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API ABossEnemy : public ABaseEnemy
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UEnemyInfoWidget> BossInfoWidgetClass;

	UPROPERTY()
	UEnemyInfoWidget* BossInfoWidget;

public:
	UEnemyInfoWidget* GetBossInfoWidget() const;
};
