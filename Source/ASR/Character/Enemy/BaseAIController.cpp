// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

#include "ASR/Character/Enemy/BaseEnemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ASR/Enums/EnemyAIState.h"
#include "Kismet/GameplayStatics.h"


void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(InPawn);

	if (Enemy != nullptr)
	{
		UBehaviorTree* BTAsset = Enemy->GetBehaviorTree();
		RunBehaviorTree(BTAsset);
		SetBlackboardKeys();
	}
}

void ABaseAIController::SetBlackboardKeys()
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	BBComponent->SetValueAsObject(AttackTargetKeyName, UGameplayStatics::GetPlayerCharacter(this, 0));
	BBComponent->SetValueAsEnum(AIStateKeyName, uint8(EEnemyAIState::EAS_Passive));


	// Legacy
	//BBComponent->SetValueAsObject(FName("Target"), UGameplayStatics::GetPlayerCharacter(this, 0));
	//BBComponent->SetValueAsFloat(FName("StrafeDistance"), StrafeDistance);
	//BBComponent->SetValueAsBool(FName("bSaveAttack"), true);
	//float DelaySecond = 2.f;
	//GetWorldTimerManager().SetTimer(TimerHandle, this, &ABaseAIController::ExecuteNormalAttack, DelaySecond, true);
}

void ABaseAIController::SetPassiveState()
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	BBComponent->SetValueAsEnum(AIStateKeyName, uint8(EEnemyAIState::EAS_Passive));
}

void ABaseAIController::SetAttackState(AActor* TargetActor )
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	BBComponent->SetValueAsObject(AttackTargetKeyName, TargetActor);
	BBComponent->SetValueAsEnum(AIStateKeyName, uint8(EEnemyAIState::EAS_Attack));
}

void ABaseAIController::ExecuteNormalAttack()
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(GetPawn());
	if (Enemy != nullptr && Enemy->GetCharacterState() != EASRCharacterState::ECS_Death)
	{
		BBComponent->SetValueAsEnum(FName("State"), StaticCast<uint8>(EEnemyBehaviorState::EBS_Attack));
	}
}
