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

protected:
	virtual void BeginPlay() override;

private:
	class AASRCharacter* Owner;
	AActor* TargetActor;


	void FindTarget();
	void ClearTarget();
	void PlaceDecalActor();



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	bool bIsTargeting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float ClearTargetDistance = 1000.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float TargetingDistance = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	float CameraRotationSpeed = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	FVector CameraOffset;
	
	// TODO - Write Comprehensive Motion Warping Control Function
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = NormalCombat, meta = (AllowPrivateAccess = "true"))
	FTransform GetTargetTransform();

	UPROPERTY(EditAnywhere, Category = NormalCombat, meta = (AllowPrivateAccess="true"))
	UMaterialInterface* DecalMaterial;
};
