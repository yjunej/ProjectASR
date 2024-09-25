// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Field/FieldSystemActor.h"
#include "BaseFieldSystem.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API ABaseFieldSystem : public AFieldSystemActor
{
	GENERATED_BODY()



public:
	ABaseFieldSystem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class URadialFalloff* RadialFalloff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class URadialVector* RadialVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UFieldSystemMetaData* FieldSystemMetaData;


protected:
	virtual void BeginPlay() override;


};
