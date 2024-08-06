// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RangerBattleComponent.generated.h"

class ARangerWeapon;



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASR_API URangerBattleComponent : public UActorComponent
{
	GENERATED_BODY()
	 
public:	
	// Sets default values for this component's properties
	URangerBattleComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; 

	void EquipWeapon(ARangerWeapon* Weapon);
	void Fire(bool bFire);

	void FireWithTimer();

	friend class ARanger;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	class ARanger* Ranger;
	class AASRPlayerController* PlayerController;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	ARangerWeapon* EquippedWeapon; 

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UPROPERTY(Replicated)
	bool bIsAiming = false;

	bool bStartFire = false;

	float SpreadRate;

public:	
	

	// Fire

	void SetAiming(bool IsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool IsAiming);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
	
	void TraceCrosshairs(FHitResult& HitResult);

	void Reload();

	UFUNCTION(BlueprintCallable)
	void ReloadFinished();

	// HUD

	void SetHUDCrosshair(float DeltaSeconds);

	float CrosshairInAirCoef;
	float CrosshairCrouchCoef;
	float CrosshairAimCoef;

	FVector HitPoint;

	// Zoom
	float DefaultFOV;
	float FOV;

	// FPS Zoom

	UPROPERTY(EditAnywhere, Category = Zoom)
	float ZoomArmLength = 80.f;


	UPROPERTY(EditAnywhere, Category =Zoom)
	FVector ZoomArmSocketOffset= { 0.f, 60.f, 40.f };

	UPROPERTY(EditAnywhere, Category = Zoom)
	float CrouchArmHeightOffset = -50.f;


	UPROPERTY(EditAnywhere, Category=Zoom)
	float ZoomFOV = 30.f;

	UPROPERTY(EditAnywhere, Category=Zoom)
	float ZoomSpeed = 20.f;

	void InterpFOV(float DeltaSeconds);


	UPROPERTY(EditAnywhere)
	float ArmedMaxWalkSpeed = 600.f;

	UPROPERTY(EditAnywhere)
	float ArmedMaxWalkSpeedAiming = 350.f;


	// Automatic Gun
	FTimerHandle FireTimer;
	void StartFireTimer();
	void EndFireTimer();
	
	bool bCanFire = true;
	bool CanFire();



};
