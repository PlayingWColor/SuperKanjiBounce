// Fill out your copyright notice in the Description page of Project Settings.


#include "DropZone.h"

#include "Pickup.h"
#include "SuperBallPlayer.h"

// Sets default values
ADropZone::ADropZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("SphereComponent");
	if (BoxComponent != nullptr)
	{
		BoxComponent->InitBoxExtent(FVector::OneVector*50);
		BoxComponent->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);

		BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

		BoxComponent->CanCharacterStepUpOn = ECB_No;

		BoxComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void ADropZone::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ADropZone::OnCollision);
}

// Called every frame
void ADropZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADropZone::CheckPickup()
{
	if (LatestPickup != nullptr && LatestPickup->TextID.EqualTo(TextID))
	{
		ASuperBallPlayer* Player = Cast<ASuperBallPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (Player != nullptr)
		{
			Player->Score++;
			if (Player->OnSubmitPickup.IsBound())
				Player->OnSubmitPickup.Broadcast();
		}
		LatestPickup->SphereComponent->OnComponentBeginOverlap.RemoveAll(this);
		LatestPickup->SphereComponent->SetHiddenInGame(true);
		LatestPickup->Destroy();
	}
}

void ADropZone::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APickup* Pickup = Cast<APickup>(OtherActor);
	if (Pickup != nullptr && Pickup != LatestPickup)
	{
		LatestPickup = Pickup;
		FTimerHandle UnusedHandle;
		GetWorldTimerManager().SetTimer(UnusedHandle, this, &ADropZone::CheckPickup, 0.01f, false);
	}
}

