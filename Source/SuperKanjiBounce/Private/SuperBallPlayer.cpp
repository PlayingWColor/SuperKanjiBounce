// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperBallPlayer.h"
#include "Pickup.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASuperBallPlayer::ASuperBallPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	if (SphereComponent != nullptr)
	{
		SphereComponent->InitSphereRadius(50);

		SphereComponent->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SphereComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
		SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

		SphereComponent->CanCharacterStepUpOn = ECB_No;

		RootComponent = SphereComponent;


		SkinSphereComponent = CreateDefaultSubobject<USphereComponent>("SkinSphereComponent");
		if (SkinSphereComponent != nullptr)
		{
			SkinSphereComponent->InitSphereRadius(51);
			SkinSphereComponent->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);

			SkinSphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			SkinSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

			SkinSphereComponent->CanCharacterStepUpOn = ECB_No;

			SkinSphereComponent->SetupAttachment(RootComponent);
		}


		StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
		if (StaticMeshComponent != nullptr)
		{
			StaticMeshComponent->bCastDynamicShadow = true;
			StaticMeshComponent->bAffectDynamicIndirectLighting = true;
			StaticMeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
			StaticMeshComponent->SetupAttachment(RootComponent);
			StaticMeshComponent->SetGenerateOverlapEvents(false);
			StaticMeshComponent->SetCanEverAffectNavigation(false);


			// Load cube static mesh from package
			static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereStaticMesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
			//if (SphereStaticMesh.Object != nullptr)
			//{
				StaticMeshComponent->SetStaticMesh(SphereStaticMesh.Object);
			//}
		}

		// Create a camera boom (pulls in towards the player if there is a collision)
		CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
		CameraBoom->SetupAttachment(SphereComponent);
		CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
		CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

		// Create a follow camera
		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
		FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
		FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	}

	static ConstructorHelpers::FClassFinder<APickup> PickupClassFinder(TEXT("/Game/BP_Pickup"));
	PickupClass = PickupClassFinder.Class;

	KanjiCollection.Init(FText(), 0);
	CurrentlySelected = 0;
	Score = 0;

	JumpDelayLimit = 0.2f;
	CollisionNormal = FVector::UpVector;
	JumpForceAmount = 500.0f;

	MaxMoveVelocity = 2000.0f;
	MoveAcceleration = 1000.0f;

	MaxMoveVelocityAir = 2000.0f;
	MoveAccelerationAir = 500.0f;

	ExtendAmountFromVelocity = 1.0f;
	CameraFramePadding = 2000.0f;
	PredictAmountFromVelocity = 0.1f;
	CameraLerpSpeed = 1.0f;
}

// Called when the game starts or when spawned
void ASuperBallPlayer::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->SetMassOverrideInKg(NAME_None, 1.0f, true);

	SphereComponent->OnComponentHit.AddDynamic(this, &ASuperBallPlayer::OnHit);
	SkinSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASuperBallPlayer::OnCollision);
	SkinSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ASuperBallPlayer::OnEndCollision);

	SphereComponent->SetShouldUpdatePhysicsVolume(true);
	SphereComponent->SetCanEverAffectNavigation(false);
	SphereComponent->SetSimulatePhysics(true);
	SphereComponent->SetNotifyRigidBodyCollision(true);

}

// Called every frame
void ASuperBallPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(OUT Hit, GetActorLocation(), GetActorLocation() + FVector::DownVector * 100000, FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic));

	FVector GroundPosition = Hit.Location + FVector::UpVector*500;
	FVector LocalizedCenterByGroundPosition = (GroundPosition + GetActorLocation())/2 - GetActorLocation();

	FVector HorizontalVelocity = SphereComponent->GetPhysicsLinearVelocity();
	HorizontalVelocity = FVector(0, HorizontalVelocity.Y, 0);

	//set camera distance
	float DistanceToExtend = CameraFramePadding + Hit.Distance/1.333f;

	float DistanceFromVelocity = ExtendAmountFromVelocity * HorizontalVelocity.Size();

	DistanceToExtend += DistanceFromVelocity;

	TargetCameraDistance = DistanceToExtend / 2 * FMath::Tan(FollowCamera->FieldOfView / FollowCamera->AspectRatio);
	TargetCameraOffset = HorizontalVelocity * PredictAmountFromVelocity + LocalizedCenterByGroundPosition;

	CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, TargetCameraDistance, DeltaTime * CameraLerpSpeed);
	FollowCamera->SetRelativeLocation(FMath::Lerp(FollowCamera->GetRelativeLocation(), TargetCameraOffset, DeltaTime * CameraLerpSpeed));



	if (IsColliding)
	{
		if (HorizontalVelocity.Size() > MaxMoveVelocity)
		{
			FVector NewVelocity = SphereComponent->GetPhysicsLinearVelocity();
			NewVelocity.Y = HorizontalVelocity.Y / HorizontalVelocity.Size() * MaxMoveVelocity;
			SphereComponent->SetPhysicsLinearVelocity(NewVelocity);
		}
	}
	else
	{
		if (HorizontalVelocity.Size() > MaxMoveVelocityAir)
		{
			FVector NewVelocity = SphereComponent->GetPhysicsLinearVelocity();
			NewVelocity.Y = HorizontalVelocity.Y / HorizontalVelocity.Size() * MaxMoveVelocityAir;
			SphereComponent->SetPhysicsLinearVelocity(NewVelocity);
		}
	}
}

// Called to bind functionality to input
void ASuperBallPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASuperBallPlayer::Jump);
	PlayerInputComponent->BindAxis("Move", this, &ASuperBallPlayer::Move);

	PlayerInputComponent->BindAction("SelectLeft", IE_Pressed, this, &ASuperBallPlayer::SelectPickupLeft);
	PlayerInputComponent->BindAction("SelectRight", IE_Pressed, this, &ASuperBallPlayer::SelectPickupRight);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &ASuperBallPlayer::Drop);
}

void ASuperBallPlayer::PerformJump()
{
	UE_LOG(LogTemp, Log, TEXT("Try to Perform Jump"));
	
	FVector BiasedNormal = (CollisionNormal + FVector::UpVector);
	BiasedNormal.Normalize();

	SphereComponent->AddImpulse(BiasedNormal * JumpForceAmount, NAME_None, true);

	if(OnJump.IsBound())
		OnJump.Broadcast();
}

void ASuperBallPlayer::Jump()
{
	UE_LOG(LogTemp, Log, TEXT("Jump"));
	
	if (IsColliding)
	{
		UE_LOG(LogTemp, Log, TEXT("Will Peform Jump from Jump"));
		
		PerformJump();
	}
	else
	{
		NotCollidingJumpTime = UGameplayStatics::GetUnpausedTimeSeconds(GetWorld());
		if (NotCollidingJumpTime - JustCollidedJumpTime <= JumpDelayLimit / 2)
		{
			UE_LOG(LogTemp, Log, TEXT("Will Peform Jump from Jump due to Recent Collision"));
			PerformJump();
		}
	}
}

void ASuperBallPlayer::Move(float Value)
{
	//UE_LOG(LogTemp, Log, TEXT("Move: %f"), Value);
	
	FVector HorizontalVelocity = SphereComponent->GetComponentVelocity();
	HorizontalVelocity = FVector(0, HorizontalVelocity.Y, 0);
	
	if (IsColliding)
	{
		SphereComponent->AddForce(FVector::RightVector * Value * MoveAcceleration, NAME_None, true);
		

	}
	else
	{
		SphereComponent->AddForce(FVector::RightVector * Value * MoveAccelerationAir, NAME_None, true);
		
		if (HorizontalVelocity.Size() > MaxMoveVelocityAir)
		{
			//UE_LOG(LogTemp, Log, TEXT("Force to add: %f"), Value * MoveAccelerationAir);

		}
	}
}

void ASuperBallPlayer::SelectPickupRight()
{
	if (KanjiCollection.Num() == 0)
		return;

	if (CurrentlySelected < KanjiCollection.Num() - 1)
		CurrentlySelected++;
	else
		CurrentlySelected = 0;

	if (OnSelect.IsBound())
		OnSelect.Broadcast();
}

void ASuperBallPlayer::SelectPickupLeft()
{
	if (KanjiCollection.Num() == 0)
		return;
	
	if (CurrentlySelected == 0)
		CurrentlySelected = KanjiCollection.Num() - 1;
	else
		CurrentlySelected--;

	if (OnSelect.IsBound())
		OnSelect.Broadcast();
}

void ASuperBallPlayer::Drop()
{
	if (PickupClass == nullptr || KanjiCollection.Num() == 0)
		return;

	APickup* NewPickup = GetWorld()->SpawnActor<APickup>(PickupClass, GetActorLocation(), FRotator::ZeroRotator);
	if (NewPickup != nullptr)
		NewPickup->Initialize(this, KanjiCollection[CurrentlySelected]);

	KanjiCollection.RemoveAt(CurrentlySelected);
	if (KanjiCollection.Num() != 0 && CurrentlySelected >= KanjiCollection.Num())
		CurrentlySelected = KanjiCollection.Num() - 1;

	if (OnDropPickup.IsBound())
		OnDropPickup.Broadcast();
}

void ASuperBallPlayer::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("OnCollision"));
	
	CollisionNormal = SweepResult.Normal;
	IsColliding = true;

	if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_WorldStatic && OnBounce.IsBound())
		OnBounce.Broadcast();
}

void ASuperBallPlayer::OnEndCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Log, TEXT("OnEndCollision"));
	
	IsColliding = false;
}

void ASuperBallPlayer::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Log, TEXT("OnHit"));
	
	CollisionNormal = Hit.Normal;
	LastHitPoint = Hit.ImpactPoint;

	JustCollidedJumpTime = UGameplayStatics::GetUnpausedTimeSeconds(GetWorld());

	if (JustCollidedJumpTime - NotCollidingJumpTime <= JumpDelayLimit/2)
	{
		UE_LOG(LogTemp, Log, TEXT("Will Peform Jump from Collision"));
		PerformJump();
	}
}

void ASuperBallPlayer::PerformPickup(FText pickupID)
{
	KanjiCollection.Emplace(pickupID);

	if (OnGrabPickup.IsBound())
		OnGrabPickup.Broadcast();
}