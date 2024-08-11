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

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("Targeting"));
	TargetingComponent->Owner = this;

	CharacterState = EASRCharacterState::ECS_None;

}

void AASRCharacter::BeginPlay()
{
	Super::BeginPlay();
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

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AASRCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AASRCharacter::StopJumping);
	}
}

void AASRCharacter::Input_Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();

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
			AddControllerYawInput(LookVector.X);
			AddControllerPitchInput(LookVector.Y);
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

void AASRCharacter::ResetState()
{
	CharacterState = EASRCharacterState::ECS_None;
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
					HitInterface->GetHit(HitResult, BaseDamage, DamageType);
					HitActors.AddUnique(HitActor);
				}
			}
		}
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

void AASRCharacter::GetHit(const FHitResult& HitResult, float Damage, EASRDamageType DamageType)
{	// TODO
	FDamageTypeMapping* Mapping;
	Mapping = DamageTypeMappings.Find(DamageType);
	if (Mapping != nullptr)
	{
		CharacterState = Mapping->CharacterState;
		PlayAnimMontage(Mapping->HitReactionMontage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL DamageType Mapping!"));
	}
	

}