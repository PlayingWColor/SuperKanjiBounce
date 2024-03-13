// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SphereComponent.h"


#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SuperBallPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleBallDelegate);

class APickup;

UCLASS()
class SUPERKANJIBOUNCE_API ASuperBallPlayer : public APawn
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Sets default values for this pawn's properties
	ASuperBallPlayer();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APickup> PickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TArray<int32> Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TArray<FText> KanjiCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	int32 CurrentlySelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	int32 Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics, meta = (AllowPrivateAccess = "true"))
	float JumpDelayLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics, meta = (AllowPrivateAccess = "true"))
	float JumpForceAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics, meta = (AllowPrivateAccess = "true"))
	float MaxMoveVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics, meta = (AllowPrivateAccess = "true"))
	float MoveAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics, meta = (AllowPrivateAccess = "true"))
	float MaxMoveVelocityAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics, meta = (AllowPrivateAccess = "true"))
	float MoveAccelerationAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraFramePadding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float ExtendAmountFromVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float PredictAmountFromVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraLerpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SkinSphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	FVector CollisionNormal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	bool IsColliding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	float NotCollidingJumpTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	float JustCollidedJumpTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	FVector LastHitPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float TargetCameraDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FVector TargetCameraOffset;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(float Value);
	 
	void Jump();

	void PerformJump();

	void SelectPickupRight();

	void SelectPickupLeft();

	void Drop();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void PerformPickup(FText pickupID);

	UPROPERTY(BlueprintAssignable, Category = "Super Ball Events")
	FSimpleBallDelegate OnJump;

	UPROPERTY(BlueprintAssignable, Category = "Super Ball Events")
	FSimpleBallDelegate OnBounce;

	UPROPERTY(BlueprintAssignable, Category = "Super Ball Events")
	FSimpleBallDelegate OnSelect;

	UPROPERTY(BlueprintAssignable, Category = "Super Ball Events")
	FSimpleBallDelegate OnGrabPickup;

	UPROPERTY(BlueprintAssignable, Category = "Super Ball Events")
	FSimpleBallDelegate OnDropPickup;

	UPROPERTY(BlueprintAssignable, Category = "Super Ball Events")
	FSimpleBallDelegate OnSubmitPickup;
};
