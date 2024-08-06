// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterInfoWidget.h"

#include "Components/TextBlock.h"

void UCharacterInfoWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}

void UCharacterInfoWidget::SetDisplayText(FString InDisplayText)
{
	if (DisplayText != nullptr)
	{
		DisplayText->SetText(FText::FromString(InDisplayText));
	}
}

void UCharacterInfoWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString LocalRoleString;

	switch (LocalRole)
	{
	case ENetRole::ROLE_Authority:
		LocalRoleString = FString("ROLE_Authority");
		break;

	case ENetRole::ROLE_AutonomousProxy:
		LocalRoleString = FString("ROLE_AutonomousProxy");
		break;

	case ENetRole::ROLE_SimulatedProxy:
		LocalRoleString = FString("ROLE_SimulatedProxy");
		break;

	case ENetRole::ROLE_None:
		LocalRoleString = FString("ROLE_None");
		break;

	default:
		LocalRoleString = FString("ROLE_Default");
	}

	SetDisplayText(LocalRoleString);
}
