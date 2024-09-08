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

void AASRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GunenrHUDClass != nullptr)
	{
		GunnerHUD = CreateWidget<URangerHUD>(GetWorld(), GunenrHUDClass);
		GunnerHUD->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		GunnerHUD->AddToViewport();
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
	if (GunnerHUD != nullptr && GunnerHUD->CharacterOverlay != nullptr)
	{
		FString KillScoreString = FString::Printf(TEXT("%d"), FMath::FloorToInt(KillScore));
		GunnerHUD->CharacterOverlay->KillScoreText->SetText(FText::FromString(KillScoreString));
	}
}

void AASRPlayerController::SetRangerAmmo(float AmmoPercent)
{
	if (GunnerHUD != nullptr && GunnerHUD->AmmoProgressBar != nullptr)
	{
		GunnerHUD->AmmoProgressBar->SetPercent(AmmoPercent);
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
			// Sync
			UClass* LoadedClass = NewCharacterClass.LoadSynchronous();
			if (LoadedClass != nullptr)
			{
				NextCharacter = GetWorld()->SpawnActor<AASRCharacter>(LoadedClass, NewLocation, Rotation);
				if (NextCharacter != nullptr)
				{

					bSpawned = true;
					Possess(NextCharacter);
					ESlateVisibility GunnerHUDTemp = Cast<AGunner>(NextCharacter) == nullptr ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible;
					GunnerHUD->SetVisibility(GunnerHUDTemp);
					Swap(NextCharacter, ControlCharacter);
				}
			}

			// Async
			//FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			//StreamableManager.RequestAsyncLoad(NewCharacterClass.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([this, NewLocation, Rotation]()
			//	{
			//		UClass* LoadedClass = NewCharacterClass.Get();
			//		if (LoadedClass != nullptr)
			//		{
			//			NextCharacter = GetWorld()->SpawnActor<AASRCharacter>(LoadedClass, NewLocation, Rotation);

			//			//NextCharacter = GetWorld()->SpawnActor<AASRCharacter>(NewCharacterClass.Get(), NewLocation, Rotation);

			//			bSpawned = true;
			//			Possess(NextCharacter);

			//			// TODO - UI Switching
			//			ESlateVisibility GunnerHUDTemp = Cast<AGunner>(NextCharacter) == nullptr ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible;
			//			GunnerHUD->SetVisibility(GunnerHUDTemp);
			//			Swap(NextCharacter, ControlCharacter);
			//		}
			//	}
			//));



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

		// Hard Reference
		//AASRCharacter* NewOriginalCharacter = GetWorld()->SpawnActor<AASRCharacter>(OriginalCharacterClass.Get(), NewLocation, Rotation);
		
		// Sync
		UClass* LoadedClass = OriginalCharacterClass.LoadSynchronous();
		if (LoadedClass != nullptr)
		{
			AASRCharacter* NewOriginalCharacter = GetWorld()->SpawnActor<AASRCharacter>(LoadedClass, NewLocation, Rotation);
			if (NewOriginalCharacter != nullptr)
			{
				Possess(NewOriginalCharacter);
				bSpawned = true;
				ESlateVisibility GunnerHUDTemp = Cast<AGunner>(NextCharacter) == nullptr ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible;
				GunnerHUD->SetVisibility(GunnerHUDTemp);
				Swap(NextCharacter, ControlCharacter);
			}
		}


		//ASYNC 
		//AASRCharacter* NewOriginalCharacter = nullptr;
		//FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		//StreamableManager.RequestAsyncLoad(OriginalCharacterClass.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([this, &NewOriginalCharacter, NewLocation, Rotation]()
		//	{
		//		UClass* LoadedClass = OriginalCharacterClass.Get();
		//		if (LoadedClass != nullptr)
		//		{
		//			NewOriginalCharacter = GetWorld()->SpawnActor<AASRCharacter>(LoadedClass, NewLocation, Rotation);
		//			if (NewOriginalCharacter)
		//			{
		//				// Possess the original character
		//				Possess(NewOriginalCharacter);

		//				// Destroy the current character
		//				ControlCharacter->Destroy();
		//				ControlCharacter = NewOriginalCharacter;

		//				// Clear the original character reference
		//				OriginalCharacter = nullptr;
		//			}
		//		}
		//	}
		//));

		//AASRCharacter* NewOriginalCharacter = GetWorld()->SpawnActor<AASRCharacter>(OriginalCharacterClass.Get(), NewLocation, Rotation);


	}
}

