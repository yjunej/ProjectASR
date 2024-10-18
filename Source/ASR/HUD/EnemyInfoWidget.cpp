// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyInfoWidget.h"

#include "ASR/Character/Enemy/BaseEnemy.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UEnemyInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEnemyInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePostHealthBar();
	UpdatePostStaminaBar();

}

void UEnemyInfoWidget::UpdateHealthBar()
{
	if (Owner != nullptr)
	{
		if (Owner->MaxHealth > 0)
		{
			EnemyHealthBar->SetPercent(Owner->Health / Owner->MaxHealth);
		}

	}
}

void UEnemyInfoWidget::UpdateStaminaBar()
{
	if (Owner != nullptr)
	{
		if (Owner->MaxStamina > 0)
		{
			EnemyStaminaBar->SetPercent(Owner->Stamina / Owner->MaxStamina);
			if (Owner->Stamina <= 0.f)
			{
				UpdateDamageMultiPlier();

				FProgressBarStyle StaminaBarStyle = EnemyStaminaBar->GetWidgetStyle();
				StaminaBarStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor::Red);
				EnemyStaminaBar->SetWidgetStyle(StaminaBarStyle);

				// 0.2
				// 0.5, 0.0, 0.0, 1.0
				DamageMultiplierIcon->SetColorAndOpacity(FLinearColor(0.25f, 0.0f, 0.0f, 1.0f));
				DamageMultiplierText->SetColorAndOpacity(FSlateColor(FLinearColor(0.25f, 0.0f, 0.0f, 1.0f)));

				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UEnemyInfoWidget::ResetGuardBrokenColorChange, 1.0f, false);


			}
		}
	}
}

void UEnemyInfoWidget::UpdatePostHealthBar()
{
	if (Owner != nullptr)
	{
		if (Owner->MaxHealth > 0)
		{
			PostHealth = FMath::FInterpTo(PostHealth, Owner->Health, GetWorld()->GetDeltaSeconds(), PostBarLerpSpeed);
			EnemyPostHealthBar->SetPercent(PostHealth / Owner->MaxHealth);
		}
	}
}

void UEnemyInfoWidget::UpdatePostStaminaBar()
{
	if (Owner != nullptr)
	{
		if (Owner->MaxStamina > 0)
		{
			PostStamina = FMath::FInterpTo(PostStamina, Owner->Stamina, GetWorld()->GetDeltaSeconds(), PostBarLerpSpeed);
			EnemyPostStaminaBar->SetPercent(PostStamina / Owner->MaxStamina);
		}
	}
}

void UEnemyInfoWidget::SetOwner(ABaseEnemy* NewOwner)
{
	if (Owner != NewOwner && NewOwner != nullptr)
	{	
		if (Owner != nullptr)
		{
			Owner->OnHealthChanged.RemoveDynamic(this, &UEnemyInfoWidget::UpdateHealthBar);
			Owner->OnStaminaChanged.RemoveDynamic(this, &UEnemyInfoWidget::UpdateStaminaBar);
		}
		Owner = NewOwner;
		Owner->OnHealthChanged.AddDynamic(this, &UEnemyInfoWidget::UpdateHealthBar);
		Owner->OnStaminaChanged.AddDynamic(this, &UEnemyInfoWidget::UpdateStaminaBar);

		UpdateHealthBar();
		UpdateStaminaBar();
		PostHealth = Owner->Health;
		PostStamina = Owner->Stamina;
	}
}

void UEnemyInfoWidget::ResetGuardBrokenColorChange()
{
	if (Owner != nullptr)
	{
		FProgressBarStyle StaminaBarStyle = EnemyStaminaBar->GetWidgetStyle();
		StaminaBarStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor::White);
		EnemyStaminaBar->SetWidgetStyle(StaminaBarStyle);
		DamageMultiplierIcon->SetColorAndOpacity(FLinearColor::White);
		DamageMultiplierText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}
}

void UEnemyInfoWidget::UpdateDamageMultiPlier()
{
	if (Owner != nullptr)
	{
		float DamageMultiplier = Owner->GetDamageMultiplier();
		DamageMultiplierText->SetText(FText::FromString(FString::Printf(TEXT("x%.1f"), DamageMultiplier)));
	}
}
