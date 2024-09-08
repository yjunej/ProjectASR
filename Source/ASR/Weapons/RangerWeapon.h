// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RangerWeapon.generated.h"

UENUM(BlueprintType)
enum class ERangerWeaponState : uint8
{
	EWS_Initialized UMETA(DisplayName = "Initialized"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Unequipped UMETA(DisplayName = "UnEquipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(Hidden)
};

UCLASS()
class ASR_API ARangerWeapon : public AActor
{
	GENERATED_BODY()

public:
	ARangerWeapon();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetInfoWidgetVisibility(bool bVisible);
	virtual void Fire(const FVector& HitTarget);
	void UseAmmo();
	void SetHUDAmmo();


protected:
	virtual void BeginPlay() override;

	virtual void OnRep_Owner() override;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:
	UFUNCTION()
	void OnRep_WeaponState();


private:
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class USphereComponent* CollisionSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = Weapon)
	ERangerWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class UWidgetComponent* WeaponInfoWidget;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<class ABulletCase> CaseClass;

	UPROPERTY(EditAnywhere, Category = UI)
	class URangerHUD* GunnerHUD;

	// Zoom FOV
	UPROPERTY(EditAnywhere)
	float ZoomFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = GunFire)
	float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, Category = GunFire)
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere, Category = GunFire)
	int32 Ammo;

	UPROPERTY(EditAnywhere, Category = GunFire)
	int32 MagazineCapacity;

	UPROPERTY()
	class ARanger* OwnerRanger;

	UPROPERTY()
	class AASRPlayerController* PlayerController;


public:
	bool IsOutOfAmmo() const;
	// Getter & Setter
	void SetWeaponState(ERangerWeaponState State);

	// INLINE
	FORCEINLINE ERangerWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE USphereComponent* GetCollisionSphere() const { return CollisionSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return Mesh; }
	FORCEINLINE float GetZoomFOV() const { return ZoomFOV; }
	FORCEINLINE float GetZoomSpeed() const { return ZoomSpeed; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE bool IsAutomatic() const { return bAutomatic; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE void SetAmmo(int32 InAmmo) { Ammo = InAmmo; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }



};
