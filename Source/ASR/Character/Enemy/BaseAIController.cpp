// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

#include "ASR/Character/Enemy/BaseEnemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	BBComponent->SetValueAsObject(FName("Target"), UGameplayStatics::GetPlayerCharacter(this, 0));
	BBComponent->SetValueAsFloat(FName("StrafeDistance"), StrafeDistance);



}
