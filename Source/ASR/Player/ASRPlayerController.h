// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ASRPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class ASR_API AASRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public: 
	void SetKillScore(float KillScore);
	void SetRangerAmmo(float AmmoPercent);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GunenrHUDClass;

	class URangerHUD* GunnerHUD;


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RestoreAction;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<class AASRCharacter> NewCharacterClass;


	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<AASRCharacter> OriginalCharacterClass;

	UPROPERTY(EditDefaultsOnly)
	AASRCharacter* ControlCharacter;

	UPROPERTY()
	AASRCharacter* NextCharacter;

	UPROPERTY()
	AASRCharacter* OriginalCharacter;

	UFUNCTION(BlueprintCallable)
	void SwapCharacter();

	UFUNCTION(BlueprintCallable)
	void RestoreOriginalCharacter();

	bool bSpawned = false;
};
