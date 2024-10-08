// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "BossEnemy.generated.h"


UCLASS()
class ASR_API ABossEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float FlinchRate = 0.15f;

protected:
	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& HitResult) override;
	virtual void ProcessHitAnimation(const FHitData& HitData, AActor* Attacker) override;
	virtual float SetMovementSpeed(EEnemyMovementSpeed EnemyMovementSpeed) override;
	
	// Inherit Parent Hook method for Boss Enemy CoolDown system
	virtual int32 ModifyAttackMontage(EAIAttack AIAttackType, int32 SelectedIndex) override;

	void BossPlayHitAnimation(const FHitData& HitData, FDamageTypeMapping* DamageMapping, AActor* Attacker);

	UFUNCTION(BlueprintCallable)
	virtual bool LaunchForJumpSmash(bool bIsJump, float Arc, float TimeToTarget);

	

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UEnemyInfoWidget> BossInfoWidgetClass;

	UPROPERTY()
	UEnemyInfoWidget* BossInfoWidget;

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	bool BossAIAttack(AActor* AttackTarget, EAIAttack BossAttackType);

	// Boss Ult Timer 
	FTimerHandle BossUltTimerHandle;

	bool IsUltAttackReady() const;
	void StartUltCoolDown();
	void ResetUltCoolDown();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta=(AllowPrivateAccess = "true"))
	float BossUltCooldown = 5.f;


public:
	UEnemyInfoWidget* GetBossInfoWidget() const;
};
