// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Drone.generated.h"

UCLASS()
class ASR_API ADrone : public AActor
{
	GENERATED_BODY()
	
public:	
	ADrone();
	
	virtual void Tick(float DeltaTime) override;
	void Fire(FVector FirePoint);
	void SetDroneOwnerGunner(class AGunner* Gunner);
	FTimerHandle DestroyTimerHandle;
	void SelfDestruct();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector FollowRelativePosition;

protected:
	virtual void BeginPlay() override;

private:
	class AGunner* DroneOwner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* MeshComponent;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float FireSpeed;

	float FollowSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<class AProjectile> ProjectileClass;



public:	

};
