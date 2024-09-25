// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseFieldSystem.h"
#include "Field/FieldSystemComponent.h"


ABaseFieldSystem::ABaseFieldSystem()
{
	RadialFalloff = CreateDefaultSubobject<URadialFalloff>(TEXT("RadialFalloff"));
	RadialVector = CreateDefaultSubobject<URadialVector>(TEXT("RadialVector"));
	FieldSystemMetaData = CreateDefaultSubobject<UFieldSystemMetaData>(TEXT("FieldSystemMetaData"));
}

void ABaseFieldSystem::BeginPlay()
{
	Super::BeginPlay();

	if (RadialFalloff != nullptr && FieldSystemComponent != nullptr)
	{
		RadialFalloff->SetRadialFalloff(
			1000000.f,
			0.8f,
			1.f,
			0.f,
			200.f,
			GetActorLocation(),
			EFieldFalloffType::Field_FallOff_None
		);

		FieldSystemComponent->ApplyPhysicsField(
			true,
			EFieldPhysicsType::Field_ExternalClusterStrain,
			nullptr,
			RadialFalloff
		);

		RadialVector->SetRadialVector(
			1500000.f,
			GetActorLocation()
		);

		FieldSystemComponent->ApplyPhysicsField(
			true,
			EFieldPhysicsType::Field_LinearForce,
			nullptr,
			RadialVector
		);
	}


}
