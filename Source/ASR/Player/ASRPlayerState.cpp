// Fill out your copyright notice in the Description page of Project Settings.


#include "ASRPlayerState.h"

#include "ASR/Character/Ranger.h"
#include "ASR/Player/ASRPlayerController.h"

void AASRPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Ranger = Ranger == nullptr ? Cast<ARanger>(GetPawn()) : Ranger;
	PlayerController = PlayerController == nullptr ? Cast<AASRPlayerController>(Ranger->Controller) : PlayerController;


	if (Ranger != nullptr && PlayerController != nullptr)
	{
		PlayerController->SetKillScore(GetScore());
	}
}

void AASRPlayerState::AddScore(float InScore)
{
	SetScore(GetScore() + InScore);

	Ranger = Ranger == nullptr ? Cast<ARanger>(GetPawn()) : Ranger;
	PlayerController = PlayerController == nullptr ? Cast<AASRPlayerController>(Ranger->Controller) : PlayerController;


	if (Ranger != nullptr && PlayerController != nullptr)
	{
		PlayerController->SetKillScore(InScore);
	}
}
