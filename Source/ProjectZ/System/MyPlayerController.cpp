

#include "MyPlayerController.h"
#include "../Component/CharacterComp.h"
#include "../Component/MaterialComp.h"
#include "../Component/WeaponComp.h"
#include "../Manager/LockOnManager.h"
#include "../Manager/ObjectManager.h"
#include "../Manager/DataInfoManager.h"
#include "../Actor/CharacterPC.h"
#include "../System/MyAnimInstance.h"
#include "../System/MyGameInstance.h"
#include "../ETC/SDB.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AMyPlayerController::AMyPlayerController()
{
	//bShowMouseCursor = true;
	//DefaultMouseCursor = EMouseCursor::Crosshairs;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;

	MyPlayer = Cast<ACharacterPC>( GetPawn() );
	if( !MyPlayer )
		return;

	CharacterComp   = MyPlayer->CharacterComp;
	MatComp      = MyPlayer->MatComp;
	WeaponComp   = MyPlayer->WeaponComp;

	check( InputComponent );
	InputComponent->BindAxis( "MoveForward", this, &AMyPlayerController::MoveForward );
	InputComponent->BindAxis( "MoveRight", this, &AMyPlayerController::MoveRight );
	InputComponent->BindAxis( "TurnRate", this, &AMyPlayerController::TurnAtRate );
	InputComponent->BindAxis( "LookUpRate", this, &AMyPlayerController::LookUpAtRate );

	InputComponent->BindAxis( "Turn", MyPlayer, &APawn::AddControllerYawInput );
	InputComponent->BindAxis( "LookUp", MyPlayer, &APawn::AddControllerPitchInput );

	InputComponent->BindAction( "Shift",      IE_Pressed, this, &AMyPlayerController::JumpStart );
	InputComponent->BindAction( "Shift",      IE_Released, this, &AMyPlayerController::JumpStop );

	InputComponent->BindAction( "Space",      IE_Pressed, this, &AMyPlayerController::ProcessSpace);
	InputComponent->BindAction( "Tab",        IE_Pressed, this, &AMyPlayerController::ProcessTab );
	InputComponent->BindAction( "F",          IE_Pressed, this, &AMyPlayerController::ProcessF );
	InputComponent->BindAction( "R",          IE_Pressed, this, &AMyPlayerController::ProcessR );
	InputComponent->BindAction( "LeftClick",  IE_Pressed, this, &AMyPlayerController::ProcessLeftMouse );
	InputComponent->BindAction( "RightClick", IE_Pressed, this, &AMyPlayerController::ProcessRightMouse );
	InputComponent->BindAction( "WheelClick", IE_Pressed, this, &AMyPlayerController::ProcessWheelClick );
	InputComponent->BindAction( "WheelUp",    IE_Pressed, this, &AMyPlayerController::ProcessWheelUp );
	InputComponent->BindAction( "WheelDown",  IE_Pressed, this, &AMyPlayerController::ProcessWheelDown );
	InputComponent->BindAction( "1",          IE_Pressed, this, &AMyPlayerController::Process1 );
	InputComponent->BindAction( "2",          IE_Pressed, this, &AMyPlayerController::Process2 );
	InputComponent->BindAction( "3",          IE_Pressed, this, &AMyPlayerController::Process3 );


	InputTypeAndFuncMap[ EInputKeyType::LEFT_MOUSE ]  = bind( &AMyPlayerController::ProcessLeftMouse, this );
	InputTypeAndFuncMap[ EInputKeyType::RIGHT_MOUSE ] = bind( &AMyPlayerController::ProcessRightMouse, this );
	InputTypeAndFuncMap[ EInputKeyType::SPACE ]       = bind( &AMyPlayerController::ProcessSpace, this );
	InputTypeAndFuncMap[ EInputKeyType::Tab ]         = bind( &AMyPlayerController::ProcessTab, this );

	_ResetReadySkill();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Tick
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::Tick( float InDeltaTime )
{
	Super::Tick( InDeltaTime );

	_ProcessInputBothKey();
	_ProcessRotationRate();
	_ProcessReadySkill( InDeltaTime );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief MoveForward
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::MoveForward( float Value )
{
	if( !MyPlayer || !CharacterComp )
		return;

	UMyAnimInstance* animInstance = MyPlayer ? Cast<UMyAnimInstance>( MyPlayer->GetMesh()->GetAnimInstance() ) : nullptr;
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
void AMyPlayerController::MoveRight( float Value )
{
	if( !MyPlayer || !CharacterComp )
		return;

	UMyAnimInstance* animInstance = MyPlayer ? Cast<UMyAnimInstance>( MyPlayer->GetMesh()->GetAnimInstance() ) : nullptr;
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
void AMyPlayerController::TurnAtRate( float Rate )
{
	if( MyPlayer )
		MyPlayer->AddControllerYawInput( Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief LookUpAtRate
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::LookUpAtRate( float Rate )
{
	if( MyPlayer )
		MyPlayer->AddControllerPitchInput( Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief JumpStart
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::JumpStart()
{
	if ( MyPlayer && CharacterComp && CharacterComp->GetAnimState() == EAnimState::IDLE_RUN )
		MyPlayer->Jump();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief JumpStart
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::JumpStop()
{
	if( MyPlayer )
		MyPlayer->StopJumping();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ���콺 ���� ����
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessLeftMouse()
{
	if( !WeaponComp )
		return;

	const auto& skillInfo = GetDataInfoManager().GetPlayerWeaponSkillInfos().Find( WeaponComp->GetCurWeaponNum() );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->L_SkillNum ) ? _ResetReadySkill() : _SetReadySkill( EInputKeyType::LEFT_MOUSE );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ���콺 ������ ����
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessRightMouse()
{
	if( !WeaponComp )
		return;

	const auto& skillInfo = GetDataInfoManager().GetPlayerWeaponSkillInfos().Find( WeaponComp->GetCurWeaponNum() );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->R_SkillNum ) ? _ResetReadySkill() : _SetReadySkill( EInputKeyType::RIGHT_MOUSE );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ���콺 ���� Ŭ�� ����
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessBothMouseDown()
{
	if( !MatComp || !MyPlayer )
		return;

	switch( MatComp->GetMatState() )
	{
		case EMaterialState::GRASS:
		{
			UMyAnimInstance* animInstance = Cast< UMyAnimInstance >( MyPlayer->GetMesh()->GetAnimInstance() );
			if( animInstance && animInstance->IsJump )
				animInstance->IsFly = true;
		}
		break;
		case EMaterialState::ROCK:
		{
			_SkillPlay( 14 );
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ���콺 ���� Ŭ�� ��� ����
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessBothMouseUp()
{
	if( !MatComp || !MyPlayer )
		return;

	switch( MatComp->GetMatState() )
	{
		case EMaterialState::GRASS:
		{
			UMyAnimInstance* animInstance = Cast< UMyAnimInstance >( MyPlayer->GetMesh()->GetAnimInstance() );
			if( animInstance && animInstance->IsJump )
				animInstance->IsFly = false;
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ���콺 �� Ŭ�� ����
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessWheelClick()
{
	if( GetLockOnManager().GetLockOnTarget() )
	{
		GetLockOnManager().LockOnRelease();
		return;
	}

	GetLockOnManager().LockOnStart();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ���콺 �� ���� ����
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessWheelUp()
{
	if ( GetLockOnManager().GetLockOnTarget() )
	{
		GetLockOnManager().LockOnStart( ELockOnMode::Prev );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ���콺 �� �Ʒ��� ����
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessWheelDown()
{
	if ( GetLockOnManager().GetLockOnTarget() )
	{
		GetLockOnManager().LockOnStart( ELockOnMode::Next );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief �����̽� Ű ���� (������)
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessSpace()
{
	const auto& skillInfo = GetDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::SPACE );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->SkillNum ) ? _ResetReadySkill() : _SetReadySkill( EInputKeyType::SPACE );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief �����̽� Ű ���� ( ���� ������ )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessTab()
{
	if ( !WeaponComp )
		return;

	if ( WeaponComp->GetWeaponState() == EWeaponState::DEFAULT )
		return;

	const auto& skillInfo = GetDataInfoManager().GetPlayerWeaponSkillInfos().Find( WeaponComp->GetCurWeaponNum() );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->ThrowSkillNum ) ? _ResetReadySkill() : _SetReadySkill( EInputKeyType::Tab );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief F Ű ���� ( ���� ���� )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessF()
{
	const auto& skillInfo = GetDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::F );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->SkillNum );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief R Ű ���� ( ���� ���ƿ��� )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::ProcessR()
{
	const auto& skillInfo = GetDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::R );
	if ( !skillInfo )
		return;

	if ( MatComp && MatComp->GetMatState() != EMaterialState::JELLY )
	{
		if ( _SkillPlay( skillInfo->SkillNum ) )
		{
			MatComp->SetMatState( nullptr, true );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 1��Ű ���� ( ���� ���� )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::Process1()
{
	if( !WeaponComp )
		return;

	if( !( WeaponComp->CanWeaponComp( EWeaponState::SWORD ) ) )
		return;

	const auto& skillInfo = GetDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::Num1 );
	if ( !skillInfo )
		return;

	_SkillPlay( skillInfo->SkillNum );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 2��Ű ���� ( ���� ���� )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::Process2()
{
	if( !WeaponComp )
		return;

	if( !( WeaponComp->CanWeaponComp( EWeaponState::AXE ) ) )
		return;

	const auto& skillInfo = GetDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::Num2 );
	if( !skillInfo )
		return;

	_SkillPlay( skillInfo->SkillNum );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 3��Ű ���� ( ���� ���� )
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::Process3()
{
	if( !WeaponComp )
		return;

	if( !( WeaponComp->CanWeaponComp( EWeaponState::SPEAR ) ) )
		return;

	const auto& skillInfo = GetDataInfoManager().GetPlayerDefaultSkillInfos().Find( EInputKeyType::Num3 );
	if( !skillInfo )
		return;

	_SkillPlay( skillInfo->SkillNum );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ��ų�� ����Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool AMyPlayerController::_SkillPlay( int InSkillNum )
{
	if( !CharacterComp )
		return false;

	return CharacterComp->SkillPlay( InSkillNum );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief �ߵ� ����� ��ų �ʱ�ȭ
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::_ResetReadySkill()
{
	ReadySkillFunc = nullptr;
	ReadySkillResetTime = 0.f;
	ReadySkillInputKey = EInputKeyType::MAX;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief �ߵ� ����� ��ų ����
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::_SetReadySkill( EInputKeyType InReadyInputKey )
{
	if( ReadySkillInputKey == InReadyInputKey )
		return;

	ReadySkillFunc = InputTypeAndFuncMap[ InReadyInputKey ];
	ReadySkillResetTime = Const::READY_SKILL_DURATION;
	ReadySkillInputKey = InReadyInputKey;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief �ߵ� ������� ��ų ����
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::_ProcessReadySkill( float InDeltaTime )
{
	// Ÿ�Ӿƿ� �ʱ�ȭ
	if( ReadySkillResetTime <= 0 )
	{
		_ResetReadySkill();
		return;
	}
	else
	{
		ReadySkillResetTime -= InDeltaTime;

		// ������� ��ų �ߵ�
		if( ReadySkillFunc )
			ReadySkillFunc();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ȸ�� ���� ���� �����Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::_ProcessRotationRate()
{
	if( !MyPlayer )
		return;

	UMyAnimInstance* animInstance = Cast< UMyAnimInstance >( MyPlayer->GetMesh()->GetAnimInstance() );
	if( !animInstance )
		return;

	if( animInstance->IsFly )
	{
		MyPlayer->GetCharacterMovement()->RotationRate = FRotator( 0.0f, Const::FLY_ROTATION_RATE, 0.0f );
		return;
	}
	else if( ReadySkillResetTime > 0 || CharacterComp->IsMontageInitialTime() )
	{
		MyPlayer->GetCharacterMovement()->RotationRate = FRotator( 0.0f, Const::READY_SKILL_ROTATION_RATE, 0.0f );
		return;
	}

	MyPlayer->GetCharacterMovement()->RotationRate = FRotator( 0.0f, Const::DEFAULT_ROTATION_RATE, 0.0f );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ���� Ű �Է��� �޴´�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AMyPlayerController::_ProcessInputBothKey()
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