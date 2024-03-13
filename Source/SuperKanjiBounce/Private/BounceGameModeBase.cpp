// Fill out your copyright notice in the Description page of Project Settings.


#include "BounceGameModeBase.h"
#include "SuperBallPlayer.h"

ABounceGameModeBase::ABounceGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}