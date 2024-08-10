// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

#include "Components/CapsuleComponent.h"


ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseEnemy::ResetState()
{
	CharacterState = EASRCharacterState::ECS_None;
}

void ABaseEnemy::HandleDeath()
{
	CharacterState = EASRCharacterState::ECS_Death;
	PlayAnimMontage(DeathMontage); 
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAnimMontage* ABaseEnemy::GetHitReactionMontage(EASRDamageType DamageType)
{
	return nullptr;
}

void ABaseEnemy::SetCharacterState(EASRCharacterState InCharacterState)
{
	{
		if (InCharacterState != CharacterState)
		{
			CharacterState = InCharacterState;
		}
	}
}

void ABaseEnemy::GetHit(const FHitResult& HitResult, float Damage, EASRDamageType DamageType)
{
	// [TODO] Block Handling Before Take Damage
	
	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("HEALTH: %f"), Health);

	if (CharacterState == EASRCharacterState::ECS_Death)
	{
		return;
	}

	if (Health <= 0)
	{
		HandleDeath();
		return;
	}
	else
	{
		FDamageTypeMapping* Mapping;
		Mapping = DamageTypeMappings.Find(DamageType);
		if (Mapping != nullptr)
		{
			CharacterState = Mapping->CharacterState;
			PlayAnimMontage(Mapping->HitReactionMontage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NULL DamageType Mapping!"));
		}
	}
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

