// Copyright Epic Games, Inc. All Rights Reserved.


#include "ProjectZGameModeBase.h"
#include "MyPlayerController.h"
#include "MyGameInstance.h"
#include "../Manager/UIManager.h"
#include "../Actor/CharacterPC.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AProjectZGameModeBase::AProjectZGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerControllerClass = AMyPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass( TEXT( "/Game/Blueprints/Player/BP_Reva" ) );
	if( PlayerPawnBPClass.Class != NULL )
		DefaultPawnClass = PlayerPawnBPClass.Class;
}

void AProjectZGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GetUIManager().ShowUI( "WBP_GameUI" );
}

void AProjectZGameModeBase::BeginDestroy()
{
	Super::BeginDestroy();
}

void AProjectZGameModeBase::Tick( float InDeltaTime )
{
	Super::Tick( InDeltaTime );

	UMyGameInstance* gameInstance = Cast<UMyGameInstance>( GetGameInstance() );
	if( !gameInstance )
		return;

	gameInstance->Tick( InDeltaTime );
}

