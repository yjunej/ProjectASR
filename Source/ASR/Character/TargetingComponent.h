// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASR_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetingComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	friend class AASRCharacter;
	friend class AASRPlayerController;



	bool FindSubTarget();
	void ClearSubTarget();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	FTransform GetTargetTransform();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	FTransform GetLastSubTargetTransform();

	void ClearTarget();

protected:
	virtual void BeginPlay() override;

private:
	class AASRCharacter* Owner;
	AActor* TargetActor;
	AActor* SubTargetActor;


	void FindTarget();
	void FindNearestTarget();
	void LockOnTarget(const FHitResult& HitResult);
	void PlaceDecalActor();
	AActor* LastSubTargetActor;




	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	bool bIsTargeting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float ClearTargetDistance = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float ClearSubTargetDistance = 600.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float TargetingDistance = 1200.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float TargetRadius = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float SubTargetingDistance = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float SubTargetingRadius = 100.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float CameraRotationSpeed = 6.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	FVector CameraOffset = { 0.f ,0.f, 0.f };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float CameraStartHeightOffset = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float CameraMinPitch = -90.f;

	
	// TODO - Write Comprehensive Motion Warping Control Function
	UPROPERTY(EditAnywhere, Category = NormalCombat, meta = (AllowPrivateAccess="true"))
	UMaterialInterface* DecalMaterial;

public:
	FORCEINLINE bool IsTargeting() const { return bIsTargeting; }
	FORCEINLINE AActor* GetTargetActor() const { return TargetActor; }
	FORCEINLINE AActor* GetSubTargetActor() const { return SubTargetActor; }
	FORCEINLINE AActor* GetLastSubTargetActor() const { return LastSubTargetActor; }

	

};
