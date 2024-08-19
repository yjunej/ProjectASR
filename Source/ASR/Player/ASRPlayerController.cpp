// Fill out your copyright notice in the Description page of Project Settings.


#include "ASRPlayerController.h"

#include "ASR/Character/ASRCharacter.h"
#include "ASR/HUD/RangerHUD.h"
#include "ASR/HUD/CharacterOverlay.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AASRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (RangerHUDClass != nullptr)
	{
		RangerHUD = CreateWidget<URangerHUD>(GetWorld(), RangerHUDClass);
		RangerHUD->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		RangerHUD->AddToViewport();
	}

	ACharacter* PlayCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (PlayCharacter != nullptr)
	{
		ControlCharacter = Cast<AASRCharacter>(PlayCharacter);
	}
}

void AASRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();


	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (SwapAction != nullptr)
		{
			EnhancedInputComponent->BindAction(SwapAction, ETriggerEvent::Triggered, this, &AASRPlayerController::SwapCharacter);
			EnhancedInputComponent->BindAction(RestoreAction, ETriggerEvent::Triggered, this, &AASRPlayerController::RestoreOriginalCharacter);

		}
	}
}

void AASRPlayerController::SetKillScore(float KillScore)
{
	if (RangerHUD != nullptr && RangerHUD->CharacterOverlay != nullptr)
	{
		FString KillScoreString = FString::Printf(TEXT("%d"), FMath::FloorToInt(KillScore));
		RangerHUD->CharacterOverlay->KillScoreText->SetText(FText::FromString(KillScoreString));
	}
}

void AASRPlayerController::SetRangerAmmo(int32 Ammo)
{
	if (RangerHUD != nullptr && RangerHUD->CharacterOverlay != nullptr)
	{
		FString AmmoString = FString::Printf(TEXT("%d"),Ammo);
		RangerHUD->CharacterOverlay->AmmoAmountText->SetText(FText::FromString(AmmoString));
	}
}

void AASRPlayerController::SwapCharacter()
{

	if (ControlCharacter && NewCharacterClass)
	{
		FVector Location = ControlCharacter->GetActorLocation();
		FRotator Rotation = ControlCharacter->GetActorRotation();

		// Save the current character as the original character
		if (!OriginalCharacter)
		{
			OriginalCharacter = ControlCharacter;
		}

		// Define a new location for the new character to avoid overlap
		FVector NewLocation = Location + FVector(0.0f, 0.0f, 0.0f); // Example offset

		// Spawn the new character
		if (!bSpawned)
		{
			NextCharacter = GetWorld()->SpawnActor<AASRCharacter>(NewCharacterClass, NewLocation, Rotation);
			bSpawned = true;
			Possess(NextCharacter);
			Swap(NextCharacter, ControlCharacter);
		}
		else
		{
			Possess(NextCharacter);
			Swap(NextCharacter, ControlCharacter);

		}
	}
}

void AASRPlayerController::RestoreOriginalCharacter()
{
	if (OriginalCharacter && ControlCharacter)
	{
		FVector Location = ControlCharacter->GetActorLocation();
		FRotator Rotation = ControlCharacter->GetActorRotation();

		// Define a new location for the original character to avoid overlap
		FVector NewLocation = Location + FVector(0.0f, 0.0f, 0.0f); // Example offset

		// Spawn the original character
		AASRCharacter* NewOriginalCharacter = GetWorld()->SpawnActor<AASRCharacter>(OriginalCharacterClass, NewLocation, Rotation);
		if (NewOriginalCharacter)
		{
			// Possess the original character
			Possess(NewOriginalCharacter);

			// Destroy the current character
			ControlCharacter->Destroy();
			ControlCharacter = NewOriginalCharacter;

			// Clear the original character reference
			OriginalCharacter = nullptr;
		}
	}
}

