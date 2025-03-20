
#include "GgUIManager.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Actor/GgCharacterPC.h"
#include "System/GgAnimInstance.h"
#include "System/GgGameInstance.h"
#include "System/GgGameModeBase.h"
#include "Manager/GgLockOnManager.h"
#include "Manager/GgDataInfoManager.h"
//#include "ETC/CameraShakeEffect.h"
#include "UI/UIBase.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Actor.h"


FGgUIManager::FGgUIManager()
{
	
}

FGgUIManager::~FGgUIManager()
{
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 틱 함수
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgUIManager::Tick( float InDeltaTime )
{
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief UI를 생성하고 띄운다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
UUIBase* FGgUIManager::ShowUI( const FString& InPath )
{
	FString path = FString( TEXT( "/Game/UI/" ) ) + InPath;
	UClass* loadedClass = ConstructorHelpersInternal::FindOrLoadClass( path, UUserWidget::StaticClass() );
	if( !loadedClass )
		return nullptr;

	UUIBase* userWidget = CreateWidget<UUIBase>( &GetGgGameInstance(), loadedClass );
	if( !userWidget )
		return nullptr;

	userWidget->AddToViewport( 9999 );
	userWidget->OnCreated();

	return userWidget;
}