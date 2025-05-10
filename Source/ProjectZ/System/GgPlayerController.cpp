

#include "GgPlayerController.h"
#include "Actor/GgCharacterPC.h"
#include "Component/GgCharacterComp.h"
#include "Component/GgMaterialComp.h"
#include "Component/GgWeaponComp.h"
#include "Manager/GgLockOnManager.h"
#include "Manager/GgObjectManager.h"
#include "Manager/GgDataInfoManager.h"
#include "System/GgAnimInstance.h"
#include "System/GgGameInstance.h"
#include "ETC/GgConst.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AGgPlayerController::AGgPlayerController()
{
	//bShowMouseCursor = true;
	//DefaultMouseCursor = EMouseCursor::Crosshairs;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

void AGgPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;

	MyPlayer = Cast<AGgCharacterPC>( GetPawn() );
	if( !MyPlayer )
		return;

	CharacterComp = MyPlayer->CharacterComp;
	WeaponComp    = MyPlayer->WeaponComp;

	check( InputComponent );
	InputComponent->BindAxis( "MoveForward", this, &AGgPlayerController::MoveForward );
	InputComponent->BindAxis( "MoveRight", this, &AGgPlayerController::MoveRight );
	InputComponent->BindAxis( "TurnRate", this, &AGgPlayerController::TurnAtRate );
	InputComponent->BindAxis( "LookUpRate", this, &AGgPlayerController::LookUpAtRate );

	InputComponent->BindAxis( "Turn", MyPlayer, &APawn::AddControllerYawInput );
	InputComponent->BindAxis( "LookUp", MyPlayer, &APawn::AddControllerPitchInput );

	InputComponent->BindAction( "Alt",      IE_Pressed, this, &AGgPlayerController::JumpStart );
	InputComponent->BindAction( "Alt",      IE_Released, this, &AGgPlayerController::JumpStop );
	InputComponent->BindAction( "Shift",    IE_Pressed, this, &AGgPlayerController::DashStart );
	InputComponent->BindAction( "Shift",    IE_Released, this, &AGgPlayerController::DashStop );

	InputComponent->BindAction( "Space",      IE_Pressed, this, &AGgPlayerController::ProcessSpace);
	InputComponent->BindAction( "Tab",        IE_Pressed, this, &AGgPlayerController::ProcessTab );
	InputComponent->BindAction( "F",          IE_Pressed, this, &AGgPlayerController::ProcessF );
	InputComponent->BindAction( "R",          IE_Pressed, this, &AGgPlayerController::ProcessR );
	InputComponent->BindAction( "LeftClick",  IE_Pressed, this, &AGgPlayerController::ProcessLeftMouse );
	InputComponent->BindAction( "RightClick", IE_Pressed, this, &AGgPlayerController::ProcessRightMouse );
	InputComponent->BindAction( "WheelClick", IE_Pressed, this, &AGgPlayerController::ProcessWheelClick );
	InputComponent->BindAction( "WheelUp",    IE_Pressed, this, &AGgPlayerController::ProcessWheelUp );
	InputComponent->BindAction( "WheelDown",  IE_Pressed, this, &AGgPlayerController::ProcessWheelDown );
	InputComponent->BindAction( "1",          IE_Pressed, this, &AGgPlayerController::Process1 );
	InputComponent->BindAction( "2",          IE_Pressed, this, &AGgPlayerController::Process2 );
	InputComponent->BindAction( "3",          IE_Pressed, this, &AGgPlayerController::Process3 );

	InputTypeAndFuncMap.Empty();
	InputTypeAndFuncMap.Add( EInputKeyType::LEFT_MOUSE,  bind( &AGgPlayerController::ProcessLeftMouse, this ) );
	InputTypeAndFuncMap.Add( EInputKeyType::RIGHT_MOUSE, bind( &AGgPlayerController::ProcessRightMouse, this ) );
	InputTypeAndFuncMap.Add( EInputKeyType::SPACE,       bind( &AGgPlayerController::ProcessSpace, this ) );
	InputTypeAndFuncMap.Add( EInputKeyType::Tab,         bind( &AGgPlayerController::ProcessTab, this ) );

	_ResetReadySkill();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Tick
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::Tick( float InDeltaTime )
{
	Super::Tick( InDeltaTime );

	_ProcessInputBothKey();
	_ProcessRotationRate();
	_ProcessReadySkill( InDeltaTime );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief MoveForward
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::MoveForward( float Value )
{
	if( !MyPlayer || !CharacterComp )
		return;

	UGgAnimInstance* animInstance = MyPlayer ? Cast<UGgAnimInstance>( MyPlayer->GetMesh()->GetAnimInstance() ) : nullptr;
	if( !animInstance )
		return;

	if( Value != 0.0f )
	{
		if( animInstance->AnimState    == EAnimState   ::IDLE_RUN ||
			animInstance->AnimState    == EAnimState   ::JUMP     ||
			animInstance->AnimSubState == EAnimSubState::MOVABLE  ||
			animInstance->AnimSubState == EAnimSubState::UPPER_LOWER_BLEND ||
			CharacterComp->IsMontageInitialTime() )
		{
			const FRotator Rotation = GetControlRotation();
			const FRotator YawRotation( 0, Rotation.Yaw, 0 );
			const FVector Direction = FRotationMatrix( YawRotation ).GetUnitAxis( EAxis::X );
			MyPlayer->AddMovementInput( Direction, Value );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief MoveRight
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::MoveRight( float Value )
{
	if( !MyPlayer || !CharacterComp )
		return;

	UGgAnimInstance* animInstance = MyPlayer ? Cast<UGgAnimInstance>( MyPlayer->GetMesh()->GetAnimInstance() ) : nullptr;
	if( !animInstance )
		return;

	if( Value != 0.0f )
	{
		if( animInstance->AnimState    == EAnimState   ::IDLE_RUN ||
			animInstance->AnimState    == EAnimState   ::JUMP     || 
			animInstance->AnimSubState == EAnimSubState::MOVABLE  ||
			animInstance->AnimSubState == EAnimSubState::UPPER_LOWER_BLEND ||
			CharacterComp->IsMontageInitialTime() )
		{
			const FRotator Rotation = GetControlRotation();
			const FRotator YawRotation( 0, Rotation.Yaw, 0 );
			const FVector Direction = FRotationMatrix( YawRotation ).GetUnitAxis( EAxis::Y );
			MyPlayer->AddMovementInput( Direction, Value );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief TurnAtRate
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::TurnAtRate( float Rate )
{
	if( MyPlayer )
		MyPlayer->AddControllerYawInput( Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief LookUpAtRate
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::LookUpAtRate( float Rate )
{
	if( MyPlayer )
		MyPlayer->AddControllerPitchInput( Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief JumpStart
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::JumpStart()
{
	if ( MyPlayer && CharacterComp && CharacterComp->GetAnimState() == EAnimState::IDLE_RUN )
		MyPlayer->Jump();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief JumpStart
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::JumpStop()
{
	if( MyPlayer )
		MyPlayer->StopJumping();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief DashStart
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::DashStart()
{
	if( CharacterComp && CharacterComp->GetAnimState() == EAnimState::IDLE_RUN )
		CharacterComp->SetIsDash( true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief DashStop
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::DashStop()
{
	if( CharacterComp )
		CharacterComp->SetIsDash( false );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 마우스 왼쪽 실행
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessLeftMouse()
{
	if( !WeaponComp )
		return;

	const auto& skillInfo = GetGgDataInfoManager().GetPlayerWeaponSkillInfos().Find( WeaponComp->GetCurWeaponNum() );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->L_SkillNum ) ? _ResetReadySkill() : _SetReadySkill( EInputKeyType::LEFT_MOUSE );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 마우스 오른쪽 실행
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessRightMouse()
{
	if( !WeaponComp )
		return;

	const auto& skillInfo = GetGgDataInfoManager().GetPlayerWeaponSkillInfos().Find( WeaponComp->GetCurWeaponNum() );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->R_SkillNum ) ? _ResetReadySkill() : _SetReadySkill( EInputKeyType::RIGHT_MOUSE );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 마우스 양쪽 클릭 실행
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessBothMouseDown()
{
	/*if( !MatComp || !MyPlayer )
		return;

	switch( MatComp->GetMatState() )
	{
		case EMaterialState::GRASS:
		{
			UGgAnimInstance* animInstance = Cast< UGgAnimInstance >( MyPlayer->GetMesh()->GetAnimInstance() );
			if( animInstance && animInstance->IsJump )
				animInstance->IsFly = true;
		}
		break;
		case EMaterialState::ROCK:
		{
			_SkillPlay( 14 );
		}
		break;
	}*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 마우스 양쪽 클릭 취소 실행
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessBothMouseUp()
{
	/*if( !MatComp || !MyPlayer )
		return;

	switch( MatComp->GetMatState() )
	{
		case EMaterialState::GRASS:
		{
			UGgAnimInstance* animInstance = Cast< UGgAnimInstance >( MyPlayer->GetMesh()->GetAnimInstance() );
			if( animInstance && animInstance->IsJump )
				animInstance->IsFly = false;
		}
		break;
	}*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 마우스 휠 클릭 실행
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessWheelClick()
{
	if( GetGgLockOnManager().GetLockOnTarget() )
	{
		GetGgLockOnManager().LockOnRelease();
		return;
	}

	GetGgLockOnManager().LockOnStart();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 마우스 휠 위로 실행
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessWheelUp()
{
	if ( GetGgLockOnManager().GetLockOnTarget() )
	{
		GetGgLockOnManager().LockOnStart( ELockOnMode::Prev );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 마우스 휠 아래로 실행
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessWheelDown()
{
	if ( GetGgLockOnManager().GetLockOnTarget() )
	{
		GetGgLockOnManager().LockOnStart( ELockOnMode::Next );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스페이스 키 실행 (구르기)
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessSpace()
{
	const auto& skillInfo = GetGgDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::SPACE );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->SkillNum ) ? _ResetReadySkill() : _SetReadySkill( EInputKeyType::SPACE );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스페이스 키 실행 ( 무기 던지기 )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessTab()
{
	if ( !WeaponComp )
		return;

	if ( WeaponComp->GetWeaponType() == EWeaponType::DEFAULT )
		return;

	const auto& skillInfo = GetGgDataInfoManager().GetPlayerWeaponSkillInfos().Find( WeaponComp->GetCurWeaponNum() );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->ThrowSkillNum ) ? _ResetReadySkill() : _SetReadySkill( EInputKeyType::Tab );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief F 키 실행 ( 재질 변경 )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessF()
{
	const auto& skillInfo = GetGgDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::F );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->SkillNum );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief R 키 실행 ( 젤리 돌아오기 )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::ProcessR()
{
	const auto& skillInfo = GetGgDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::R );
	if ( !skillInfo )
		return;

	//if ( MatComp && MatComp->GetMatState() != EMaterialState::JELLY )
	//{
	//	if ( _SkillPlay( skillInfo->SkillNum ) )
	//	{
	//		MatComp->SetMatState( nullptr, true );
	//	}
	//}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 1번키 실행 ( 무기 생성 )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::Process1()
{
	if( !WeaponComp )
		return;

	//if( !( WeaponComp->CanWeaponComp( EWeaponState::SWORD ) ) )
	//	return;

	const auto& skillInfo = GetGgDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::Num1 );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->SkillNum );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 2번키 실행 ( 무기 생성 )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::Process2()
{
	if( !WeaponComp )
		return;

	//if( !( WeaponComp->CanWeaponComp( EWeaponState::AXE ) ) )
	//	return;

	const auto& skillInfo = GetGgDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::Num2 );
	if( !skillInfo )
		return;

	_SkillPlay( skillInfo->SkillNum );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 3번키 실행 ( 무기 생성 )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::Process3()
{
	if( !WeaponComp )
		return;

	//if( !( WeaponComp->CanWeaponComp( EWeaponState::SPEAR ) ) )
	//	return;

	const auto& skillInfo = GetGgDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::Num3 );
	if( !skillInfo )
		return;

	_SkillPlay( skillInfo->SkillNum );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스킬을 재생한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool AGgPlayerController::_SkillPlay( int InSkillNum )
{
	if( !CharacterComp )
		return false;

	return CharacterComp->SkillPlay( InSkillNum );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 발동 대기중 스킬 초기화
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::_ResetReadySkill()
{
	ReadySkillFunc = nullptr;
	ReadySkillResetTime = 0.f;
	ReadySkillInputKey = EInputKeyType::MAX;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 발동 대기중 스킬 설정
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::_SetReadySkill( EInputKeyType InReadyInputKey )
{
	if( ReadySkillInputKey == InReadyInputKey )
		return;

	ReadySkillFunc = InputTypeAndFuncMap[ InReadyInputKey ];
	ReadySkillResetTime = CONST::READY_SKILL_DURATION;
	ReadySkillInputKey = InReadyInputKey;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 발동 대기중인 스킬 수행
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::_ProcessReadySkill( float InDeltaTime )
{
	// 타임아웃 초기화
	if( ReadySkillResetTime <= 0 )
	{
		_ResetReadySkill();
		return;
	}
	else
	{
		ReadySkillResetTime -= InDeltaTime;

		// 대기중인 스킬 발동
		if( ReadySkillFunc )
			ReadySkillFunc();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 회전 비율 값을 셋팅한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::_ProcessRotationRate()
{
	if( !MyPlayer )
		return;

	UGgAnimInstance* animInstance = Cast< UGgAnimInstance >( MyPlayer->GetMesh()->GetAnimInstance() );
	if( !animInstance )
		return;

	if( animInstance->IsFly )
	{
		MyPlayer->GetCharacterMovement()->RotationRate = FRotator( 0.0f, CONST::FLY_ROTATION_RATE, 0.0f );
		return;
	}
	else if( ReadySkillResetTime > 0 || CharacterComp->IsMontageInitialTime() )
	{
		MyPlayer->GetCharacterMovement()->RotationRate = FRotator( 0.0f, CONST::READY_SKILL_ROTATION_RATE, 0.0f );
		return;
	}

	MyPlayer->GetCharacterMovement()->RotationRate = FRotator( 0.0f, CONST::DEFAULT_ROTATION_RATE, 0.0f );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 다중 키 입력을 받는다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AGgPlayerController::_ProcessInputBothKey()
{
	if( !IsDownBothMoustButton && IsInputKeyDown( EKeys::LeftMouseButton ) && IsInputKeyDown( EKeys::RightMouseButton ) )
	{
		IsDownBothMoustButton = true;
		ProcessBothMouseDown();
	}
	else if( IsDownBothMoustButton && !IsInputKeyDown( EKeys::LeftMouseButton ) && !IsInputKeyDown( EKeys::RightMouseButton ) )
	{
		IsDownBothMoustButton = false;
		ProcessBothMouseUp();
	}
}
