// Copyright Epic Games, Inc. All Rights Reserved.


#include "GgGameModeBase.h"
#include "GgPlayerController.h"
#include "GgGameInstance.h"
#include "Actor/GgCharacterPC.h"
#include "Manager/GgObjectManager.h"
#include "Manager/GgCameraManager.h"
#include "Manager/GgLockOnManager.h"
#include "Manager/GgUIManager.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AGgGameModeBase::AGgGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerControllerClass = AGgPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass( TEXT( "/Game/Blueprint/Player/BP_Reva" ) );
	if( PlayerPawnBPClass.Class != NULL )
		DefaultPawnClass = PlayerPawnBPClass.Class;
}

void AGgGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GetGgUIManager().ShowUI( "WBP_GameUI" );
}

void AGgGameModeBase::BeginDestroy()
{
	Super::BeginDestroy();
}

void AGgGameModeBase::Tick( float InDeltaTime )
{
	Super::Tick( InDeltaTime );

	GetGgObjectManager().Tick( InDeltaTime );
	GetGgCameraManager().Tick( InDeltaTime );
	GetGgLockOnManager().Tick( InDeltaTime );
	GetGgUIManager().Tick( InDeltaTime );
}

