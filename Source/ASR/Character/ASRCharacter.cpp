// Fill out your copyright notice in the Description page of Project Settings.


#include "ASRCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "MotionWarpingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TargetingComponent.h"
#include "Sound/SoundCue.h"
#include "Blueprint/UserWidget.h"
#include "ASR/HUD/ASRMainHUD.h"
#include "ASR/Character/Enemy/BaseEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/TimelineComponent.h"
#include "Camera/CameraActor.h"




// Sets default values
AASRCharacter::AASRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Disable Side Walk by Default
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// Action Game Movement
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->RotationRate.Yaw = YawRotationRate;
	bUseControllerRotationYaw = false;


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 16.f;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 120.f));

	// Katana Blade Zero 
	// CameraBoom->TargetArmLength = 350.f;
	// CameraBoom->SocketOffset = FVector(0.f, 100.f, 40.f);

	// Sekiro
	// CameraBoom->TargetArmLength = 400.f;
	// CameraBoom->SocketOffset = FVector(0.f, 0.f, 40.f);

	// Elden Ring
	CameraBoom->TargetArmLength = 450.f;
	CameraBoom->SocketOffset = FVector(0.f, 10.f, 40.f);
	CameraBoom->CameraLagMaxDistance = 200.f;
	CameraBoom->CameraLagSpeed = 8.f;
	CameraBoom->CameraRotationLagSpeed = 14.f;
	CameraBoom->bEnableCameraRotationLag = true;


	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetupAttachment(GetMesh());

	//FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	FollowCameraManager = CreateDefaultSubobject<UChildActorComponent>(TEXT("FollowCameraManager"));
	FollowCameraManager->SetupAttachment(CameraBoom);
	FollowCameraManager->SetChildActorClass(ACameraActor::StaticClass());

	//ExecutionCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ExecutionCamera"));
	//ExecutionCamera->SetupAttachment(RootComponent);

	ExecutionCameraManager = CreateDefaultSubobject<UChildActorComponent>(TEXT("ExecutionCameraManager"));
	ExecutionCameraManager->SetupAttachment(RootComponent);
	ExecutionCameraManager->SetChildActorClass(ACameraActor::StaticClass());

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	TargetingComp = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
	TargetingComp->Owner = this;

	ArmLengthTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ArmLengthTimeline"));

	SetCombatState(ECombatState::ECS_None);

}

void AASRCharacter::PlayRandomSection(UAnimMontage* Montage)
{
	if (Montage != nullptr)
	{
		int32 NumSections = Montage->GetNumSections();
		PlayAnimMontage(Montage, 1.f, Montage->GetSectionName(FMath::RandRange(0, NumSections - 1)));
	}

}

bool AASRCharacter::IsAttackFromFront(const FHitResult& HitResult) const
{
	FVector AttackDirection = (HitResult.TraceStart - HitResult.TraceEnd).GetSafeNormal();
	FVector CharacterForward = GetActorForwardVector();
	FVector NormalizedAttackDirection = AttackDirection.GetSafeNormal();

	float DotProduct = FVector::DotProduct(CharacterForward, NormalizedAttackDirection);

	// forward 120 degrees
	return DotProduct > 0.5f; 
}

void AASRCharacter::UpdateArmLength(float Value)
{
	if (CameraBoom != nullptr)
	{
		CameraBoom->TargetArmLength = InitialArmLength + Value;
	}
}

void AASRCharacter::StartArmLengthChange(UCurveFloat* ArmCurve)
{
	ArmCurve == nullptr ? ArmLengthCurve : ArmCurve;
	if (ArmCurve != nullptr)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateArmLength"));
		ArmLengthTimeline->AddInterpFloat(ArmCurve, TimelineCallback);
		ArmLengthTimeline->SetLooping(false);
		ArmLengthTimeline->PlayFromStart();
	}
}

void AASRCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (MainHUDWidgetClass != nullptr)
	{
		UUserWidget* UserWidget = CreateWidget(GetWorld(), MainHUDWidgetClass);
		if (UserWidget != nullptr)
		{
			MainHUDWidget = Cast<UASRMainHUD>(UserWidget);
			if (MainHUDWidget != nullptr)
			{
				MainHUDWidget->Owner = this;
				MainHUDWidget->UpdateHealthBar();
				MainHUDWidget->AddToViewport();
			}
		}
			
	}
	

	// Camera Actor Setting
	if (FollowCameraManager != nullptr)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		PlayerController->SetViewTargetWithBlend(FollowCameraManager->GetChildActor(), 0.0f, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.0f, false);
	}


	SetHealth(MaxHealth);
	SetStamina(MaxStamina);

	GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &AASRCharacter::RegenStamina, StaminaRegenInterval, true);
	if (CameraBoom != nullptr)
	{
		InitialArmLength = CameraBoom->TargetArmLength;
	}
}

void AASRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AASRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AASRCharacter::Input_Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AASRCharacter::Input_Look);
		EnhancedInputComponent->BindAction(ToggleCrouchAction, ETriggerEvent::Started, this, &AASRCharacter::Input_ToggleCrouch);
		EnhancedInputComponent->BindAction(ToggleLockOnAction, ETriggerEvent::Triggered, this, &AASRCharacter::Input_ToggleLockOn);
		EnhancedInputComponent->BindAction(ExecutionAction, ETriggerEvent::Triggered, this, &AASRCharacter::Input_Execution);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AASRCharacter::Input_Dodge);

		EnhancedInputComponent->BindAction(NormalAttackAction, ETriggerEvent::Triggered, this, &AASRCharacter::Input_NormalAttack);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AASRCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AASRCharacter::StopJumping);

		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Started, this, &AASRCharacter::Input_Guard);
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Completed, this, &AASRCharacter::Input_Release_Guard);

	}
}

void AASRCharacter::Input_Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();
	PrevInput = MoveVector;

	// Guard Accept, Gunner Flinching Not use Root motion (Knockback by launch..)
	if (CombatState == ECombatState::ECS_Guard || CombatState == ECombatState::ECS_Flinching ||
		CombatState == ECombatState::ECS_KnockDown || CombatState == ECombatState::ECS_Death)
	{
		return;
	}

	if (Controller != nullptr)
	{
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector ForwardDirectionVector(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		const FVector RightDirectionVector(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		AddMovementInput(ForwardDirectionVector, MoveVector.Y);
		AddMovementInput(RightDirectionVector, MoveVector.X);
	}
}

void AASRCharacter::Input_Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		if (TargetingComp == nullptr || !(TargetingComp->bIsTargeting))
		{
			if (!bIsExecuting)
			{
				AddControllerYawInput(LookVector.X);
				AddControllerPitchInput(LookVector.Y);
			}
		}
	}
}

void AASRCharacter::Input_ToggleCrouch(const FInputActionValue& Value)
{
	if (GetMovementComponent()->IsFalling())
	{
		return;
	}

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AASRCharacter::Input_Guard(const FInputActionValue& Value)
{
	bIsGuardPressed = true;
	if (CombatState != ECombatState::ECS_Attack && CombatState != ECombatState::ECS_Guard && CombatState != ECombatState::ECS_Dodge && CombatState != ECombatState::ECS_Flinching)
	{
		Guard();
	}
}

void AASRCharacter::Input_Dodge(const FInputActionValue& Value)
{
	if (CombatState == ECombatState::ECS_Attack || CombatState == ECombatState::ECS_Dodge)
	{
		bIsDodgePending = true;
	}
	else
	{
		Dodge();
	}
}

void AASRCharacter::Input_NormalAttack(const FInputActionValue& Value)
{
	if (CombatState == ECombatState::ECS_Attack)
	{
		bIsNormalAttackPending = true;
	}
	else
	{
		NormalAttack();
	}
}


void AASRCharacter::ResetNormalAttack()
{
	bIsNormalAttackPending = false;
	NormalAttackIndex = 0;
}

float AASRCharacter::GetFirstSkillWarpDistance() const
{
	return NormalAttackWarpDistance;
}

void AASRCharacter::ExecuteNormalAttack(int32 AttackIndex)
{
	if (AttackIndex >= NormalAttackMontages.Num())
	{
		NormalAttackIndex = 0;
	}
	else
	{
		if (NormalAttackMontages.IsValidIndex(AttackIndex) && NormalAttackMontages[AttackIndex] != nullptr)
		{
			SetCombatState(ECombatState::ECS_Attack);
			PlayAnimMontage(NormalAttackMontages[AttackIndex]);

			if (NormalAttackIndex + 1 >= NormalAttackMontages.Num())
			{
				NormalAttackIndex = 0;
			}
			else
			{
				++NormalAttackIndex;
			}
		}
	}
}

void AASRCharacter::ResolveLightAttackPending()
{
	if (bIsNormalAttackPending)
	{
		bIsNormalAttackPending = false;
		if (CombatState == ECombatState::ECS_Attack)
		{
			SetCombatState(ECombatState::ECS_None);
		}
		NormalAttack();
	}
}

void AASRCharacter::ResolveDodgeAndGuardPending()
{
	if (bIsGuardPressed)
	{
		bIsDodgePending = false;
		if (CombatState == ECombatState::ECS_Attack || CombatState == ECombatState::ECS_Dodge || CombatState == ECombatState::ECS_Guard)
		{
			SetCombatState(ECombatState::ECS_None);
		}

		Guard();
	}
	else if (bIsDodgePending)
	{
		bIsDodgePending = false;
		if (CombatState == ECombatState::ECS_Attack || CombatState == ECombatState::ECS_Dodge || CombatState == ECombatState::ECS_Guard)
		{
			SetCombatState(ECombatState::ECS_None);
		}
		Dodge();
	}
}


void AASRCharacter::Input_Release_Guard(const FInputActionValue& Value)
{
	bIsGuardPressed = false;
	if (CombatState == ECombatState::ECS_Guard)
	{
		SetCombatState(ECombatState::ECS_None);
		PlayAnimMontage(GuardMontage, 1.f, FName("GuardEnd"));
	}
}

bool AASRCharacter::CanAttack() const
{
	if (CombatState != ECombatState::ECS_Attack && CombatState != ECombatState::ECS_Dodge
		&& CombatState != ECombatState::ECS_Death && !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying()
		&& CombatState != ECombatState::ECS_Flinching && CombatState != ECombatState::ECS_KnockDown)
	{
		return true;
	}
	return false;
}

void AASRCharacter::Dodge()
{
	if (CanDodge())
	{
		SetCombatState(ECombatState::ECS_Dodge);

		ResetNormalAttack();
		ResetSkills();

		// Rotate Before Dodge
		FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();
		FName SectionName("Forward");
		float StaminaAmount = 150.f;

		if (LastInputVector.Size() != 0.f)
		{
			if (!bIsStrafe)
			{
				SetActorRotation(UKismetMathLibrary::MakeRotFromX(LastInputVector));
			}
			else
			{
				FRotator RotFromX =	UKismetMathLibrary::MakeRotFromX(LastInputVector);
				float DeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(RotFromX, GetActorRotation()).Yaw;
				if (-45.f <= DeltaYaw && DeltaYaw < 45.f)
				{
					SectionName = "Forward";
				}
				else if (-135.f <= DeltaYaw && DeltaYaw < -45.f)
				{
					SectionName = "Left";
					StaminaAmount = 120.f;
				}
				else if (45 <= DeltaYaw && DeltaYaw < 135.f)
				{
					SectionName = "Right";
					StaminaAmount = 120.f;
				}
				else
				{
					SectionName = "Backward";
				}
			}
		}
		SetStamina(Stamina - StaminaAmount);
		PlayAnimMontage(DodgeMontage, 1.f, SectionName);

	}
}

bool AASRCharacter::CanDodge() const
{
	if (CombatState != ECombatState::ECS_Attack && CombatState != ECombatState::ECS_Dodge
		&& CombatState != ECombatState::ECS_Death && !GetCharacterMovement()->IsFalling() && CombatState != ECombatState::ECS_Flinching && Stamina > 0)
	{
		return true;
	}
	return false;
}

void AASRCharacter::ResetDodge()
{
	bIsDodgePending = false;
}

void AASRCharacter::Execution()
{
	SetCombatState(ECombatState::ECS_Attack);
	ResetNormalAttack();
	ResetSkills();
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

	// TODO: Solve Anim Notify Bug
	bIsInvulnerable = true;


	PlayAnimMontage(ExecutionMontage);
	FOnMontageEnded LMontageEnded;
	LMontageEnded.BindUObject(this, &AASRCharacter::OnExecutionMontageEnd);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr)
	{
		AnimInstance->Montage_SetEndDelegate(LMontageEnded, ExecutionMontage);
	}

}

void AASRCharacter::Input_ToggleLockOn(const FInputActionValue& Value)
{
	if (TargetingComp != nullptr)
	{
		if (TargetingComp->bIsTargeting)
		{
			TargetingComp->ClearTarget();
		}
		else
		{
			TargetingComp->FindTarget();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL TargetingComponent"));
	}
}

void AASRCharacter::Input_Execution(const FInputActionValue& Value)
{
	if (CanExecution())
	{
		Execution();
	}
}



void AASRCharacter::SetHealth(float NewHealth)
{
	NewHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);
	if (Health != NewHealth)
	{
		Health = NewHealth;
		OnHealthChanged.Broadcast();
	}
}

void AASRCharacter::SetStamina(float NewStamina)
{
	NewStamina = FMath::Clamp(NewStamina, 0.f, MaxStamina);
	if (Stamina != NewStamina)
	{
		// Use Stamina
		if (NewStamina < Stamina)
		{
			float StaminaRegenCooldown = NewStamina == 0 ? 2.5f : 1.5f;
			GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &AASRCharacter::RegenStamina, StaminaRegenInterval, true, StaminaRegenCooldown);
		}
		Stamina = NewStamina;
		OnStaminaChanged.Broadcast();
	}
}

void AASRCharacter::SetExecutionCamera()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->SetViewTargetWithBlend(GetExecutionCameraManager()->GetChildActor(), 0.3f, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.0f, false);
}

void AASRCharacter::ResetState()
{
	if (CombatState != ECombatState::ECS_Death)
	{
		SetCombatState(ECombatState::ECS_None);
	}
}


void AASRCharacter::SphereTrace(float TraceDistance, float TraceRadius, const FHitData& HitData, ECollisionChannel CollisionChannel, bool bDrawDebugTrace)
{
	HitActors.Empty();
	TArray<FHitResult> HitResults;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * TraceDistance;


	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(CollisionChannel));



	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this, GetActorLocation(), TraceEnd, TraceRadius, ObjectTypes, false, TArray<AActor*>(),
		bDrawDebugTrace? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();

			// Check Duplicated Hit
			if (HitActor != nullptr && !HitActors.Contains(HitActor))
			{
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(HitActor);
				if (CombatInterface != nullptr)
				{
					CombatInterface->GetHit(HitResult, this, HitData);
					HitActors.AddUnique(HitActor);
				}
			}
		}
	}
}

bool AASRCharacter::CanExecution() const
{
	if (TargetingComp == nullptr || TargetingComp->GetTargetActor() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Comp Execution"));
		return false;
	}

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(TargetingComp->GetTargetActor());
	if (Enemy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Enemy Execution"));
		return false;
	}

	if (!Enemy->CanBeExecuted() || GetDistanceTo(Enemy) > ExecutionDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Distance or CanExecution Execution"));
		return false;
	}

	return !bIsExecuting;
}

void AASRCharacter::ResetCamera()
{
	// Execution Camera To Follow Camera
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APawn* ControlledPawn = PlayerController->GetPawn();
	if (PlayerController != nullptr && this == ControlledPawn)
	{
		PlayerController->SetViewTargetWithBlend(FollowCameraManager->GetChildActor(), 0.5f, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.f, false);
	}
}

EHitDirection AASRCharacter::GetHitDirection(const FVector AttackerLocation) const
{
	FVector Direction = (AttackerLocation - GetActorLocation()).GetSafeNormal();
	FRotator AttackerRotator = UKismetMathLibrary::MakeRotFromX(Direction);
	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(AttackerRotator, GetActorRotation()); 
	float HitDirectionYaw = DeltaRotator.Yaw;

	if (HitDirectionYaw >= -90.0f && HitDirectionYaw <= 90.0f)
	{
		return EHitDirection::EHD_Front;
	}
	// TODO - Make 4-Directional Animation
	
	//else if (HitDirectionYaw > 45.0f && HitDirectionYaw <= 135.0f)
	//{
	//	return EHitDirection::EHD_Right;
	//}
	//else if (HitDirectionYaw < -45.0f && HitDirectionYaw >= -135.0f)
	//{
	//	return EHitDirection::EHD_Left;
	//}
	else
	{
		return EHitDirection::EHD_Back;
	}
}


void AASRCharacter::OnExecutionMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	bIsExecuting = false;
}

FDamageTypeMapping* AASRCharacter::FindDamageDTRow(EASRDamageType DamageType) const
{
	FDamageInfoData* DamageInfoData = nullptr;
	FText RowText;
	UEnum::GetDisplayValueAsText(DamageType, RowText);

	UE_LOG(LogTemp, Warning, TEXT("Find DT Row TEXT: %s"), *RowText.ToString());


	if (DamageDataTable != nullptr)
	{
		DamageInfoData = DamageDataTable->FindRow<FDamageInfoData>(*RowText.ToString(), FString::Printf(TEXT("Failed to Find: [%s] %s"), *GetName(), *RowText.ToString()));
		if (DamageInfoData == nullptr)
		{
			DamageInfoData = DamageDataTable->FindRow<FDamageInfoData>(FName("Default"), FString::Printf(TEXT("Failed to Find Default: [%s] %s"), *GetName(), *RowText.ToString()));
		}


		return &DamageInfoData->DamageReaction;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL DT!"));

	}
	return nullptr;
}

void AASRCharacter::HandleDeath()
{
	SetCombatState(ECombatState::ECS_Death);

	// TODO
	PlayAnimMontage(StandingDeathMontage);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController != nullptr)
	{
		DisableInput(PlayerController);
	}
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bUseControllerRotationYaw = false;
}

void AASRCharacter::RegenStamina()
{
	SetStamina(Stamina + StaminaRegenRate * StaminaRegenInterval);
}

void AASRCharacter::SetCombatState(ECombatState InCombatState)
{
	if (InCombatState != CombatState)
	{
		CombatState = InCombatState;
		OnCombatStateChanged.Broadcast(InCombatState);
	}
}

void AASRCharacter::SetStrafe(bool bEnableStrafe)
{
	bIsStrafe = bEnableStrafe;
	GetCharacterMovement()->bOrientRotationToMovement = !bEnableStrafe;
	GetCharacterMovement()->bUseControllerDesiredRotation = bEnableStrafe;

	GetCharacterMovement()->MaxWalkSpeed = bEnableStrafe ? MaxStrafeSpeed : MaxWalkSpeed;
}

UCameraComponent* AASRCharacter::GetFollowCamera() const
{
	return FollowCameraManager->GetChildActor()->FindComponentByClass<UCameraComponent>();
}

UCameraComponent* AASRCharacter::GetExecutionCamera() const
{
	return ExecutionCameraManager->GetChildActor()->FindComponentByClass<UCameraComponent>();
}

void AASRCharacter::Jump()
{
	if (CombatState == ECombatState::ECS_None)
	{
		if (bIsCrouched)
		{
			UnCrouch();
		}
		else
		{
			Super::Jump();
		}
	}
	else
	{
		StopJumping();
	}
}

void AASRCharacter::Guard()
{
	if (CanGuard())
	{
		SetCombatState(ECombatState::ECS_Guard);

		ResetNormalAttack();
		ResetSkills();
		ResetDodge();

		//FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();
		//if (LastInputVector.Size() != 0.f)
		//{
		//	SetActorRotation(UKismetMathLibrary::MakeRotFromX(LastInputVector));
		//}

		SetStamina(Stamina - 100.f);
		PlayAnimMontage(GuardMontage);
	}
}

bool AASRCharacter::CanGuard() const
{
	if (CombatState != ECombatState::ECS_Attack && CombatState != ECombatState::ECS_Guard
		&& CombatState != ECombatState::ECS_Death && !GetCharacterMovement()->IsFalling() && Stamina > 0)
	{
		return true;
	}
	return false;
}


bool AASRCharacter::GetHit(const FHitResult& HitResult, AActor* Attacker, const FHitData& HitData)
{	
	// Dead
	if (CombatState == ECombatState::ECS_Death)
	{
		return false;
	}

	if (bIsInvulnerable)
	{
		// Optional : Add Dodge Succeed Effects
		return false;
	}

	if (Cast<ABaseEnemy>(Attacker) == nullptr)
	{
		return false;
	}

	// Guard
	if (CombatState == ECombatState::ECS_Guard && IsAttackFromFront(HitResult))
	{
		if (HitReactionState == EHitReactionState::EHR_Parry)
		{
			SetCombatState(ECombatState::ECS_Attack);
			SetHitReactionState(EHitReactionState::EHR_None);
			SetStamina(Stamina + 100.f);
			if (Cast<ABaseEnemy>(Attacker) != nullptr)
			{
				ABaseEnemy* Enemy = Cast<ABaseEnemy>(Attacker);
				UE_LOG(LogTemp, Warning, TEXT("Stamina: %f"), Enemy->Stamina);
				Enemy->SetStamina(Enemy->Stamina - 50.f);
				UE_LOG(LogTemp, Warning, TEXT("Stamina: %f"), Enemy->Stamina);
			}
			// TODO - Divide Parry System and Parry Counter System
			PlayAnimMontage(ParryCounterMontage, 1.f);
			return true;
		}
		else
		{
			FVector KnockbackForce = -GetActorForwardVector() * HitData.Damage * 20;
			UE_LOG(LogTemp, Warning, TEXT("KnockbackForce: %s"), *KnockbackForce.ToString());
			LaunchCharacter(KnockbackForce, true, false);
			SetStamina(Stamina - 50.f);
			PlayAnimMontage(GuardAcceptMontage, 1.f, "Guard");
			return true;
		}

	}

	// Apply Damage
	SetHealth(Health - HitData.Damage);
	UE_LOG(LogTemp, Warning, TEXT("HEALTH: %f"), Health);


	// Effects
	SpawnEffects(HitData, HitResult);

	// Handle Death
	if (Health <= 0 && !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
	{
		HandleDeath();
		return true;
	}

	// Play Hit React Animation
	PlayHitAnimation(HitData, Attacker);
	return true;
	
}

void AASRCharacter::SpawnEffects(const FHitData& HitData, const FHitResult& HitResult)
{
	if (HitData.HitSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this,
			HitData.HitSound, GetActorLocation());
	}
	if (HitData.HitEffect != nullptr)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HitData.HitEffect,
			HitResult.ImpactPoint,
			HitData.HitEffectRotation,
			HitData.HitEffectScale
		);
	}
	else if (HitData.HitParticleEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitData.HitParticleEffect,
			HitResult.ImpactPoint,
			HitData.HitEffectRotation,
			HitData.HitEffectScale
		);
	}
}

void AASRCharacter::PlayHitAnimation(const FHitData& HitData, AActor* Attacker)
{
	FDamageTypeMapping* DamageMapping = nullptr;
	DamageMapping = FindDamageDTRow(HitData.DamageType);
	if (DamageMapping != nullptr)
	{
		SetCombatState(DamageMapping->CombatState);
		UAnimMontage* LoadedMontage = DamageMapping->HitReactionMontage;
		if (LoadedMontage != nullptr)
		{
			EHitDirection HitDirection = GetHitDirection(Attacker->GetActorLocation());
			FName SectionName = "Front";
			switch (HitDirection)
			{
			case EHitDirection::EHD_Back:
				SectionName = "Back";
				break;
			case EHitDirection::EHD_Left:
				SectionName = "Left";
				break;
			case EHitDirection::EHD_Right:
				SectionName = "Right";
				break;
			case EHitDirection::EHD_Front:
			case EHitDirection::EHD_MAX:
			default:
				break;
			}
			if (LoadedMontage->IsValidSectionName(SectionName))
			{
				PlayAnimMontage(LoadedMontage, 1.f, SectionName);
			}
			else
			{
				PlayRandomSection(LoadedMontage);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to Load Animation Montage"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL DamageType Mapping!"));
	}
}

bool AASRCharacter::IsDead() const
{
	return CombatState == ECombatState::ECS_Death;
}

ECombatState AASRCharacter::GetCombatState() const
{
	return CombatState;
}

EHitReactionState AASRCharacter::GetHitReactionState() const
{
	return HitReactionState;
}

void AASRCharacter::SetHitReactionState(EHitReactionState NewState)
{
	HitReactionState = NewState;
}

bool AASRCharacter::ReserveAttackTokens(int32 Amount)
{
	if (AttackTokensCount >= Amount)
	{
		AttackTokensCount -= Amount;
		return true;
	}
	return false;
}

void AASRCharacter::ReturnAttackTokens(int32 Amount)
{
	AttackTokensCount += Amount;
}

UDataTable* AASRCharacter::GetAttackDataTable() const
{
	return AttackDataTable;
}

void AASRCharacter::ApplyHitStop(float Duration, float TimeDilation)
{
	CustomTimeDilation = 0.f;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AASRCharacter::ResetTimeDilation, Duration, false);
}

void AASRCharacter::ResetTimeDilation()
{
	CustomTimeDilation = 1.0f;
}
