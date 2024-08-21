// Fill out your copyright notice in the Description page of Project Settings.


#include "Blader.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TargetingComponent.h"
#include "Components/TimelineComponent.h"
#include "ASR/Character/Enemy/BaseEnemy.h"


ABlader::ABlader()
{
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.f, 0.0f));

    WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMeshComponent->SetupAttachment(GetMesh(), FName("RightHandKatanaSocket"));
	
	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));
	FloatCurve = nullptr; // Draw In Editor

}

void ABlader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsUltCharging)
	{
		UltTargets.Empty();
		UCameraComponent* FollowCam = GetFollowCamera();
		FVector TraceEnd;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TraceEnd = FollowCam->GetComponentLocation() + FollowCam->GetForwardVector() * 1000;
		TArray<FHitResult> HitResults;

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(Cast<AActor>(this));


		bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
			GetWorld(),
			GetActorLocation() + GetFollowCamera()->GetForwardVector() * 1000,
			GetActorLocation() + GetFollowCamera()->GetForwardVector() * 1000,
			1000,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResults,
			true
		);
		if (bHit)
		{
			for (const FHitResult& HitResult : HitResults)
			{
				AActor* HitActor = HitResult.GetActor();
				if (HitActor != nullptr && !UltTargets.Contains(HitActor))
				{
					ABaseEnemy* Enemy = Cast<ABaseEnemy>(HitActor);
					if (Enemy != nullptr && Enemy->GetCharacterState() != EASRCharacterState::ECS_Death)
					{
						UltTargets.AddUnique(HitActor);
					}
				}
			}
		}
	}
}

void ABlader::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetMesh()->HideBoneByName(FName("weapon_l"), EPhysBodyOp::PBO_Term);
	GetMesh()->HideBoneByName(FName("weapon_r"), EPhysBodyOp::PBO_Term);
	GetMesh()->HideBoneByName(FName("grenade"), EPhysBodyOp::PBO_Term);
}

void ABlader::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Triggered, this, &ABlader::Input_HeavyAttack);
		EnhancedInputComponent->BindAction(BladerFirstSkillAction, ETriggerEvent::Triggered, this, &ABlader::Input_FirstSkill);
		EnhancedInputComponent->BindAction(UltAction, ETriggerEvent::Started, this, &ABlader::Input_Ult);
		EnhancedInputComponent->BindAction(UltAction, ETriggerEvent::Completed, this, &ABlader::Input_Release_Ult);
	}
}


bool ABlader::CanAttakInAir() const
{
	return bCanAttackInAir && bIsLevitating;
}



void ABlader::BeginPlay()
{
	Super::BeginPlay();
	if (FloatCurve)
	{
		InitializeTimeline();
	}
}

void ABlader::ResetState()
{
	Super::ResetState();

	EMovementMode MovementMode = GetCharacterMovement()->MovementMode;
	if (MovementMode == EMovementMode::MOVE_Flying || MovementMode == EMovementMode::MOVE_Falling)
	{
		// TODO: Block 1 Frame Walking
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	// Reset Attack Pendig & Count
	ResetLightAttack();
	ResetHeavyAttack();
	ResetDodge();
	ResetSkills();

	if (GetTargetingComponent() != nullptr)
	{
		GetTargetingComponent()->ClearSubTarget();
	}
	bCanAttackInAir = true;
	bIsLevitating = false;

	// Attack during Execution
	ResetCamera();
	bIsInvulnerable = false;
}

void ABlader::Input_Move(const FInputActionValue& Value)
{
	if (bIsUltCharging)
	{
		ResetUlt();
	}
	Super::Input_Move(Value);
}

void ABlader::Input_Execution(const FInputActionValue& Value)
{

	if (bIsUltCharging)
	{
		ResetUlt();
	}
	if (GetCharacterMovement()->IsFalling() || GetCharacterMovement()->IsFlying())
	{
		return;
	}
	Super::Input_Execution(Value);

}


void ABlader::Input_LightAttack(const FInputActionValue& Value)
{
	if (bIsUltCharging)
	{
		ResetUlt();
	}
	bIsHeavyAttackPending = false;
	Super::Input_LightAttack(Value);
}

void ABlader::LightAttack()
{
	if (CanAttack())
	{
		if (GetVelocity().Size() >= 1000.f && LightAttackIndex ==0)
		{
			DashLightAttack();
		}
		else
		{
			ResetHeavyAttack();
			ExecuteLightAttack(LightAttackIndex);
		}
	}
	else if (CanAttakInAir())
	{
		ExecuteLightAttackInAir(LightAttackIndex);
	}
}


void ABlader::Input_HeavyAttack(const FInputActionValue& Value)
{
	if (bIsUltCharging)
	{
		ResetUlt();
	}
	bIsLightAttackPending = false;
	if (CharacterState == EASRCharacterState::ECS_Attack)
	{
		bIsHeavyAttackPending = true;
	}
	else
	{
		HeavyAttack();
	}
}

void ABlader::Input_Dodge(const FInputActionValue& Value)
{
	if (bIsUltCharging)
	{
		ResetUlt();
	}
	Super::Input_Dodge(Value);

}

void ABlader::Input_FirstSkill(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("INPUT_FIRSTSKILL!"));
	if (bIsUltCharging)
	{
		ResetUlt();
	}
	bIsLightAttackPending = false;
	bIsHeavyAttackPending = false;
	if (CharacterState == EASRCharacterState::ECS_Attack)
	{
		bIsFirstSkillPending = true;
	}
	else
	{
		FirstSkill();
	}
}

void ABlader::Input_Ult(const FInputActionValue& Value)
{
	if (CanAttack())
	{
		UGameplayStatics::SetGlobalTimeDilation(this, 0.5f);
		PlayAnimMontage(UltReadyMontage);
		bIsUltCharging = true;
	}

}

void ABlader::Input_Release_Ult(const FInputActionValue& Value)
{
	// TODO - make Combat state Reset function (Attack Counter, Pending Attack, TimeDilation)
	UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
	UltStartTransform = GetActorTransform();

	bIsUltCharging = false;
	UltTargetIndex = 0;

	if (UltTargets.Num() > 0)
	{
		AActor* Target = UltTargets[UltTargetIndex];

		GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(
			FName("Ult"), Target->GetActorLocation(), Target->GetActorRotation()
		);
		CharacterState = EASRCharacterState::ECS_Attack;
		PlayUltAttackMontage();
	}
	else
	{
		ResetUlt();
	}
}

void ABlader::Input_Guard(const FInputActionValue& Value)
{
	if (bIsUltCharging)
	{
		ResetUlt();
	}
	Super::Input_Guard(Value);
}

bool ABlader::CanAttack() const
{
	bool bCanAttack = Super::CanAttack();
	return bCanAttack && !bIsLevitating;
}


void ABlader::HeavyAttack()
{
	if (CanAttack())
	{
		// TODO: NOT RESET L/H Counter for custom combo

		if (GetVelocity().Size() >= 1000.f && HeavyAttackIndex == 0)
		{
			DashHeavyAttack();
		}
		else
		{
			ResetLightAttack();
			ExecuteHeavyAttack(HeavyAttackIndex);
		}


	}
}

void ABlader::DashLightAttack()
{
	SetCharacterState(EASRCharacterState::ECS_Attack);
	ResetLightAttack();
	ResetHeavyAttack();
	ResetSkills();
	ResetDodge();
	PlayAnimMontage(DashLightAttackMontage);
}

void ABlader::DashHeavyAttack()
{
	SetCharacterState(EASRCharacterState::ECS_Attack);
	ResetLightAttack();
	ResetHeavyAttack();
	ResetSkills();
	ResetDodge();
	PlayAnimMontage(DashHeavyAttackMontage);
}


void ABlader::FirstSkill()
{
	UE_LOG(LogTemp, Warning, TEXT("FIRSTSKILL!"));

	if (CanAttack())
	{
		ResetLightAttack();
		ResetHeavyAttack();
		ResetDodge();
		ExecuteAerialAttack();
	}
}

void ABlader::PlayUltAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && UltAttackMontage != nullptr)
	{
		AnimInstance->Montage_Play(UltAttackMontage);
	}
}


void ABlader::ExecuteLightAttackInAir(int32 AttackIndex)
{
	if (AttackIndex >= LightAttackInAirMontages.Num())
	{
		LightAttackIndex = 0;
	}
	else
	{
		if (LightAttackInAirMontages.IsValidIndex(AttackIndex) && LightAttackInAirMontages[AttackIndex] != nullptr)
		{
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
			SetCharacterState(EASRCharacterState::ECS_Attack);
			PlayAnimMontage(LightAttackInAirMontages[AttackIndex]);

			if (LightAttackIndex + 1 >= LightAttackInAirMontages.Num())
			{
				LightAttackIndex = 0;
				bCanAttackInAir = false;	
			}
			else
			{
				++LightAttackIndex;
			}
		}
	}
}

void ABlader::ExecuteHeavyAttack(int32 AttackIndex)
{
	if (AttackIndex >= HeavyAttackMontages.Num())
	{
		HeavyAttackIndex = 0;
	}
	else
	{
		if (HeavyAttackMontages.IsValidIndex(AttackIndex) && HeavyAttackMontages[AttackIndex] != nullptr)
		{
			SetCharacterState(EASRCharacterState::ECS_Attack);


			PlayAnimMontage(HeavyAttackMontages[AttackIndex]);

			if (HeavyAttackIndex + 1 >= HeavyAttackMontages.Num())
			{
				HeavyAttackIndex = 0;
			}
			else
			{
				++HeavyAttackIndex;
			}
		}
	}
}

void ABlader::ResetHeavyAttack()
{
	bIsHeavyAttackPending = false;
	HeavyAttackIndex = 0;
}

void ABlader::ResetSkills()
{
	bIsFirstSkillPending = false;
}


void ABlader::ExecuteAerialAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("EXECUTEAERIAL!"));

	if (AerialAttackMontage != nullptr)
	{
		SetCharacterState(EASRCharacterState::ECS_Attack);
		UE_LOG(LogTemp, Warning, TEXT("PLAYANIM!"));

		PlayAnimMontage(AerialAttackMontage);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		bIsLevitating = true;
	}
}

bool ABlader::AerialAttack()
{
	if (GetTargetingComponent()->IsTargeting())
	{
		if (PrevInput.Y <= -0.5)
		{
			ExecuteAerialAttack();
			return true;
		}
	}
	return false;
}

void ABlader::UltEnd()
{
	GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(
		FName("Ult"), UltStartTransform.GetLocation(), UltStartTransform.GetRotation().Rotator()
	);
	PlayAnimMontage(UltLastAttackMontage, 1.f);
}

void ABlader::ResetUlt()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
	bIsUltCharging = false;

	float FadeOutTime = 0.2f; 
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(UltReadyMontage))
	{
		AnimInstance->Montage_Stop(FadeOutTime, UltReadyMontage);
	}

	UltTargetIndex = 0;
	UltTargets.Empty();

}


void ABlader::ResolveLightAttackPending()
{
	if (bIsFirstSkillPending)
	{
		bIsFirstSkillPending = false; 
		bIsLightAttackPending = false;
		if (CharacterState == EASRCharacterState::ECS_Attack)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}
		FirstSkill();
	}

	Super::ResolveLightAttackPending();

}

void ABlader::ResolveHeavyAttackPending()
{
	if (bIsHeavyAttackPending)
	{
		bIsHeavyAttackPending = false;

		// Process Pending H Attack
		if (CharacterState == EASRCharacterState::ECS_Attack)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}

		// Try Heavy Attack (CanAttack check in this function)
		HeavyAttack();

	}
}

void ABlader::HandleTimelineUpdate(float Value)
{
	if (GetWorld() != nullptr)
	{
		FVector NewLocation = UKismetMathLibrary::VLerp(LevitateLocation, LevitateLocation + FVector(0.f, 0.f, LevitateHeight), Value);
		SetActorLocation(NewLocation, true);
	}
}

void ABlader::InitializeTimeline()
{
	if (FloatCurve != nullptr)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("HandleTimelineUpdate"));
		TimelineComponent->AddInterpFloat(FloatCurve, TimelineCallback);

		FOnTimelineEvent TimelineFinishedCallback;
		TimelineFinishedCallback.BindUFunction(this, FName("HandleTimelineFinished"));
		TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
}

void ABlader::StopTimeline()
{
	if (TimelineComponent != nullptr)
	{
		TimelineComponent->Stop();
	}
}

void ABlader::Levitate()
{
	LevitateLocation = GetActorLocation();
	TimelineComponent->PlayFromStart();
}

void ABlader::ApplyUltDamage()
{
	for (AActor* Target : UltTargets)
	{
		IHitInterface* HitInterface = Cast<IHitInterface>(Target);
		FHitResult HitResult;
		HitInterface->GetHit(HitResult, this, 10000.f, EASRDamageType::EDT_Die);
	}
}
