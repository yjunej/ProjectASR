// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

#include "ASR/Character/Enemy/BaseEnemy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ASR/Enums/EnemyAIState.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AIPerceptionSystem.h"


#include "Kismet/GameplayStatics.h"


ABaseAIController::ABaseAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	// Sight Config
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 800.f;
	SightConfig->LoseSightRadius = 1200.f; 
	SightConfig->PeripheralVisionAngleDegrees = 60.f; 
	SightConfig->SetMaxAge(5.f); 
	SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.f;

	// Detect only specific actors 
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

	// Hearing Config
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 500.f;
	HearingConfig->SetMaxAge(3.f);

	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->ConfigureSense(*HearingConfig);

	// Damage Sense Config
	DamageSenseConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageSenseConfig"));
	DamageSenseConfig->SetMaxAge(5.f);
	AIPerception->ConfigureSense(*DamageSenseConfig);


	AIPerception->OnPerceptionUpdated.AddDynamic(this, &ABaseAIController::PerceptionUpdated);

}

EEnemyAIState ABaseAIController::GetCurrentAIState()
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	return StaticCast<EEnemyAIState>(BBComponent->GetValueAsEnum(AIStateKeyName));
}

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

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseAIController::SetBlackboardKeys()
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	BBComponent->SetValueAsObject(AttackTargetKeyName, UGameplayStatics::GetPlayerCharacter(this, 0));
	BBComponent->SetValueAsEnum(AIStateKeyName, StaticCast<uint8>(EEnemyAIState::EAS_Passive));
	
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(GetPawn());
	if (Enemy != nullptr)
	{
		BBComponent->SetValueAsEnum(CharacterStateKeyName, StaticCast<uint8>(Enemy->GetCharacterState()));
		BBComponent->SetValueAsFloat(AttackDistanceKeyName, Enemy->GetAttackDistance());
		BBComponent->SetValueAsFloat(DefendDistanceKeyName, Enemy->GetDefendDistance());
	}


	// Legacy
	//BBComponent->SetValueAsObject(FName("Target"), UGameplayStatics::GetPlayerCharacter(this, 0));
	//BBComponent->SetValueAsFloat(FName("StrafeDistance"), StrafeDistance);
	//BBComponent->SetValueAsBool(FName("bSaveAttack"), true);
	//float DelaySecond = 2.f;
	//GetWorldTimerManager().SetTimer(TimerHandle, this, &ABaseAIController::ExecuteNormalAttack, DelaySecond, true);
}

void ABaseAIController::SwitchToPassiveState()
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	BBComponent->SetValueAsEnum(AIStateKeyName, StaticCast<uint8>(EEnemyAIState::EAS_Passive));
}

void ABaseAIController::SwitchToAttackState(AActor* TargetActor)
{
	AttackTarget = TargetActor;
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	BBComponent->SetValueAsObject(AttackTargetKeyName, TargetActor);
	BBComponent->SetValueAsEnum(AIStateKeyName, StaticCast<uint8>(EEnemyAIState::EAS_Attack));
}

void ABaseAIController::SwitchToInvestigateState(FVector InvestigateLocaton)
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	BBComponent->SetValueAsVector(AttentionPointKeyName, InvestigateLocaton);
	BBComponent->SetValueAsEnum(AIStateKeyName, StaticCast<uint8>(EEnemyAIState::EAS_Investigate));
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

FAIStimulus ABaseAIController::CanSenseActor(AActor* Actor, EAIPerceptionSense AIPerceptionSense)
{
	FActorPerceptionBlueprintInfo ActorPerceptionBlueprintInfo;
	FAIStimulus ResultStimulus;

	AIPerception->GetActorsPerception(Actor, ActorPerceptionBlueprintInfo);

	TSubclassOf<UAISense> QuerySenseClass;
	switch (AIPerceptionSense)
	{
	case EAIPerceptionSense::EPS_None:
		break;
	case EAIPerceptionSense::EPS_Sight:
		QuerySenseClass = UAISense_Sight::StaticClass();
		break;
	case EAIPerceptionSense::EPS_Hearing:
		QuerySenseClass = UAISense_Hearing::StaticClass();
		break;
	case EAIPerceptionSense::EPS_Damage:
		QuerySenseClass = UAISense_Damage::StaticClass();
		break;
	case EAIPerceptionSense::EPS_MAX:
		break;
	default:
		break;
	}

	TSubclassOf<UAISense> LastSensedStimulusClass;

	for (const FAIStimulus& AIStimulus : ActorPerceptionBlueprintInfo.LastSensedStimuli)
	{
		LastSensedStimulusClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, AIStimulus);


		if (QuerySenseClass == LastSensedStimulusClass)
		{
			ResultStimulus = AIStimulus;
			return ResultStimulus;
		}

	}
	return ResultStimulus;
}

void ABaseAIController::HandleSensedSight(AActor* Actor)
{
	EEnemyAIState CurrentAIState = GetCurrentAIState();
	bool bConvertToAttack = false;

	switch (CurrentAIState)
	{
	case EEnemyAIState::EAS_Passive:
		bConvertToAttack = true;
		break;
	case EEnemyAIState::EAS_Attack:
		break;
	case EEnemyAIState::EAS_Frozen:
		break;
	case EEnemyAIState::EAS_Investigate:
		bConvertToAttack = true;
		break;
	case EEnemyAIState::EAS_Dead:
		break;
	case EEnemyAIState::EAS_MAX:
		break;
	default:
		break;
	}
	
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (bConvertToAttack && PlayerCharacter != nullptr && PlayerCharacter == Actor)
	{
		SwitchToAttackState(Actor);
	}
}

void ABaseAIController::HandleSensedHearing(FVector NoiseLocation)
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	EEnemyAIState CurrentAIState = GetCurrentAIState();

	switch (CurrentAIState)
	{
	case EEnemyAIState::EAS_Passive:
		SwitchToInvestigateState(NoiseLocation);
		break;
	case EEnemyAIState::EAS_Attack:
		break;
	case EEnemyAIState::EAS_Frozen:
		break;
	case EEnemyAIState::EAS_Investigate:
		SwitchToInvestigateState(NoiseLocation);
		break;
	case EEnemyAIState::EAS_Dead:
		break;
	case EEnemyAIState::EAS_MAX:
		break;
	default:
		break;
	}

}

void ABaseAIController::HandleSensedDamage(AActor* Actor)
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	EEnemyAIState CurrentAIState = GetCurrentAIState();

	switch (CurrentAIState)
	{
	case EEnemyAIState::EAS_Passive:
		SwitchToAttackState(Actor);
		break;
	case EEnemyAIState::EAS_Attack:
		break;
	case EEnemyAIState::EAS_Frozen:
		break;
	case EEnemyAIState::EAS_Investigate:
		SwitchToAttackState(Actor);
		break;
	case EEnemyAIState::EAS_Dead:
		break;
	case EEnemyAIState::EAS_MAX:
		break;
	default:
		break;
	}
}

void ABaseAIController::PerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* UpdatedActor : UpdatedActors)
	{
		FAIStimulus AIStimulus;
		AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Sight);
		if (AIStimulus.WasSuccessfullySensed())
		{
			UE_LOG(LogTemp, Warning, TEXT("Sight Sensed!"));
			HandleSensedSight(UpdatedActor);
		}
		AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Hearing);
		if (AIStimulus.WasSuccessfullySensed())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hearing Sensed!"));
			HandleSensedHearing(AIStimulus.StimulusLocation);
		}
		AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Damage);
		if (AIStimulus.WasSuccessfullySensed())
		{
			UE_LOG(LogTemp, Warning, TEXT("Damage Sensed!"));
			HandleSensedDamage(UpdatedActor);
		}
	}
}
