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
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	
	void SetBlackboardKeys();

	UFUNCTION(BlueprintCallable)
	void SwitchToPassiveState();

	UFUNCTION(BlueprintCallable)
	void SwitchToAttackState(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void SwitchToInvestigateState(FVector InvestigateLocaton);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StrafeDistance = 800.f;

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

	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPerceptionUpdatedDelegate, const TArray<AActor*>&, UpdatedActors);

};
