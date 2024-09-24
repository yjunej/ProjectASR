// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASR/Interfaces/CombatInterface.h"
#include "ASR/Enums/CombatState.h"
#include "ASRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStateChanged, ECombatState, NewState);


class UInputAction;
class UInputMappingContext;
struct FInputActionValue;


USTRUCT(BlueprintType)
struct FDamageTypeMapping
{
	GENERATED_USTRUCT_BODY()

public:
	FDamageTypeMapping()
	: CombatState(ECombatState::ECS_None)
	{}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
	ECombatState CombatState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
	UAnimMontage* HitReactionMontage;

};


USTRUCT(BlueprintType)
struct FDamageInfoData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FDamageInfoData()
		: DamageType(EASRDamageType::EDT_Default)
	{}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
	EASRDamageType DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
	FDamageTypeMapping DamageReaction;
};


UCLASS()
class ASR_API AASRCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AASRCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

	// TODO - Create Combat Component.. 
	// CombatInterface
	virtual bool GetHit(const FHitResult& HitResult, AActor* Attacker, const FHitData& HitData) override;
	void SpawnEffects(const FHitData& HitData, const FHitResult& HitResult);
	virtual bool IsDead() const override;
	virtual ECombatState GetCombatState() const override;
	virtual EHitReactionState GetHitReactionState() const override;
	virtual void SetHitReactionState(EHitReactionState NewState) override;
	virtual bool ReserveAttackTokens(int32 Amount) override;
	virtual void ReturnAttackTokens(int32 Amount) override;
	virtual UDataTable* GetAttackDataTable() const override; 
	virtual void SphereTrace(float TraceDistance, float TraceRadius, const FHitData& HitData, ECollisionChannel CollisionChannel, bool bDrawDebugTrace) override;



	FOnHealthChanged OnHealthChanged;
	FOnStaminaChanged OnStaminaChanged;


	void PlayRandomSection(UAnimMontage* Montage);
	bool IsAttackFromFront(const FHitResult& HitResult) const;
	virtual float GetFirstSkillWarpDistance() const;

	// TODO - Interface 
	virtual void OnAttackEnemy() {};

	void ApplyHitStop(float Duration, float TimeDilation);
	void ResetTimeDilation();

	class UASRMainHUD* MainHUDWidget;

	FOnCombatStateChanged OnCombatStateChanged;

	UPROPERTY(EditAnywhere, Category=Move)
	float MaxWalkSpeed = 700.f;

	UPROPERTY(EditAnywhere, Category = Move)
	float MaxStrafeSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = Move)
	float YawRotationRate = 900.f;


	// Cam Control Test
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	float InitialArmLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	float TargetArmLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	class UTimelineComponent* ArmLengthTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	UCurveFloat* ArmLengthCurve;

	UFUNCTION()
	void UpdateArmLength(float Value);

	UFUNCTION(BlueprintCallable)
	void StartArmLengthChange(UCurveFloat* ArmCurve);
	//


	// TODO - Move To Utils Func, Ensure SoftObjectPtr Asset Loaded 
	template <typename AssetType>
	AssetType* EnsureAssetLoaded(TSoftObjectPtr<AssetType> const& AssetPtr)
	{
		if (AssetPtr.IsValid())
		{
			return AssetPtr.Get();
		}

		return AssetPtr.LoadSynchronous();
	}

protected:
	virtual void BeginPlay() override;
	// Enhanced Input
	virtual void Input_Move(const FInputActionValue& Value);
	virtual void Input_ToggleLockOn(const FInputActionValue& Value);
	virtual void Input_Execution(const FInputActionValue& Value);
	virtual void Input_Guard(const FInputActionValue& Value);
	virtual void Input_Dodge(const FInputActionValue& Value);
	virtual void Input_NormalAttack(const FInputActionValue& Value);


	void Input_Look(const FInputActionValue& Value);
	void Input_ToggleCrouch(const FInputActionValue& Value);
	void Input_Release_Guard(const FInputActionValue& Value);


	virtual bool CanAttack() const;

	virtual void Guard();
	virtual bool CanGuard() const;

	virtual void Dodge();
	virtual bool CanDodge() const;
	virtual void ResetDodge();

	virtual void Execution();

	virtual void ResetNormalAttack();


	void ExecuteNormalAttack(int32 AttackIndex);

	// Handle logic that is fully state-based in the parent class.
	// Use Hook method for character specific skill base logic
	virtual void ResetSkills() {};
	virtual void NormalAttack() {};

	UFUNCTION(BlueprintCallable)
	virtual void ResolveHeavyAttackPending() {};
	//

	UFUNCTION(BlueprintCallable)
	virtual void ResolveLightAttackPending();

	UFUNCTION(BlueprintCallable)
	virtual void ResolveDodgeAndGuardPending();




	TArray<AActor*> HitActors;
	FVector2D PrevInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float Health = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float MaxHealth = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float Stamina = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float MaxStamina = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float StaminaRegenRate = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float ExecutionDistance = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float NormalAttackWarpDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float DashAttackWarpDistance = 500.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 AttackTokensCount;

	UFUNCTION(BlueprintCallable)
	virtual void ResetState();

	UFUNCTION(BlueprintCallable)
	virtual void ResetCamera();

	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable)
	void SetStamina(float NewStamina);

	UFUNCTION(BlueprintCallable)
	void SetInvulnerable(bool InInvulnerable) { bIsInvulnerable = InInvulnerable; }

	UFUNCTION(BlueprintCallable)
	virtual void SetExecutionCamera();

	UFUNCTION(BlueprintCallable)
	virtual void HandleDeath();

	bool CanExecution() const;


	bool bIsExecuting = false;
	bool bIsInvulnerable = false;
	bool bIsGuardPressed = false;
	bool bIsDodgePending = false;
	bool bIsNormalAttackPending = false;
	bool bIsStrafe = false;
	int32 NormalAttackIndex;




private:
	// Enhanced Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleCrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleLockOnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ExecutionAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GuardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* NormalAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> NormalAttackMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	EHitReactionState HitReactionState;

private:
	// Camera Setting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* FollowCameraManager;

	// TODO - Delete Duplicated Camera (Use Child Actor)
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ExecutionCamera;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* ExecutionCameraManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MotionWarping, meta = (AllowPrivateAccess = "true"))
	class UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MotionWarping, meta = (AllowPrivateAccess = "true"))
	class UTargetingComponent* TargetingComp;

	UPROPERTY(EditDefaultsOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> MainHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StandingDeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))

	UAnimMontage* GuardAcceptMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GuardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ExecutionMontage;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ParryCounterMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	UDataTable* AttackDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = "true"))
	UDataTable* DamageDataTable;

	EHitDirection GetHitDirection(const FVector AttackerLocation) const;

	UFUNCTION()
	void OnExecutionMontageEnd(UAnimMontage* Montage, bool bInterrupted);


	FDamageTypeMapping* FindDamageDTRow(EASRDamageType DamageType) const;


	// Stamina
	FTimerHandle StaminaRegenTimerHandle;
	float StaminaRegenInterval = 0.1f;
	void RegenStamina();


	APlayerCameraManager* PlayerCameraManager;

	void PlayHitAnimation(const FHitData& HitData, AActor* Attacker);

	

public:
	// Getter & Setter
	void SetCombatState(ECombatState InCombatState);
	void SetStrafe(bool bEnableStrafe);

	// FORCEINLINE 
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCameraComponent* GetExecutionCamera() const { return ExecutionCamera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
	FORCEINLINE UTargetingComponent* GetTargetingComponent() const { return TargetingComp; }
	FORCEINLINE UChildActorComponent* GetExecutionCameraManager() const { return ExecutionCameraManager; }
	FORCEINLINE UDataTable* GetDamageDataTable() const { return DamageDataTable; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetExecutionDistance() const { return ExecutionDistance; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	FORCEINLINE float GetDashAttackWarpDistance() const { return DashAttackWarpDistance; }
	FORCEINLINE float GetNormalAttackWarpDistance() const { return NormalAttackWarpDistance; }

};
