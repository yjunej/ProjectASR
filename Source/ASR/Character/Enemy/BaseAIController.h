// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ASR/Enums/AIPerceptionSense.h"
#include "ASR/Enums/EnemyAIState.h"
#include "Perception/AIPerceptionTypes.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseAIController();

	EEnemyAIState GetCurrentAIState();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* AttackTarget;
	
	UFUNCTION()
	void OnAICombatStateChanged(ECombatState NewState);

	UFUNCTION()
	void OnPlayerCombatStateChanged(ECombatState NewState);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	
	void SetBlackboardKeys();


	UFUNCTION(BlueprintCallable)
	void SwitchToPassiveState();

	UFUNCTION(BlueprintCallable)
	virtual void SwitchToAttackState(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void SwitchToInvestigateState(FVector InvestigateLocaton);

	UFUNCTION(BlueprintCallable)
	void ExecuteNormalAttack();

	FTimerHandle TimerHandle;

	// Blackboard Keys
	UPROPERTY(EditDefaultsOnly)
	FName AttackTargetKeyName = "AttackTarget";

	UPROPERTY(EditDefaultsOnly)
	FName AIStateKeyName = "AIState";

	UPROPERTY(EditDefaultsOnly)
	FName AttentionPointKeyName = "AttentionPoint";

	UPROPERTY(EditDefaultsOnly)
	FName CombatStateKeyName = "CombatState";

	UPROPERTY(EditDefaultsOnly)
	FName AttackTargetCombatStateKeyName = "AttackTargetCombatState";

	UPROPERTY(EditDefaultsOnly)
	FName AttackDistanceKeyName = "AttackDistance";

	UPROPERTY(EditDefaultsOnly)
	FName DefendDistanceKeyName = "DefendDistance";
	//
	

	// Perception
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAIPerceptionComponent* AIPerception;

	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearingConfig;
	class UAISenseConfig_Damage* DamageSenseConfig;

	FAIStimulus CanSenseActor(AActor* Actor, EAIPerceptionSense AIPerceptionSense);

	void HandleSensedSight(AActor* Actor);
	void HandleSensedHearing(FVector NoiseLocation);
	void HandleSensedDamage(AActor* Actor);


	UFUNCTION()
	void PerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	TArray<AActor*> SightedActors;

	UFUNCTION(BlueprintCallable)
	bool CheckForgottenSightedActor();
	void HandleForgottenActor(AActor* ForgottenActor);
	void HandleLostSight(AActor* LostSightedActor);


	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPerceptionUpdatedDelegate, const TArray<AActor*>&, UpdatedActors);

public:
	FORCEINLINE FName GetAttackTargetKeyName() const { return AttackTargetKeyName; }
	FORCEINLINE FName GetAttentionPointKeyName() const { return AttentionPointKeyName; }

};
