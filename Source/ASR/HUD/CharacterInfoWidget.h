// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API UCharacterInfoWidget : public UUserWidget
{
	GENERATED_BODY()
		
protected:
	virtual void NativeDestruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	
	void SetDisplayText(FString InDisplayText);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);  
};
