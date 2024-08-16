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
	ECS_Flinching	UMETA(DisplayName = "Flinching"),
	ECS_Stunned		UMETA(DisplayName = "Stunned"),
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

	virtual void GetHit(const FHitResult& HitResult, AActor* Attacker, float Damage, EASRDamageType DamageType) override;

	FOnHealthChanged OnHealthChanged;

protected:
	virtual void BeginPlay() override;

	// Enhanced Input
	virtual void Input_Move(const FInputActionValue& Value);
	virtual void Input_ToggleLockOn(const FInputActionValue& Value);
	virtual void Input_Execution(const FInputActionValue& Value);

	void Input_Look(const FInputActionValue& Value);
	void Input_ToggleCrouch(const FInputActionValue& Value);


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State)
	EASRCharacterState CharacterState;

	TArray<AActor*> HitActors;
	FVector2D PrevInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float Health = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float MaxHealth = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float ExecutionDistance = 400.f;


	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable)
	virtual void ResetState();

	UFUNCTION(BlueprintCallable)
	virtual void SphereTrace(float End, float Radius, float BaseDamage, EASRDamageType DamageType, ECollisionChannel CollisionChannel, bool bDrawDebugTrace);

	bool CanExecution() const;

	UFUNCTION(BlueprintCallable)
	virtual void ResetCamera();

	bool bIsExecuting = false;
	bool bIsInvulnerable = false;


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




private:
	// Camera Setting
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* FollowCameraManager;


	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ExecutionCamera;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* ExecutionCameraManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MotionWarping, meta = (AllowPrivateAccess = "true"))
	class UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MotionWarping, meta = (AllowPrivateAccess = "true"))
	class UTargetingComponent* TargetingComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<EASRDamageType, FDamageTypeMapping> DamageTypeMappings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	class USoundCue* GetHitSoundCue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	class USoundCue* HitSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> MainHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StandingDeathMontage;


	class UASRMainHUD* MainHUDWidget;

	UFUNCTION(BlueprintCallable)
	virtual void HandleDeath();

public:
	// Getter & Setter
	void SetCharacterState(EASRCharacterState InCharacterState);

	// FORCEINLINE 
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCameraComponent* GetExecutionCamera() const { return ExecutionCamera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE EASRCharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
	FORCEINLINE UTargetingComponent* GetTargetingComponent() const { return TargetingComponent; }
	FORCEINLINE UChildActorComponent* GetExecutionCameraManager() const { return ExecutionCameraManager; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetExecutionDistance() const { return ExecutionDistance; }

	
};
