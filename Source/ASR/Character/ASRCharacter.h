// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASR/Interfaces/HitInterface.h"
#include "ASRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);


class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EASRCharacterState : uint8
{
	ECS_None		UMETA(DisplayName = "Default State"),
	ECS_Attack		UMETA(DisplayName = "Attack"),
	ECS_Dodge		UMETA(DisplayName = "Dodge"),
	ECS_Guard		UMETA(DisplayName = "Guard"),
	ECS_Flinching	UMETA(DisplayName = "Flinching"),
	ECS_KnockDown	UMETA(DisplayName = "Knockout"),
	ECS_Death		UMETA(DisplayName = "Death"),

	ECS_MAX			UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FDamageTypeMapping
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
	EASRCharacterState CharacterState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
	UAnimMontage* HitReactionMontage;

};


UCLASS()
class ASR_API AASRCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AASRCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

	virtual void GetHit(const FHitResult& HitResult, AActor* Attacker, const FHitData& HitData) override;

	FOnHealthChanged OnHealthChanged;

	void PlayRandomSection(UAnimMontage* Montage);
	bool IsAttackFromFront(const FHitResult& HitResult) const;
	virtual float GetFirstSkillWarpDistance() const;

	// TODO - Interface 
	virtual void OnAttackEnemy() {};

	void ApplyHitStop(float Duration, float TimeDilation);
	void ResetTimeDilation();

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


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State)
	EASRCharacterState CharacterState;

	TArray<AActor*> HitActors;
	FVector2D PrevInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float Health = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float MaxHealth = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float Stability = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float MaxStability = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float ExecutionDistance = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float NormalAttackWarpDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float DashAttackWarpDistance = 500.f;


	UFUNCTION(BlueprintCallable)
	virtual void ResetState();

	UFUNCTION(BlueprintCallable)
	virtual void SphereTrace(float TraceDistance, float TraceRadius, const FHitData& HitData, ECollisionChannel CollisionChannel, bool bDrawDebugTrace);

	UFUNCTION(BlueprintCallable)
	virtual void ResetCamera();

	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewHealth);

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

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<EASRDamageType, FDamageTypeMapping> DamageTypeMappings;
	

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

	UFUNCTION()
	void OnExecutionMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	class UASRMainHUD* MainHUDWidget;



public:
	// Getter & Setter
	void SetCharacterState(EASRCharacterState InCharacterState);

	// FORCEINLINE 
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCameraComponent* GetExecutionCamera() const { return ExecutionCamera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE EASRCharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
	FORCEINLINE UTargetingComponent* GetTargetingComponent() const { return TargetingComp; }
	FORCEINLINE UChildActorComponent* GetExecutionCameraManager() const { return ExecutionCameraManager; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetExecutionDistance() const { return ExecutionDistance; }
	FORCEINLINE float GetStability() const { return Stability; }
	FORCEINLINE float GetMaxStability() const { return MaxStability; }
	FORCEINLINE float GetDashAttackWarpDistance() const { return DashAttackWarpDistance; }
	FORCEINLINE float GetNormalAttackWarpDistance() const { return NormalAttackWarpDistance; }

	
};
