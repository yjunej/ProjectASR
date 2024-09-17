// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAnimInstance.h"
#include "ASR/Character/Enemy/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ASR/Interfaces/EnemyAIInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"

void UBaseEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	bIsFocusingTarget = IsFocusingTarget();
	bIsAttackTargetExists = IsAttackTargetExists();
	
	// TODO
	if (EnemyAIInterface == nullptr ? EnemyAIInterface : Cast<IEnemyAIInterface>(GetOwningActor()))
	{
		MaxWalkSpeed = EnemyAIInterface->GetMovementSpeed();
	}
	UpdateDesiredDirection();
	
}

void UBaseEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	EnemyAIInterface = Cast<IEnemyAIInterface>(GetOwningActor());
}

bool UBaseEnemyAnimInstance::IsFocusingTarget() const
{
	APawn* ControlPawn = Cast<APawn>(GetOwningActor());
	if (ControlPawn != nullptr)
	{
		AAIController* AIController = Cast<AAIController>(ControlPawn->GetController());
		if (AIController != nullptr)
		{
			return AIController->GetFocusActor() != nullptr;
		}
	}
	return false;
}

bool UBaseEnemyAnimInstance::IsAttackTargetExists() const
{
	APawn* ControlPawn = Cast<APawn>(GetOwningActor());
	if (ControlPawn != nullptr)
	{
		ABaseAIController* AIController = Cast<ABaseAIController>(ControlPawn->GetController());
		if (AIController != nullptr)
		{
			UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();
			if (BBComponent != nullptr)
			{
				AActor* TargetActor = Cast<AActor>(BBComponent->GetValueAsObject(AIController->GetAttackTargetKeyName()));
				return TargetActor != nullptr;
			}
		}
	}
	return false;
}

void UBaseEnemyAnimInstance::UpdateDesiredDirection()
{
	APawn* ControlPawn = Cast<APawn>(GetOwningActor());
	if (ControlPawn != nullptr)
	{
		ABaseAIController* AIController = Cast<ABaseAIController>(ControlPawn->GetController());
		if (AIController != nullptr)
		{
			UBlackboardComponent* BBComponent = AIController->GetBlackboardComponent();
			if (BBComponent != nullptr)
			{
				FVector AttentionPoint = BBComponent->GetValueAsVector(AIController->GetAttentionPointKeyName());
				float DistanceToAttentionPoint = UKismetMathLibrary::Vector_Distance2D(ControlPawn->GetActorLocation(), AttentionPoint);

				//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), DistanceToAttentionPoint);

				if (AIController->GetAcceptanceRadius() < DistanceToAttentionPoint)
				{
					FVector DirectionUnitVector = UKismetMathLibrary::GetDirectionUnitVector(ControlPawn->GetActorLocation(), AttentionPoint);
					FRotator LookAtRotation = UKismetMathLibrary::MakeRotFromX(DirectionUnitVector);
					FRotator PawnRotation = ControlPawn->GetActorRotation();
					DesiredDirection = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRotation, PawnRotation).Yaw;
					return;
				}
				else
				{	
					//UE_LOG(LogTemp, Warning, TEXT("IdleMode: %f %f"), AIController->GetAcceptanceRadius(), DistanceToAttentionPoint);

					//DesiredDirection = 0.f;
					//IEnemyAIInterface* EnemyAIInterface = Cast<IEnemyAIInterface>(GetOwningActor());
					//if (EnemyAIInterface != nullptr)
					//{
						//EnemyAIInterface->SetMovementSpeed(EEnemyMovementSpeed::EMS_Idle);
					//}
					return;
				}
			}
		}
	}
}
