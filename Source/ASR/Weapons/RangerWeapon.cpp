// Fill out your copyright notice in the Description page of Project Settings.


#include "RangerWeapon.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Engine/SkeletalMeshSocket.h"
#include "BulletCase.h"

#include "ASR/Character/Ranger.h"
#include "ASR/Player/ASRPlayerController.h"

// Sets default values
ARangerWeapon::ARangerWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; 

	// Disalbe Collision on Client by Default
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponInfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidget"));
	WeaponInfoWidget->SetupAttachment(RootComponent); 


}

// Called when the game starts or when spawned
void ARangerWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())	
	{
		// Enable Collision if has Authority 
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ARangerWeapon::OnSphereBeginOverlap);
		CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ARangerWeapon::OnSphereEndOverlap);

		UE_LOG(LogTemp, Warning, TEXT("Binding Done!"));
	}
	if (WeaponInfoWidget != nullptr)
	{
		WeaponInfoWidget->SetVisibility(false);
	}
	
}

void ARangerWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	SetHUDAmmo();
}


void ARangerWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARanger* Ranger = Cast<ARanger>(OtherActor);
	if (Ranger != nullptr)
	{
		Ranger->SetOverlappedWeapon(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL Ranger Casting"));
	}
}

void ARangerWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARanger* Ranger = Cast<ARanger>(OtherActor);
	if (Ranger != nullptr)
	{
		Ranger->SetOverlappedWeapon(nullptr);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL WarComposer Casting"));
	}
}

void ARangerWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case ERangerWeaponState::EWS_Equipped:
		SetInfoWidgetVisibility(false);
		break;
	}
}

void ARangerWeapon::UseAmmo()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagazineCapacity);
	SetHUDAmmo();
}

void ARangerWeapon::SetHUDAmmo()
{
	OwnerRanger = OwnerRanger == nullptr ? Cast<ARanger>(GetOwner()) : OwnerRanger;
	PlayerController = PlayerController == nullptr ? Cast<AASRPlayerController>(OwnerRanger->Controller) : PlayerController;

	if (OwnerRanger != nullptr && PlayerController != nullptr)
	{
		PlayerController->SetRangerAmmo(Ammo);
	}
}

bool ARangerWeapon::IsOutOfAmmo() const
{
	return Ammo <= 0;
}

void ARangerWeapon::SetWeaponState(ERangerWeaponState State)
{
	WeaponState = State;

	switch (WeaponState)
	{
	case ERangerWeaponState::EWS_Equipped:
		CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetInfoWidgetVisibility(false);
		break;
	}

}


// Called every frame
void ARangerWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARangerWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARangerWeapon, WeaponState);
}

void ARangerWeapon::SetInfoWidgetVisibility(bool bVisible)
{
	if (WeaponInfoWidget != nullptr)
	{ 
		WeaponInfoWidget->SetVisibility(bVisible);
	}
}

void ARangerWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation != nullptr)
	{
		Mesh->PlayAnimation(FireAnimation, false);
	}
	if (CaseClass != nullptr)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = GetWeaponMesh()->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket != nullptr)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());

			UWorld* World = GetWorld();
			FRotator SocketRotator = SocketTransform.GetRotation().Rotator();
			SocketRotator.Pitch += FMath::RandRange(-10.f, 10.f);
			SocketRotator.Yaw += FMath::RandRange(-10.f, 10.f);

			if (World != nullptr)
			{
				World->SpawnActor<ABulletCase>(
					CaseClass, SocketTransform.GetLocation(), SocketRotator);
			}
		}
	}
	UseAmmo();
}

