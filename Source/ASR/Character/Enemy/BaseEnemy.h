// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASR/Interfaces/HitInterface.h"
#include "ASR/Interfaces/EnemyAIInterface.h"
#include "ASR/Enums/ASRDamageType.h"
#include "ASR/Character/ASRCharacter.h" // Refactoring After Complete Basic System

#include "BaseEnemy.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGuardEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitReactionStateChanged, EHitReactionState, NewState);



UENUM(BlueprintType)
enum class EEnemyBehaviorState : uint8
{
	EBS_None						UMETA(DisplayName="None"),
	EBS_Chase						UMETA(DisplayName="Chase"),
	EBS_Strafe						UMETA(DisplayName="Strafe"),
	EBS_Attack						UMETA(DisplayName="Attack"),
	EBS_Disabled					UMETA(DisplayName="Disabled"),
	EBS_Dead						UMETA(DisplayName="Dead"),

	EBS_MAX							UMETA(Hidden)
};



UCLASS()
class ASR_API ABaseEnemy : public ACharacter, public IHitInterface, public IEnemyAIInterface
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// HitInterface
	virtual void GetHit(const FHitResult& HitResult, AActor* Attacker, const FHitData& HitData) override;
	virtual bool IsDead() const override;
	virtual ECombatState GetCombatState() const override;
	virtual EHitReactionState GetHitReactionState() const override;
	virtual void SetHitReactionState(EHitReactionState NewState) override;


	// EnemyAIInterface - Blueprint Compatable
	virtual APatrolRoute* GetPatrolRoute() const override;
	virtual float SetMovementSpeed(EEnemyMovementSpeed EnemyMovementSpeed) override;
	virtual float GetCurrentHealth() const override;
	virtual float GetMaxHealth() const override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float MaxHealth = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float ExecutionThresholdHealth = 200.f;

	//
	UPROPERTY(BlueprintAssignable, Category= AI)
	FOnAttackEnd OnAttackEnd;

	UPROPERTY(BlueprintAssignable, Category = AI)
	FOnGuardEnd OnGuardEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	bool bIsCombatReady = false;


	// Custom Notify For BT
	void NotifyAttackEnd();
	void NotifyGuardEnd();

	FOnHealthChanged OnHealthChanged;

	void SetHealth(float NewHealth);
	void OnTargeting();
	void OnUnTargeting();
	
	bool CanBeExecuted() const;
	void Executed();
	

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnCombatStateChanged OnCombatStateChanged;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnHitReactionStateChanged OnHitReactionStateChanged;

protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& HitResult) override;

	// Combat - Consider to apply Component Design
	UFUNCTION(BlueprintCallable)
	virtual bool NormalAttack();

	virtual bool ExecuteNormalAttack();
	virtual bool CanAttack() const;
	
	UFUNCTION(BlueprintCallable)
	virtual bool Guard();

	virtual bool CanGuard() const;

	bool IsAttackFromFront(const FHitResult& HitResult) const;


	// AI
	UPROPERTY(EditAnywhere, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	TMap<EASRDamageType, FDamageTypeMapping> DamageTypeMappings;

	UFUNCTION(BlueprintCallable)
	virtual void ResetState();

	UFUNCTION(BlueprintCallable)
	virtual void HandleDeath();

	// AI EQS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	float AttackDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	float DefendDistance = 450.f;

	bool bIsWeaponHidden = true;



private:
	

	// Widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* InfoWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> InfoWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UEnemyInfoWidget* InfoWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* LockOnWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> LockOnWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UEnemyLockOnWidget* LockOnWidget;


	// Animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StandUpMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StandingDeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FallingDeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ExecutionMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SmashExecutionMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> NormalAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CombatReadyMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CombatEndMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GuardMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GuardRevengeMontage;


	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// Character State
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	EHitReactionState HitReactionState;



	// TimeLine
	UPROPERTY(VisibleAnywhere, Category = Timeline)
	class UTimelineComponent* TimelineComponent;

	UFUNCTION()
	void HandleTimelineUpdate(float Value);

	UFUNCTION()
	void HandleTimelineFinished();

	UPROPERTY(EditAnywhere, Category = Timeline)
	UCurveFloat* KnockbackCurve;

	UPROPERTY(EditAnywhere, Category = Timeline)
	UCurveFloat* LevitateCurve;

	UPROPERTY(EditAnywhere, Category = Timeline)
	UCurveFloat* AirSmashCurve;

	void InitializeTimeline();
	void StopTimeline();
	float KnockbackDistance = 50.f;
	float BaseKnockbackDistance = 50.f;

	float AirSmashDistance = 50.f;

	FVector StartLocation;
	void ApplyKnockback();
	void Levitate();
	float LevitateHeight = 500.f;
	bool bIsLevitating = false;
	bool bIsAirSmash = false;
	void AerialKnockdown();


	// Hit Postprocess func
	void RotateToAttacker(AActor* Attacker, bool bIsRunFromAttacker);
	void StepBackFromAttacker(AActor* Attacker, float Distance);
	void HandleHitTransform(AActor* Attacker, EASRDamageType DamageType, float Damage);
	void AerialHitAnimMapping(AActor* Attacker, FDamageTypeMapping* Mapping, EASRDamageType DamageType);

	void DisableCollision();


	
	UFUNCTION(BlueprintCallable)
	void SphereTrace(float TraceDistance, float TraceRadius, const FHitData& HitData, ECollisionChannel CollisionChannel, bool bDrawDebugTrace);

	TArray<AActor*> HitActors;
	int32 NormalAttackIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<class AMeleeWeapon> MeleeWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AMeleeWeapon* MeleeWeapon;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	FName WeaponSocketName;

	FTimerHandle HitStopTimerHandle;

	void ApplyHitStop(float Duration, float TimeDilation);
	void ResetTimeDilation();

	UPROPERTY(EditDefaultsOnly, Category=HitStop)
	float HitStopDuration;

	UPROPERTY(EditDefaultsOnly, Category = HitStop)
	float HitStopTimeDilation;

	//AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class APatrolRoute* PatrolRoute;



public:
	void SetCombatState(ECombatState InCombatState);

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	FORCEINLINE float GetAttackDistance() const { return AttackDistance; }
	FORCEINLINE float GetDefendDistance() const { return DefendDistance; }
	FORCEINLINE AMeleeWeapon* GetMeleeWeapon() const { return MeleeWeapon; }





	


};
