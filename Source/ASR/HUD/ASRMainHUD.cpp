// Fill out your copyright notice in the Description page of Project Settings.


#include "ASRMainHUD.h"

#include "ASR/Character/ASRCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

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
				//StaminaBar->WidgetStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor::Red);
				FProgressBarStyle StaminaBarStyle = StaminaBar->GetWidgetStyle();
				StaminaBarStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor::Red);
				StaminaBar->SetWidgetStyle(StaminaBarStyle);

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
		FProgressBarStyle StaminaBarStyle = StaminaBar->GetWidgetStyle();
		StaminaBarStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor::White);
		StaminaBar->SetWidgetStyle(StaminaBarStyle);

		//StaminaBar->GetWidgetStyle().BackgroundImage.TintColor = const FSlateColor(FLinearColor::White);
		//StaminaBar->SetWidgetStyle()
	}
}

void UASRMainHUD::AddWidgetToMainCanvas(UUserWidget* Widget)
{
	if (MainCanvas)
	{
		UCanvasPanelSlot* NewSlot = MainCanvas->AddChildToCanvas(Widget);
		if (NewSlot)
		{
			// Full Screen
			NewSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			NewSlot->SetPosition(FVector2D(0.f, 0.f));
			FVector2D ScreenSize = FVector2D(MainCanvas->GetCachedGeometry().GetLocalSize());
			NewSlot->SetSize(ScreenSize);
		}
	}
}
