// Fill out your copyright notice in the Description page of Project Settings.


#include "GgGameInstance.h"
#include "Manager/GgObjectManager.h"
#include "Manager/GgDataInfoManager.h"
#include "Manager/GgCameraManager.h"
#include "Manager/GgLockOnManager.h"
#include "Manager/GgUIManager.h"
#include "Actor/GgCharacterPC.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 에디터 시작시 O, 게임 시작 시 O
/////////////////////////////////////////////////////////////////////////////////////////////////////
UGgGameInstance::UGgGameInstance()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 에디터 종료시 O, 게임 종료 시 X
/////////////////////////////////////////////////////////////////////////////////////////////////////
UGgGameInstance::~UGgGameInstance()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 에디터 시작시 X, 게임 시작 시 O
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgGameInstance::Init()
{
	Super::Init();

	FGgObjectManager::CreateInstance();
	FGgDataInfoManager::CreateInstance();
	FGgCameraManager::CreateInstance();
	FGgLockOnManager::CreateInstance();
	FGgUIManager::CreateInstance();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 에디터 종료시 X, 게임 종료 시 O
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgGameInstance::Shutdown()
{
	Super::Shutdown();

	FGgObjectManager::DestroyInstance();
	FGgDataInfoManager::DestroyInstance();
	FGgCameraManager::DestroyInstance();
	FGgLockOnManager::DestroyInstance();
	FGgUIManager::DestroyInstance();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 자신의 플레이어를 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
AGgCharacterPC* UGgGameInstance::GetMyPlayer()
{
	UWorld* world = GetWorld();
	if( !world )
		return nullptr;

	APlayerController* controller = world->GetFirstPlayerController();
	if( !controller )
		return nullptr;

	AGgCharacterPC* player = Cast<AGgCharacterPC>( controller->GetPawn() );
	if( !player )
		return nullptr;

	return player;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 자신의 컨트롤러를 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
APlayerController* UGgGameInstance::GetPlayerController()
{
	UWorld* world = GetWorld();
	if( !world )
		return nullptr;

	APlayerController* controller = world->GetFirstPlayerController();
	if( !controller )
		return nullptr;

	return controller;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 인스턴스를 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
UGgGameInstance* UGgGameInstance::GetInstance()
{
	if( !GEngine ) 
		return nullptr;
	
#if WITH_EDITOR

	for( const FWorldContext& worldContext : GEngine->GetWorldContexts() )
	{
		if( worldContext.WorldType == EWorldType::PIE || 
			worldContext.WorldType == EWorldType::Game )
			return Cast< UGgGameInstance >( worldContext.OwningGameInstance );
	}
	return nullptr;

#else
	//if( GEngine )
	//	GEngine->AddOnScreenDebugMessage( -1, 3.0f, FColor::Yellow, "game" );

	UGameEngine* GameEngine = Cast< UGameEngine >( GEngine );
	if( !GameEngine )
		return nullptr;

	return Cast< UGgGameInstance >( GameEngine->GameInstance );

#endif
}