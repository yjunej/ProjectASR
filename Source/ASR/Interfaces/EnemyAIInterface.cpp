// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIInterface.h"

// Add default functionality here for any IEnemyAI functions that are not pure virtual.

APatrolRoute* IEnemyAIInterface::GetPatrolRoute_Implementation() const
{
	return nullptr;
}

float IEnemyAIInterface::SetMovementSpeed_Implementation(EEnemyMovementSpeed EnemyMovementSpeed)
{
	return 0.0f;
}
