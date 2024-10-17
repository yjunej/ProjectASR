// Fill out your copyright notice in the Description page of Project Settings.


#include "ASRMainHUD.h"

#include "ASR/Character/ASRCharacter.h"
#include "Components/ProgressBar.h"

void UASRMainHUD::NativeConstruct()
{
	Super::NativeConstruct();
	if (Owner != nullptr)
	{
		Owner->OnHealthChanged.AddDynamic(this, &UASRMainHUD::UpdateHealthBar);
		Owner->OnStaminaChanged.AddDynamic(this, &UASRMainHUD::UpdateStaminaBar);
		PostHealth = Owner->GetHealth();
		PostStamina = Owner->GetStamina();
	}

}

void UASRMainHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePostHealthBar();
	UpdatePostStaminaBar();

}

void UASRMainHUD::UpdateHealthBar()
{
	if (Owner != nullptr)
	{
		if (Owner->GetMaxHealth() > 0)
		{
			HealthBar->SetPercent(Owner->GetHealth() / Owner->GetMaxHealth());
		}

	}
}

void UASRMainHUD::UpdateStaminaBar()
{
	if (Owner != nullptr)
	{
		if (Owner->GetMaxStamina() > 0)
		{
			StaminaBar->SetPercent(Owner->GetStamina() / Owner->GetMaxStamina());
			if (Owner->GetStamina() <= 0.f)
			{
				StaminaBar->WidgetStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor::Red);
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UASRMainHUD::ResetGuardBrokenColorChange, 1.0f, false);


			}
		}

	}
}

void UASRMainHUD::UpdatePostHealthBar()
{
	if (Owner != nullptr)
	{
		if (Owner->GetMaxHealth() > 0)
		{
			PostHealth = FMath::FInterpTo(PostHealth, Owner->GetHealth(), GetWorld()->GetDeltaSeconds(), PostBarLerpSpeed);
			PostHealthBar->SetPercent(PostHealth / Owner->GetMaxHealth());
		}
	}
}

void UASRMainHUD::UpdatePostStaminaBar()
{
	if (Owner != nullptr)
	{
		if (Owner->GetMaxStamina() > 0)
		{
			PostStamina = FMath::FInterpTo(PostStamina, Owner->GetStamina(), GetWorld()->GetDeltaSeconds(), PostBarLerpSpeed);
			PostStaminaBar->SetPercent(PostStamina / Owner->GetMaxStamina());
		}
	}
}

void UASRMainHUD::ResetGuardBrokenColorChange()
{
	if (Owner != nullptr)
	{
		StaminaBar->WidgetStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor::White);
	}
}
