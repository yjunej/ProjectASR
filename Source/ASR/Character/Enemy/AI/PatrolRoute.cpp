// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolRoute.h"

#include "Components/SplineComponent.h"

APatrolRoute::APatrolRoute()
{
	PrimaryActorTick.bCanEverTick = true;
	PatrolSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolSpline"));
	PatrolSpline->SetupAttachment(RootComponent);

}

void APatrolRoute::BeginPlay()
{
	Super::BeginPlay();
}

void APatrolRoute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector APatrolRoute::GetSplinePointLocation()
{
	return PatrolSpline->GetLocationAtSplinePoint(PatrolIndex, ESplineCoordinateSpace::World);
}

void APatrolRoute::IncrementPatrolRoute()
{
	PatrolIndex += Direction;

	if (PatrolIndex == PatrolSpline->GetNumberOfSplinePoints() - 1)
	{
		Direction = -1;
	}
	else if (PatrolIndex == 0)
	{
		Direction = 1;
	}

}

