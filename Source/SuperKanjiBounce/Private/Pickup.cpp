// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "SuperBallPlayer.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	if (SphereComponent != nullptr)
	{
		SphereComponent->InitSphereRadius(50);
		SphereComponent->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);

		SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);

		SphereComponent->CanCharacterStepUpOn = ECB_No;

		SphereComponent->SetupAttachment(RootComponent);

		StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
		if (StaticMeshComponent != nullptr)
		{
			StaticMeshComponent->bCastDynamicShadow = false;
			StaticMeshComponent->bAffectDynamicIndirectLighting = true;
			StaticMeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
			StaticMeshComponent->SetupAttachment(SphereComponent);
			StaticMeshComponent->SetGenerateOverlapEvents(false);
			StaticMeshComponent->SetCanEverAffectNavigation(false);


			// Load cube static mesh from package
			static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereStaticMesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
			//if (SphereStaticMesh.Object != nullptr)
			//{
			StaticMeshComponent->SetStaticMesh(SphereStaticMesh.Object);
			//}
		}
	}

	Initialized = false;
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnCollision);

	Player = Cast<ASuperBallPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Log, TEXT("%s, %s, %f"), Initialized ? "initialized" : "not initialized", Player == nullptr ? "exists" : "null", FVector::Distance(Player->GetActorLocation(), GetActorLocation()) > 200);

	if (!Initialized)
	{
		if (Player == nullptr)
		{
			Player = Cast<ASuperBallPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
			UE_LOG(LogTemp, Log, TEXT("PlayerIsNull"));
		}
		if (Player != nullptr && FVector::Distance(Player->GetActorLocation(), GetActorLocation()) > 200)
		{
			Initialized = true;
		}
	}
}

void APickup::Initialize(ASuperBallPlayer* PlayerPointer, const FText& NewTextID)
{
	Player = PlayerPointer;
	TextID = NewTextID;
	Initialized = true;
}

void APickup::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//ASuperBallPlayer* Player = Cast<ASuperBallPlayer>(OtherActor);

	if (Initialized == true && Player != nullptr && Player->KanjiCollection.Num() < 9)
	{
		Player->PerformPickup(TextID);

		SphereComponent->OnComponentBeginOverlap.RemoveAll(this);
		StaticMeshComponent->SetHiddenInGame(true);
		this->Destroy();
	}

}