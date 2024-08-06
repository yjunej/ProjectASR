// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ASRPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API AASRPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void OnRep_Score() override;
	void AddScore(float InScore);

private:
	class ARanger* Ranger;
	class AASRPlayerController* PlayerController;
};
