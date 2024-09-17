// Fill out your copyright notice in the Description page of Project Settings.


#include "RootMotionEnemyAnimInstance.h"
#include "ASR/Interfaces/EnemyAIInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ASR/Character/Enemy/BaseAIController.h"
#include "Kismet/KismetMathLibrary.h"


void URootMotionEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	bIsAttackTargetExists = IsAttackTargetExists();

	// TODO
	if (EnemyAIInterface == nullptr ? EnemyAIInterface : Cast<IEnemyAIInterface>(GetOwningActor()))
	{
		MaxWalkSpeed = EnemyAIInterface->GetMovementSpeed();
	}
	UpdateDesiredDirection();
}

void URootMotionEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	EnemyAIInterface = Cast<IEnemyAIInterface>(GetOwningActor());
}

bool URootMotionEnemyAnimInstance::IsAttackTargetExists() const
{
	APawn* ControlPawn = Cast<APawn>(GetOwningActor());
	if (ControlPawn != nullptr)
	{
		ABaseAIController* BaseAIController = Cast<ABaseAIController>(ControlPawn->GetController());
		if (BaseAIController != nullptr)
		{
			UBlackboardComponent* BBComponent = BaseAIController->GetBlackboardComponent();
			if (BBComponent != nullptr)
			{
				AActor* TargetActor = Cast<AActor>(BBComponent->GetValueAsObject(BaseAIController->GetAttackTargetKeyName()));
				return TargetActor != nullptr;
			}
		}
	}
	return false;
}

void URootMotionEnemyAnimInstance::UpdateDesiredDirection()
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

				if (AcceptanceRadius < DistanceToAttentionPoint)
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
