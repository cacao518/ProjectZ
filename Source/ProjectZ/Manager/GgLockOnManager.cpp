
#include "GgLockOnManager.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "Component/GgObjectComp.h"
#include "Component/GgCharacterComp.h"
#include "Actor/GgCharacterPC.h"
#include "System/GgAnimInstance.h"
#include "System/GgGameInstance.h"
#include "System/GgGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Misc/OutputDeviceNull.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"


FGgLockOnManager::FGgLockOnManager()
{
	LockOnTarget = nullptr;

}

FGgLockOnManager::~FGgLockOnManager()
{
	LockOnTarget = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 틱 함수
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgLockOnManager::Tick( float InDeltaTime )
{
	_ProcessLockOn( InDeltaTime );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 락온 시작
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgLockOnManager::LockOnStart( ELockOnMode InMode )
{
	UWorld* world = GetGgGameInstance().GetWorld();
	if( !world )
		return;

	AGgCharacterPC* myPlayer = GetGgGameInstance().GetMyPlayer();
	if( !myPlayer )
		return;

	auto myPlayerCharComp = myPlayer->FindComponentByClass<UGgCharacterComp>();
	if( !myPlayerCharComp )
		return;

	FVector center = myPlayer->GetActorLocation();

	TArray<FOverlapResult> overlapResults;
	FCollisionQueryParams collisionQueryParam( NAME_None, false, myPlayer );
	bool bResult = world->OverlapMultiByChannel(
		overlapResults,
		center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeSphere( CONST::LOCKON_RANGE ),
		collisionQueryParam
	);

	if( !bResult )
		return;

	// Character만 고른다.
	TArray< ACharacter* > filteredCharacters;
	for ( const auto& result : overlapResults )
	{
		ACharacter* character = Cast< ACharacter >( result.GetActor() );
		if ( !character )
			continue;

		auto charComp = character->FindComponentByClass<UGgCharacterComp>();
		if ( !charComp )
			continue;

		// 같은 팀은 제외한다.
		if ( charComp->GetTeamType() == myPlayerCharComp->GetTeamType() )
			continue;

		filteredCharacters.AddUnique( character );
	}

	if ( filteredCharacters.IsEmpty() )
		return;

	ACharacter* selectLockOnTarget = nullptr;

	switch ( InMode )
	{
		case ELockOnMode::Default:
		{
			// 제일 가까운 적이 락온 되도록 정렬 후 락온
			filteredCharacters.Sort( [myPlayer]( const auto& A, const auto& B ){
				return A.GetDistanceTo( myPlayer ) < B.GetDistanceTo( myPlayer );
				} );

			selectLockOnTarget = ( *filteredCharacters.begin() );
			break;
		}
		case ELockOnMode::Prev:
		{
			// 락온 된 대상 이전 대상으로 다시 락온
			int32 index = filteredCharacters.IndexOfByPredicate( [this]( const ACharacter* InChar ){
				return InChar == LockOnTarget;
				} );

			if ( index != INDEX_NONE && filteredCharacters.IsValidIndex( index - 1) )
			{
				selectLockOnTarget = filteredCharacters[ index - 1 ];
				LockOnRelease();
			}
			break;
		}
		case ELockOnMode::Next:
		{
			// 락온 된 대상 다음 대상으로 다시 락온
			int32 index = filteredCharacters.IndexOfByPredicate( [this]( const ACharacter* InChar ){
				return InChar == LockOnTarget;
				} );

			if ( index != INDEX_NONE && filteredCharacters.IsValidIndex( index + 1 ) )
			{
				selectLockOnTarget = filteredCharacters[ index + 1 ];
				LockOnRelease();
			}
			break;
		}
	}

	if ( !selectLockOnTarget )
		return;

	if ( auto widgetComp = Cast<UWidgetComponent>( selectLockOnTarget->GetDefaultSubobjectByName( TEXT( "LockOnMarkComp" ) ) ) )
	{
		if ( auto userWidget = Cast<UUserWidget>( widgetComp->GetUserWidgetObject() ) )
		{
			FOutputDeviceNull ar;
			userWidget->CallFunctionByNameWithArguments( TEXT( "AnimStart" ), ar, NULL, true );
			userWidget->SetVisibility( ESlateVisibility::SelfHitTestInvisible );

			bUseInterp = true;
			LockOnTarget = selectLockOnTarget;
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 락온 해제
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgLockOnManager::LockOnRelease()
{
	if( auto widgetComp = Cast<UWidgetComponent>( LockOnTarget->GetDefaultSubobjectByName( TEXT( "LockOnMarkComp" ) ) ) )
	{
		if( auto userWidget = Cast<UUserWidget>( widgetComp->GetUserWidgetObject() ) )
			userWidget->SetVisibility( ESlateVisibility::Collapsed );
	}

	bUseInterp = false;
	LockOnTarget = nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 락온 기능 수행
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgLockOnManager::_ProcessLockOn( float InDeltaTime )
{
	if( !LockOnTarget.IsValid() )
		return;

	AGgCharacterPC* myPlayer = GetGgGameInstance().GetMyPlayer();
	if( !myPlayer )
		return;

	auto ownerGgObjectComp = myPlayer->FindComponentByClass<UGgCharacterComp>();
	if( !ownerGgObjectComp )
		return;

	auto otherGgObjectComp = LockOnTarget->FindComponentByClass<UGgObjectComp>();
	if( !otherGgObjectComp )
		return;

	bool targetDie = !LockOnTarget.IsValid() || otherGgObjectComp->GetIsDie();
	bool targetAway = LockOnTarget->GetDistanceTo( myPlayer ) > CONST::LOCKON_RANGE;
	bool ownerDie = !GetValid( myPlayer ) || ownerGgObjectComp->GetIsDie();
	
	if ( ownerDie || targetAway )
	{
		LockOnRelease();
		return;
	}

	if( targetDie )
	{
		LockOnRelease();
		LockOnStart(); // 락온 된 적이 죽을 경우, 근처 적을 다시 락온.
		return;
	}
	
	FRotator targetRot = UKismetMathLibrary::FindLookAtRotation( myPlayer->GetActorLocation(), LockOnTarget->GetActorLocation() );
	targetRot.Pitch = CONST::LOCKON_CAMERA_FIX_PITCH;

	// 락온 시작 시 보간을 사용하여 부드럽게 카메라를 이동한다.
	if( bUseInterp )
	{
		FRotator currentRot = myPlayer->GetController()->GetControlRotation();
		FRotator newRot = FMath::RInterpTo( currentRot, targetRot, InDeltaTime, CONST::LOCKON_START_ROTAION_SPEED );
		myPlayer->GetController()->SetControlRotation( newRot );

		if( newRot.Equals( currentRot, 1.0f ) )
		{
			bUseInterp = false;
		}
	}
	else
	{
		myPlayer->GetController()->SetControlRotation( targetRot );
	}
	
	// 락온 상태에서 스킬 사용 시 내 캐릭터가 적을 바라보게 할 것 인지 확인
	auto skillInfo = ownerGgObjectComp->GetCurSkillInfo();
	if( skillInfo && skillInfo->LockOnLookAt && ownerGgObjectComp->IsMontageInitialTime() )
		ownerGgObjectComp->LookAt( GetLockOnTarget() );
}