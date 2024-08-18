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

#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/DirectionalLightComponent.h"

#include "Engine/DirectionalLight.h"


ABlader::ABlader()
{
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.f));

    WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMeshComponent->SetupAttachment(GetMesh(), FName("RightHandKatanaSocket"));  // 소켓에 부착
	
	GetCharacterMovement()->MaxWalkSpeed = 1050.f;

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

	const USkeletalMeshSocket* RHKatanaSocket = GetMesh()->GetSocketByName(FName("RightHandKatanaSocket"));


}

void ABlader::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Triggered, this, &ABlader::Input_LightAttack);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Triggered, this, &ABlader::Input_HeavyAttack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ABlader::Input_Dodge);
		EnhancedInputComponent->BindAction(FirstSkillAction, ETriggerEvent::Triggered, this, &ABlader::Input_FirstSkill);
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
	bIsFirstSkillPending = false;

	if (GetTargetingComponent() != nullptr)
	{
		GetTargetingComponent()->ClearSubTarget();
	}
	bCanAttackInAir = true;
	bIsLevitating = false;
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
	Super::Input_Execution(Value);

	if (bIsUltCharging)
	{
		ResetUlt();
	}
	if (CanExecution() && !bIsExecuting && !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
	{
		Execution();
	}

}

void ABlader::ResetCamera()
{
	Super::ResetCamera();
}

void ABlader::Input_LightAttack(const FInputActionValue& Value)
{
	if (bIsUltCharging)
	{
		ResetUlt();
	}
	bIsHeavyAttackPending = false;
	
	if (CharacterState == EASRCharacterState::ECS_Attack)
	{
		bIsLightAttackPending = true;
	}
	else
	{
		LightAttack();
	}

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
	if (CharacterState == EASRCharacterState::ECS_Attack || CharacterState == EASRCharacterState::ECS_Dodge)
	{
		bIsDodgePending = true;
	}
	else
	{
		Dodge();
	}

}

void ABlader::Input_FirstSkill(const FInputActionValue& Value)
{
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
		UseFirstSkill();
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
	ResetFirstSkill();
	ResetDodge();
	PlayAnimMontage(DashLightAttackMontage);
}

void ABlader::DashHeavyAttack()
{
	SetCharacterState(EASRCharacterState::ECS_Attack);
	ResetLightAttack();
	ResetHeavyAttack();
	ResetFirstSkill();
	ResetDodge();
	PlayAnimMontage(DashHeavyAttackMontage);
}

void ABlader::Execution()
{
	// Camera Setting

	SetCharacterState(EASRCharacterState::ECS_Attack);
	ResetLightAttack();	
	ResetHeavyAttack();
	ResetFirstSkill();
	ResetDodge();

	FTransform WarpTransform;
	FTransform TargetTransform = GetTargetingComponent()->GetTargetTransform();
	float WarpDistance = ExecutionDistance > TargetTransform.GetLocation().Length() ? TargetTransform.GetLocation().Length() : ExecutionDistance;


	ABaseEnemy* Enemy = Cast<ABaseEnemy>(GetTargetingComponent()->GetTargetActor());
	if (Enemy != nullptr)
	{
		bIsExecuting = true;
	}


	WarpTransform.SetLocation(GetActorLocation() + TargetTransform.GetLocation().GetSafeNormal() * WarpDistance);
	WarpTransform.SetRotation(TargetTransform.GetRotation());
	WarpTransform.SetScale3D(FVector(1.f, 1.f, 1.f));

	GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromTransform(FName("Execution"), WarpTransform);
	PlayAnimMontage(ExecutionMontage);

}


void ABlader::Dodge()
{
	if (CanDodge())
	{
		SetCharacterState(EASRCharacterState::ECS_Dodge); 

		ResetLightAttack();
		ResetHeavyAttack();

		// Rotate Before Dodge
		FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();
		if (LastInputVector.Size() != 0.f)
		{
			SetActorRotation(UKismetMathLibrary::MakeRotFromX(LastInputVector));
		}


		GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(
			FName("Dodge"), GetActorLocation() + GetActorForwardVector() * 150,
			GetActorRotation());
		PlayAnimMontage(DodgeMontage); 
	}
}

void ABlader::UseFirstSkill()
{
	if (CanAttack())
	{
		ResetLightAttack();
		ResetHeavyAttack();
		bIsDodgePending = false;

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


void ABlader::ExecuteLightAttack(int32 AttackIndex)
{
	if (AttackIndex >= LightAttackMontages.Num())
	{
		LightAttackIndex = 0;
	}
	else
	{
		if (LightAttackMontages.IsValidIndex(AttackIndex) && LightAttackMontages[AttackIndex] != nullptr)
		{
			SetCharacterState(EASRCharacterState::ECS_Attack);
			PlayAnimMontage(LightAttackMontages[AttackIndex]);

			if (LightAttackIndex + 1 >= LightAttackMontages.Num())
			{
				LightAttackIndex = 0;
			}
			else
			{
				++LightAttackIndex;
			}
		}
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


void ABlader::ResetLightAttack()
{
	bIsLightAttackPending = false;
	LightAttackIndex = 0;
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

void ABlader::ResetFirstSkill()
{
	bIsFirstSkillPending = false;
}

void ABlader::ResetDodge()
{
	bIsDodgePending = false;
}

void ABlader::ExecuteAerialAttack()
{
	if (AerialAttackMontage != nullptr)
	{
		SetCharacterState(EASRCharacterState::ECS_Attack);
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
		if (CharacterState == EASRCharacterState::ECS_Attack)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}
		UseFirstSkill();
	}

	if (bIsLightAttackPending)
	{

		bIsLightAttackPending = false;

		// Process Pending L Attack
		if (CharacterState == EASRCharacterState::ECS_Attack)
		{

			CharacterState = EASRCharacterState::ECS_None;
		}

		// Try Light Attack (CanAttack check in this function)
		LightAttack();

	}
	
	// TODO: make sophisticated Pending Action System - Categorize Actions


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

void ABlader::ResolveDodgeAndGuardPending()
{
	if (bIsGuardPressed)
	{
		bIsDodgePending = false;
		if (CharacterState == EASRCharacterState::ECS_Attack || CharacterState == EASRCharacterState::ECS_Dodge || CharacterState == EASRCharacterState::ECS_Guard)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}

		Guard();
	}
	else if (bIsDodgePending)
	{
		bIsDodgePending = false;
		if (CharacterState == EASRCharacterState::ECS_Attack || CharacterState == EASRCharacterState::ECS_Dodge || CharacterState == EASRCharacterState::ECS_Guard)
		{
			CharacterState = EASRCharacterState::ECS_None;
		}
		Dodge();
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

void ABlader::SetExecutionCamera()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->SetViewTargetWithBlend(GetExecutionCameraManager()->GetChildActor(), 0.2f, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.0f, false);
}

