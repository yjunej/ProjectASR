// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeleeWeapon.generated.h"

UCLASS()
class ASR_API AMeleeWeapon : public AActor
{
	GENERATED_BODY()
	
	// TODO - Change Name Or Create Base class! (Decide Not to Use Weapon mesh based Hit System)

public:	
	AMeleeWeapon();

	virtual void Tick(float DeltaTime) override;
	
	void WeaponBoxTrace(FHitResult& WeaponBoxHitResult);

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> IgnoreActors;;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly)
	bool bShowBoxDebug = true;

	AActor* WeaponOwner;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceEnd;

	UPROPERTY(EditAnywhere)
	FVector TraceExtent = FVector(10.f);

public:
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }


};
