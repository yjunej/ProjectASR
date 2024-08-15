// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASR/Interfaces/HitInterface.h"
#include "ASR/Enums/ASRDamageType.h"
#include "ASR/Character/ASRCharacter.h" // Refactoring After Complete Basic System

#include "BaseEnemy.generated.h"


UCLASS()
class ASR_API ABaseEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// HitInterface
	virtual void GetHit(const FHitResult& HitResult, AActor* Attacker, float Damage, EASRDamageType DamageType) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float MaxHealth = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float ExecutionThresholdHealth = 200.f;


	FOnHealthChanged OnHealthChanged;

	void SetHealth(float NewHealth);
	void OnTargeting();
	void OnUnTargeting();
	
	bool CanBeExecuted() const;
	void Executed();


protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& HitResult) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	EASRCharacterState CharacterState;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	TMap<EASRDamageType, FDamageTypeMapping> DamageTypeMappings;

	UFUNCTION(BlueprintCallable)
	virtual void ResetState();

	UFUNCTION(BlueprintCallable)
	virtual void HandleDeath();

private:
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StandUpMontage;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StandingDeathMontage;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FallingDeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ExecutionMontage;

	// TimeLine
	UPROPERTY(VisibleAnywhere, Category = "Timeline")
	class UTimelineComponent* TimelineComponent;

	UFUNCTION()
	void HandleTimelineUpdate(float Value);

	UFUNCTION()
	void HandleTimelineFinished();

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* KnockbackCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* LevitateCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* AirSmashCurve;


	void InitializeTimeline();
	void StopTimeline();
	float KnockbackDistance = 50.f;
	float AirSmashDistance = 400.f;

	FVector StartLocation;
	void ApplyKnockback();
	void Levitate();
	float LevitateHeight = 500.f;
	bool bIsLevitating = false;
	bool bIsAirSmash = false;
	void AerialKnockdown();

	// Hit Postprocess func
	void RotateToAttacker(AActor* Attacker);
	void StepBackFromAttacker(AActor* Attacker, float Distance);


	void HandleHitTransform(AActor* Attacker, EASRDamageType DamageType);
	void AerialHitAnimMapping(AActor* Attacker, FDamageTypeMapping* Mapping, EASRDamageType DamageType);

	void DisableCollision();


public:
	FORCEINLINE EASRCharacterState GetCharacterState() const { return CharacterState; };
	void SetCharacterState(EASRCharacterState InCharacterState);


	


};
