// Fill out your copyright notice in the Description page of Project Settings.


#include "ASRPlayerController.h"

#include "ASR/Character/ASRCharacter.h"
#include "ASR/HUD/RangerHUD.h"
#include "ASR/HUD/CharacterOverlay.h"
#include "ASR/Character/Gunner.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "ASR/Character/TargetingComponent.h"

AASRPlayerController::AASRPlayerController()
{
	bReplicates = true;
}

void AASRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* PlayCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	check(PlayCharacter);

	ControlCharacter = Cast<AASRCharacter>(PlayCharacter);
	check(DefaultMappingContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);

	Subsystem->AddMappingContext(DefaultMappingContext, 0);
}

void AASRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AASRPlayerController::Input_Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AASRPlayerController::Input_Look);
	EnhancedInputComponent->BindAction(ToggleLockOnAction, ETriggerEvent::Triggered, this, &AASRPlayerController::Input_ToggleLockOn);
}

void AASRPlayerController::Input_Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();
	LastInput = MoveVector;

	// Guard Accept, Gunner Flinching Not use Root motion (Knockback by launch..)
	ECombatState CombatState = ControlCharacter->GetCombatState();
	if (CombatState == ECombatState::ECS_Flinching || 
		CombatState == ECombatState::ECS_KnockDown || CombatState == ECombatState::ECS_Death)
	{
		return;
	}


	const FRotator YawRotation(0, GetControlRotation().Yaw, 0);
	const FVector ForwardDirectionVector(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
	const FVector RightDirectionVector(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

	check(ControlCharacter);
	ControlCharacter->AddMovementInput(ForwardDirectionVector, MoveVector.Y);
	ControlCharacter->AddMovementInput(RightDirectionVector, MoveVector.X);
	
}

void AASRPlayerController::Input_Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	check(ControlCharacter);
	if (ControlCharacter->GetTargetingComponent() == nullptr || !(ControlCharacter->GetTargetingComponent()->bIsTargeting))
	{
		if (!ControlCharacter->bIsExecuting)
		{
			ControlCharacter->AddControllerYawInput(LookVector.X);
			ControlCharacter->AddControllerPitchInput(LookVector.Y);
		}
	}
}

void AASRPlayerController::Input_ToggleLockOn(const FInputActionValue& Value)
{
	check(ControlCharacter);
	if (UTargetingComponent* TargetingComp = ControlCharacter->GetTargetingComponent())
	{
		if (TargetingComp->bIsTargeting)
		{
			TargetingComp->ClearTarget();	
		}
		else
		{
			TargetingComp->FindTarget();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL TargetingComponent"));
	}
}
