// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ASRPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API AASRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public: 
	void SetKillScore(float KillScore);
	void SetRangerAmmo(int32 Ammo);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> RangerHUDClass;

	class URangerHUD* RangerHUD;

};
