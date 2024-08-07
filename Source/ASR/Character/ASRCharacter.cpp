// Fill out your copyright notice in the Description page of Project Settings.


#include "ASRCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"



// Sets default values
AASRCharacter::AASRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;


	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	
	// Action Game Movement
	GetCharacterMovement()->MaxWalkSpeed = 700.f;
	GetCharacterMovement()->JumpZVelocity = 600.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 16.f;
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SetupAttachment(GetMesh());

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AASRCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AASRCharacter::Look);
		EnhancedInputComponent->BindAction(ToggleCrouchAction, ETriggerEvent::Started, this, &AASRCharacter::ToggleCrouch);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AASRCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AASRCharacter::StopJumping);
	}
}

void AASRCharacter::Move(const FInputActionValue& Value)
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

void AASRCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void AASRCharacter::ToggleCrouch(const FInputActionValue& Value)
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

void AASRCharacter::Jump()
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

