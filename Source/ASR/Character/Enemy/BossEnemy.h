// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "BossEnemy.generated.h"


UCLASS()
class ASR_API ABossEnemy : public ABaseEnemy
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& HitResult) override;


	UFUNCTION(BlueprintCallable)
	virtual bool LaunchForJumpSmash(bool bIsJump, float Arc, float TimeToTarget);

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UEnemyInfoWidget> BossInfoWidgetClass;

	UPROPERTY()
	UEnemyInfoWidget* BossInfoWidget;

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	bool BossAIAttack(AActor* AttackTarget, EAIAttack BossAttackType);




public:
	UEnemyInfoWidget* GetBossInfoWidget() const;
};
