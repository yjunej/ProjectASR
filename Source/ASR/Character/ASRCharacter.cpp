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



// Sets default values
AASRCharacter::AASRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Disable Side Walk by Default
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// Action Game Movement
	GetCharacterMovement()->MaxWalkSpeed = 700.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->RotationRate.Yaw = 900.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 16.f;
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetupAttachment(GetMesh());

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	FollowCameraManager = CreateDefaultSubobject<UChildActorComponent>(TEXT("FollowCameraManager"));
	FollowCameraManager->SetupAttachment(FollowCamera);

	ExecutionCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ExecutionCamera"));
	ExecutionCamera->SetupAttachment(RootComponent);

	ExecutionCameraManager = CreateDefaultSubobject<UChildActorComponent>(TEXT("ExecutionCameraManager"));
	ExecutionCameraManager->SetupAttachment(ExecutionCamera);

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("Targeting"));
	TargetingComponent->Owner = this;

	CharacterState = EASRCharacterState::ECS_None;

}

void AASRCharacter::PlayRandomSection(UAnimMontage* Montage)
{
	int32 NumSections = Montage->GetNumSections();
	PlayAnimMontage(Montage, 1.f, Montage->GetSectionName(FMath::RandRange(0, NumSections - 1)));
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

	SetHealth(MaxHealth);
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
		if (TargetingComponent == nullptr || !(TargetingComponent->bIsTargeting))
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
	if (CharacterState != EASRCharacterState::ECS_Attack && CharacterState != EASRCharacterState::ECS_Guard && CharacterState != EASRCharacterState::ECS_Dodge && CharacterState != EASRCharacterState::ECS_Flinching)
	{
		Guard();
	}
}

void AASRCharacter::Input_Release_Guard(const FInputActionValue& Value)
{
	bIsGuardPressed = false;
	if (CharacterState == EASRCharacterState::ECS_Guard)
	{
		CharacterState = EASRCharacterState::ECS_None;
		PlayAnimMontage(GuardMontage, 1.f, FName("GuardEnd"));
	}
}

bool AASRCharacter::CanAttack() const
{
	if (CharacterState != EASRCharacterState::ECS_Attack && CharacterState != EASRCharacterState::ECS_Dodge
		&& CharacterState != EASRCharacterState::ECS_Death && !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying()
		&& CharacterState != EASRCharacterState::ECS_Flinching)
	{
		return true;
	}
	return false;
}

bool AASRCharacter::CanDodge() const
{
	if (CharacterState != EASRCharacterState::ECS_Attack && CharacterState != EASRCharacterState::ECS_Dodge
		&& CharacterState != EASRCharacterState::ECS_Death && !GetCharacterMovement()->IsFalling() && CharacterState != EASRCharacterState::ECS_Flinching)
	{
		return true;
	}
	return false;
}

void AASRCharacter::Input_ToggleLockOn(const FInputActionValue& Value)
{
	if (TargetingComponent != nullptr)
	{
		if (TargetingComponent->bIsTargeting)
		{
			TargetingComponent->ClearTarget();
		}
		else
		{
			TargetingComponent->FindTarget();
		}
	}
}

void AASRCharacter::Input_Execution(const FInputActionValue& Value)
{
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

void AASRCharacter::ResetState()
{
	if (CharacterState != EASRCharacterState::ECS_Death)
	{
		CharacterState = EASRCharacterState::ECS_None;
	}
}

void AASRCharacter::SphereTrace(float End, float Radius, float BaseDamage, EASRDamageType DamageType, ECollisionChannel CollisionChannel, bool bDrawDebugTrace)
{
	HitActors.Empty();
	TArray<FHitResult> HitResults;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	FVector TraceEnd = GetActorLocation() + GetActorForwardVector() * End;


	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(CollisionChannel));



	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		this, GetActorLocation(), TraceEnd, Radius, ObjectTypes, false, TArray<AActor*>(),
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
				IHitInterface* HitInterface = Cast<IHitInterface>(HitActor);
				if (HitInterface != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, HitSoundCue, HitActor->GetActorLocation());
					HitInterface->GetHit(HitResult, this, BaseDamage, DamageType);
					HitActors.AddUnique(HitActor);

				}
			}
		}
	}
}

bool AASRCharacter::CanExecution() const
{
	if (TargetingComponent == nullptr || TargetingComponent->GetTargetActor() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Comp Execution"));
		return false;
	}

	ABaseEnemy* Enemy = Cast<ABaseEnemy>(TargetingComponent->GetTargetActor());
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

	return true;
}

void AASRCharacter::ResetCamera()
{
	// Execution Camera To Follow Camera
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController != nullptr)
	{
		PlayerController->SetViewTargetWithBlend(FollowCameraManager->GetChildActor(), 0.4f, EViewTargetBlendFunction::VTBlend_EaseInOut, 0.8f, false);
	}
	bIsExecuting = false;

}

void AASRCharacter::HandleDeath()
{
	CharacterState = EASRCharacterState::ECS_Death;

	// TODO
	PlayAnimMontage(StandingDeathMontage);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController != nullptr)
	{
		DisableInput(PlayerController);
	}
}

void AASRCharacter::SetCharacterState(EASRCharacterState InCharacterState)
{
	if (InCharacterState != CharacterState)
	{
		CharacterState = InCharacterState;
	}
}

void AASRCharacter::Jump()
{
	if (CharacterState == EASRCharacterState::ECS_None)
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
		SetCharacterState(EASRCharacterState::ECS_Guard);

		ResetLightAttack();
		ResetHeavyAttack();
		ResetFirstSkill();
		ResetDodge();

		// Rotate Before Dodge
		FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();
		if (LastInputVector.Size() != 0.f)
		{
			SetActorRotation(UKismetMathLibrary::MakeRotFromX(LastInputVector));
		}

		PlayAnimMontage(GuardMontage);
	}
}

bool AASRCharacter::CanGuard() const
{
	if (CharacterState != EASRCharacterState::ECS_Attack && CharacterState != EASRCharacterState::ECS_Guard
		&& CharacterState != EASRCharacterState::ECS_Death && !GetCharacterMovement()->IsFalling())
	{
		return true;
	}
	return false;
}


void AASRCharacter::GetHit(const FHitResult& HitResult, AActor* Attacker, float Damage, EASRDamageType DamageType)
{	
	// Dead
	if (CharacterState == EASRCharacterState::ECS_Death || bIsInvulnerable)
	{
		return;
	}

	// Gaurd
	if (CharacterState == EASRCharacterState::ECS_Guard && IsAttackFromFront(HitResult))
	{
		FVector KnockbackForce = -GetActorForwardVector() * Damage * 10;
		LaunchCharacter(KnockbackForce, true, true);
		PlayAnimMontage(GuardAcceptMontage);
		// TODO: Add Stability System
		return;
	}

	// Apply Damage
	SetHealth(Health - Damage);
	UE_LOG(LogTemp, Warning, TEXT("HEALTH: %f"), Health);

	// Handle Death
	if (Health <= 0 && !GetCharacterMovement()->IsFalling() && !GetCharacterMovement()->IsFlying())
	{
		HandleDeath();
		return;
	}

	// Select Hit React Animation
	FDamageTypeMapping* Mapping;
	Mapping = DamageTypeMappings.Find(DamageType);
	if (Mapping != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GetHitSoundCue, GetActorLocation());
		CharacterState = Mapping->CharacterState;
		PlayRandomSection(Mapping->HitReactionMontage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL DamageType Mapping!"));
	}
	
}