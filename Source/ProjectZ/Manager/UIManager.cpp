
#include "UIManager.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "../Component/ObjectComp.h"
#include "../Component/MaterialComp.h"
#include "../Actor/CharacterPC.h"
#include "../System/MyAnimInstance.h"
#include "../System/MyGameInstance.h"
#include "../System/ProjectZGameModeBase.h"
#include "../Manager/LockOnManager.h"
#include "../Manager/DataInfoManager.h"
#include "../ETC/CameraShakeEffect.h"
#include "../UI/UIBase.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Actor.h"


UIManager::UIManager()
{
	
}

UIManager::~UIManager()
{
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ƽ �Լ�
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UIManager::Tick( float InDeltaTime )
{
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief UI�� �����ϰ� ����.
/////////////////////////////////////////////////////////////////////////////////////////////////////
UUIBase* UIManager::ShowUI( const FString& InPath )
{
	FString path = FString( TEXT( "/Game/UI/" ) ) + InPath;
	UClass* loadedClass = ConstructorHelpersInternal::FindOrLoadClass( path, UUserWidget::StaticClass() );
	if( !loadedClass )
		return nullptr;

	UUIBase* userWidget = CreateWidget<UUIBase>( &GetMyGameInstance(), loadedClass );
	if( !userWidget )
		return nullptr;

	userWidget->AddToViewport( 9999 );
	userWidget->OnCreated();

	return userWidget;
}