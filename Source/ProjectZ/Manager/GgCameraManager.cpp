
#include "GgCameraManager.h"
#include "Engine/World.h"
#include "Component/GgObjectComp.h"
#include "Component/GgMaterialComp.h"
#include "Actor/GgCharacterPC.h"
#include "System/GgAnimInstance.h"
#include "System/GgGameInstance.h"
#include "System/GgGameModeBase.h"
#include "Manager/GgLockOnManager.h"
#include "Manager/GgDataInfoManager.h"
//#include "ETC/CameraShakeEffect.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Actor.h"


FGgCameraManager::FGgCameraManager()
{
	
}

FGgCameraManager::~FGgCameraManager()
{
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 틱 함수
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgCameraManager::Tick( float InDeltaTime )
{
	_ProcessCameraArm( InDeltaTime );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 카메라 쉐이크를 실행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgCameraManager::CameraShake( AActor* InCaster, float InScale, bool InShakeByWeight, bool InShakeByIntensity )
{
	if( !GetGgGameInstance().IsValidLowLevel() )
		return;

	if ( !InCaster )
		return;
	
	APlayerController* controller = GetGgGameInstance().GetPlayerController();
	if( !controller )
		return;

	if( InShakeByWeight )
	{
		ACharacter* caster = Cast< ACharacter>( InCaster );
		if ( !caster )
			return;

		auto moveComponent = caster->GetCharacterMovement();
		if ( !moveComponent )
			return;

		auto objectComp = caster ? caster->FindComponentByClass<UGgObjectComp>() : nullptr;
		if( !objectComp )
			return;

		/*if( !moveComponent->IsFalling() && objectComp->IsHeavyWeight() )
			controller->ClientStartCameraShake( UCameraShakeEffect::StaticClass(), InScale );*/
	}
	else if( InShakeByIntensity )
	{
		auto matProperty = InCaster ? InCaster->FindComponentByClass<UGgMaterialComp>() : nullptr;
		if( !matProperty )
			return;

		//if( matProperty->IsHardIntensity() )
		//	controller->ClientStartCameraShake( UCameraShakeEffect::StaticClass(), InScale );
	}
	else
	{
		//controller->ClientStartCameraShake( UCameraShakeEffect::StaticClass(), InScale );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 카메라 암 로직을 수행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgCameraManager::_ProcessCameraArm( float InDeltaTime )
{
	AGgCharacterPC* player = GetGgGameInstance().GetMyPlayer();
	if( !player )
		return;

	UGgAnimInstance* animInstance = Cast< UGgAnimInstance >( player->GetMesh()->GetAnimInstance() );
	if( !animInstance )
		return;

	USpringArmComponent* cameraBoom = player->CameraBoom;
	if( !cameraBoom )
		return;

	if( animInstance->IsFly )
		cameraBoom->TargetArmLength = FMath::Lerp( cameraBoom->TargetArmLength, CONST::FLY_TARGET_ARM_LENGTH, InDeltaTime * 10.f );
	else
		cameraBoom->TargetArmLength = FMath::Lerp( cameraBoom->TargetArmLength, CONST::DEFAULT_TARGET_ARM_LENGTH, InDeltaTime * 5.f );

	if( GetGgLockOnManager().GetLockOnTarget() )
		cameraBoom->CameraRotationLagSpeed = CONST::LOCKON_CAMERA_ROTAION_LAG_SPEED;
	else
		cameraBoom->CameraRotationLagSpeed = 0.f;
}