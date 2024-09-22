// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASR/Interfaces/CombatInterface.h"
#include "ASR/Interfaces/EnemyAIInterface.h"
#include "ASR/Enums/ASRDamageType.h"
#include "ASR/Character/ASRCharacter.h" // Refactoring After Complete Basic System

#include "BaseEnemy.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackEnd, AActor*, AttackTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGuardEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitReactionStateChanged, EHitReactionState, NewState);

// To Enable TArray in TMap
USTRUCT(BlueprintType)
struct FAIAttackMontages
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animation)
	TArray<UAnimMontage*> AIAttackMontages;
};

UCLASS()
class ASR_API ABaseEnemy : public ACharacter, public ICombatInterface, public IEnemyAIInterface
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// CombatInterface
	virtual bool GetHit(const FHitResult& HitResult, AActor* Attacker, const FHitData& HitData) override;
	virtual bool IsDead() const override;
	virtual ECombatState GetCombatState() const override;
	virtual EHitReactionState GetHitReactionState() const override;
	virtual void SetHitReactionState(EHitReactionState NewState) override;
	virtual bool ReserveAttackTokens(int32 Amount) override;
	virtual void ReturnAttackTokens(int32 Amount) override;
	virtual UDataTable* GetAttackDataTable() const override;
	virtual void SphereTrace(float TraceDistance, float TraceRadius, const FHitData& HitData, ECollisionChannel CollisionChannel, bool bDrawDebugTrace) override;


	// EnemyAIInterface - Blueprint Compatible
	virtual APatrolRoute* GetPatrolRoute() const override;
	virtual float GetMovementSpeed() const override;
	virtual float SetMovementSpeed(EEnemyMovementSpeed EnemyMovementSpeed) override;
	virtual float GetCurrentHealth() const override;
	virtual float GetMaxHealth() const override;
	virtual bool AIReserveAttackTokens(AActor* AttackTarget, int32 RequiredTokens) override;
	virtual void AIReturnAttackTokens(AActor* AttackTarget) override;
	virtual void StoreAttackTokens(AActor* AttackTarget, int32 Amount) override;
	virtual bool AIAttack(AActor* AttackTarget, EAIAttack AIAttackType) override;
	//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float Health = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float MaxHealth = 1000.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float Stamina = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float MaxStamina = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float ExecutionThresholdHealth = 200.f;

	//
	UPROPERTY(BlueprintAssignable, Category= AI)
	FOnAttackEnd OnAttackEnd;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	bool bIsCombatReady = false;


	// Custom Notify For BT
	void NotifyAttackEnd(AActor* AttackTarget);

	FOnHealthChanged OnHealthChanged;
	FOnStaminaChanged OnStaminaChanged;


	void SetHealth(float NewHealth);
	void SetStamina(float NewStamina);

	void OnTargeting();
	void OnUnTargeting();
	
	bool CanBeExecuted() const;
	void Executed();
	

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnCombatStateChanged OnCombatStateChanged;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnHitReactionStateChanged OnHitReactionStateChanged;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);


	// TODO - Move To Utils Func, Ensure SoftObjectPtr Asset Loaded 
	template <typename AssetType>
	AssetType* EnsureAssetLoaded(TSoftObjectPtr<AssetType> const& AssetPtr)
	{
		AssetType* Asset;
		if (AssetPtr.IsValid())
		{
			return AssetPtr.Get();
		}
		return AssetPtr.LoadSynchronous();
	}

protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& HitResult) override;

	class UEnemyInfoWidget* InfoWidget;

	// Combat - Consider to apply Component Design

	virtual bool CanAttack() const;
	
	UFUNCTION(BlueprintCallable)
	virtual bool Guard(float GuardProb);

	virtual bool CanGuard() const;


	virtual bool ExecuteAIAttack(AActor* AttackTarget, EAIAttack AIAttackType);


	bool IsAttackFromFront(const FHitResult& HitResult) const;


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
	UAnimMontage* CombatReadyMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CombatEndMontage;


	// Guard System
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GuardMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GuardRevengeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GuardBrokenMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GuardHitMontage;


	UPROPERTY(EditAnywhere, Category = Animation)
	TMap<EAIAttack, FAIAttackMontages> AttackMontageMap;

	// AI
	UPROPERTY(EditAnywhere, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	UFUNCTION(BlueprintCallable)
	virtual void ResetState();

	UFUNCTION(BlueprintCallable)
	virtual void HandleDeath();

	// AI EQS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	float AttackDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	float DefendDistance = 450.f;

	bool bIsWeaponHidden = false;


private:
	

	// Widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* InfoWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> InfoWidgetClass;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* LockOnWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> LockOnWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UEnemyLockOnWidget* LockOnWidget;


	
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

	TArray<AActor*> HitActors;
	int32 NormalAttackIndex = 0;

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<class AMeleeWeapon> MeleeWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AMeleeWeapon* MeleeWeapon;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	FName WeaponArmedSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	FName WeaponUnarmedSocketName;

	FTimerHandle HitStopTimerHandle;

	void ApplyHitStop(float Duration, float TimeDilation);
	void ResetTimeDilation();

	UPROPERTY(EditAnywhere, Category=HitStop)
	float HitStopDuration;

	UPROPERTY(EditAnywhere, Category = HitStop)
	float HitStopTimeDilation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	UDataTable* AttackDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	UDataTable* DamageDataTable;

	FDamageTypeMapping* FindDamageDTRow(EASRDamageType DamageType) const;

	void PlayRandomSection(UAnimMontage* Montage);

	//AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class APatrolRoute* PatrolRoute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 AttackTokensCount;

	AActor* CachedAttackTarget;
	float CachedLastUsedTokensCount;
	TMap<AActor*, int32> ReservedAttackTokensMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float AutoGuardRate = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ReactStateGuardRate = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float GuardRevengeRate = 0.5;

	void ApplyGuardKnockback(float Damage);


public:
	void SetCombatState(ECombatState InCombatState);

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	FORCEINLINE float GetAttackDistance() const { return AttackDistance; }
	FORCEINLINE float GetDefendDistance() const { return DefendDistance; }
	FORCEINLINE AMeleeWeapon* GetMeleeWeapon() const { return MeleeWeapon; }
	FORCEINLINE void SetAutoGuardRate(float Rate) { AutoGuardRate = Rate; }
	FORCEINLINE UWidgetComponent* GetInfoWidgetComponent() const { return InfoWidgetComponent; }
	FORCEINLINE void SetCachedAttackTarget(AActor* NewAttackTarget) { CachedAttackTarget = NewAttackTarget; }
};