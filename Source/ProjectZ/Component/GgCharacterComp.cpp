// Fill out your copyright notice in the Description page of Project Settings.

#include "GgCharacterComp.h"
#include "GgMaterialComp.h"
#include "GgWeaponComp.h"
#include "GgFloatingBarComp.h"
#include "Actor/GgCharacterPC.h"
#include "Actor/GgCharacterNPC.h"
#include "Manager/GgObjectManager.h"
#include "Manager/GgDataInfoManager.h"
#include "Manager/GgCameraManager.h"
#include "Manager/GgLockOnManager.h"
#include "System/GgAIController.h"
#include "System/GgGameInstance.h"
#include "System/GgAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "WaterBodyComponent.h"
#include "LandscapeComponent.h"
#include "LandscapeProxy.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


UGgCharacterComp::UGgCharacterComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UGgCharacterComp::~UGgCharacterComp()
{
	CoolingSkills.Reset();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Begin
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::BeginPlay()
{
	Super::BeginPlay();

	_Init();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Tick
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	_AnimStateChange();
	_ProcessHold( DeltaTime );
	_ProcessMove();
	_ProcessAccTime( DeltaTime );
	_ProcessMp( DeltaTime );
	_CoolingSkills( DeltaTime );
	_FallingWater( DeltaTime );
	_ProcessLand();

	ResetInfo();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 정보를 초기화한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::ResetInfo( bool InForceReset )
{
	if( InForceReset ||
		AnimState == EAnimState::IDLE_RUN ||
		AnimState == EAnimState::JUMP ||
		AnimState == EAnimState::DIE )
	{
		SetIsEnabledAttackColl( false );
		SetAttackCollData( FCollisionData() );
		IsEnableDerivedKey = false;
		IsForceMove = false;
		CurSkillInfo = nullptr;
		HoldTime = 0;
		MontagePlayTime = 0;
		MontagePlayRate = 0;
		HitObjIds.Empty();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 몽타주를 플레이한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::MontagePlay( TSoftObjectPtr<UAnimMontage> InMontagePtr, float InScale )
{
	UAnimMontage* anim = InMontagePtr.LoadSynchronous();
	if ( !anim )
		return;

	MontagePlay( anim, InScale );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 몽타주를 플레이한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::MontagePlay( UAnimMontage* InMontage, float InScale )
{
	if( !OwningCharacter.IsValid() )
		return;

	if ( !OwningCharacter->GetMesh() )
		return;

	UAnimInstance* animInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if( !animInstance || !InMontage )
		return;

	ResetInfo( true );

	animInstance->Montage_Play( InMontage, InScale );

	MontagePlayRate = InScale;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스킬을 플레이한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool UGgCharacterComp::SkillPlay( int InSkillNum )
{
	const auto& skillInfo = GetGgDataInfoManager().GetInfo<FSkillInfo>( InSkillNum );
	if ( !skillInfo )
		return false;

	// 파생스킬을 발동 시킬 것인지 확인
	if( CurSkillInfo )
	{
		if( skillInfo->DerivedSkillId != 0 && IsEnableDerivedKey )
		{
			if( CurSkillInfo->InfoId == skillInfo->InfoId )
			{
				IsEnableDerivedKey = false;
			}

			return SkillPlay( skillInfo->DerivedSkillId );
		}
	}

	// Mp 확인
	if( skillInfo->CostMP > Stat.Mp )
		return false;

	// 쿨타임 확인
	if ( IsCoolingSkill( skillInfo->InfoId ) )
		return false;

	// 현재 스킬 사용 가능한 AnimState / SkillNum 인지 확인
	bool isEmptyEnableState = skillInfo->PlayEnableState.IsEmpty();
	bool isEmptyEnableSkillNum = skillInfo->PlayEnableSkillId.IsEmpty();
	bool isFindEnableState = skillInfo->PlayEnableState.Find( AnimState ) != INDEX_NONE;
	bool isFindEnableSkillNum = skillInfo->PlayEnableSkillId.Find( CurSkillInfo ? CurSkillInfo->InfoId : 0 ) != INDEX_NONE;

	// 사용가능한 AnimState / SkillNum 이 둘 다 설정 되어 있을 경우 둘 중 하나만 만족하면 된다.
	if ( !isEmptyEnableState && !isEmptyEnableSkillNum )
	{
		if ( !isFindEnableState && !isFindEnableSkillNum )
			return false;
	}
	else
	{   // 사용가능한 AnimState / SkillNum 둘 중 하나만 설정 되어 있을 경우 설정 되어 있는 것만 만족하면 된다.
		if ( !isEmptyEnableState && !isFindEnableState )
			return false;
		if ( !isEmptyEnableSkillNum && !isFindEnableSkillNum )
			return false;
	}

	// 역경직 진행중일 경우에는 초기화 시켜주어야한다.
	HoldTime = 0;

	if ( skillInfo->PlaySpeedType == ESkillPlaySpeedType::DEFAULT )
		MontagePlay( skillInfo->AnimMontage );
	else
		MontagePlay( skillInfo->AnimMontage, skillInfo->PlaySpeedType == ESkillPlaySpeedType::ATTACK_SPEED ? Stat.AttackSpeed : Stat.MoveSpeed );

	_RegisterCoolTime( *skillInfo );

	CurSkillInfo = skillInfo;

	Stat.Mp = FMath::Clamp( Stat.Mp - skillInfo->CostMP, 0, Stat.Mpm );

	// 서브 웨폰 장착
	if( auto weaponComp = OwningActor->FindComponentByClass<UGgWeaponComp>() )
	{
		CurSkillInfo&& CurSkillInfo->EquipSubWeapon ? weaponComp->EquipSubWeapon() : weaponComp->UnEquipSubWeapon();
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 공격 성공 처리를 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::OnAttackSuccess( int64 InHitObjId )
{
	Super::OnAttackSuccess( InHitObjId );

	auto WeaponComp = OwningActor.IsValid() ? OwningActor->FindComponentByClass<UGgWeaponComp>() : nullptr;
	if( !WeaponComp )
		return;

	//WeaponComp->SubWeaponDurability();

	// 역경직 시간 추가
	HoldTime += AttackCollData.HitStopTime;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 이동속도를 설정한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::SetMoveSpeed( float InMoveSpeed )
{
	Stat.MoveSpeed = InMoveSpeed;

	auto characterMovement = OwningCharacter.IsValid() ? OwningCharacter->GetCharacterMovement() : nullptr;
	if( characterMovement )
		characterMovement->MaxWalkSpeed = Stat.MoveSpeed * CONST::DEFAULT_MOVE_SPEED;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 점프력을 설정한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::SetJumpPower( float InJumpPower )
{
	Stat.JumpPower = InJumpPower;

	auto characterMovement = OwningCharacter.IsValid() ? OwningCharacter->GetCharacterMovement() : nullptr;
	if( characterMovement )
		characterMovement->JumpZVelocity = Stat.JumpPower * CONST::DEFAULT_JUMP_POWER;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 이동할 위치를 셋팅한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::SetMovePos( float InMovePower, bool InIsKnockBack )
{
	if( !OwningCharacter.IsValid() )
		return;

	const FRotator rotation = OwningCharacter->GetActorRotation();
	const FRotator yawRotation( 0, rotation.Yaw, 0 );
	const FVector  direction = FRotationMatrix( yawRotation ).GetUnitAxis( EAxis::X );

	if( InIsKnockBack )
	{
		MovePos = OwningCharacter->GetActorLocation() - ( direction * InMovePower );
		AirborneDir = direction * InMovePower * CONST::AIRBORNE_KNOCKBACK_MULITPLIER * -1;
	}
	else
	{
		MovePos = OwningCharacter->GetActorLocation() + ( direction * ( InMovePower * Stat.MoveSpeed ) );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 현재 몽타주 이름을 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
FString UGgCharacterComp::GetCurMontageName()
{
	auto curMontage = OwningCharacter.IsValid() ? OwningCharacter->GetMesh()->GetAnimInstance()->GetCurrentActiveMontage() : nullptr;
	FString curMontageName = curMontage ? curMontage->GetName() : "";

	return curMontageName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 해당 스킬이 쿨타임이 돌고 있는지 여부를 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool UGgCharacterComp::IsCoolingSkill( int InSkillNum )
{
	float* cooltime = CoolingSkills.Find( InSkillNum );
	if ( cooltime )
	{
		if ( *cooltime > 0 )
			return true;
		else
			return false;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 몽타주 재생 초기시간인지 여부를 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool UGgCharacterComp::IsMontageInitialTime()
{
	if( AnimState != EAnimState::COMMON_ACTION )
		return false;

	if( MontagePlayTime > CONST::MONTAGE_INITIAL_TIME )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 역경직 상태 여부를 반환한다. 
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool UGgCharacterComp::IsHold()
{
	return HoldTime > 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 사망 관련 로직을 수행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_ProcessDie()
{
	if( !OwningCharacter.IsValid() )
		return;

	if( !IsDie && AnimState == EAnimState::DIE )
	{
		if( UGgAnimInstance* animInstance = Cast<UGgAnimInstance>( OwningCharacter->GetMesh()->GetAnimInstance() ) )
		{
			animInstance->StopAllMontages( 0.f );
			animInstance->IsDie = true;
		}
		IsDie = true;
		ResetInfo( true );

		switch( GetObjectType() )
		{
			case  EObjectType::NPC:
			{
				if( AGgAIController* characterNPCController = Cast< AGgAIController >( OwningCharacter->GetController() ) )
				{
					characterNPCController->StopAI();
				}

				OwningCharacter->GetMesh()->SetSimulatePhysics( true );
				OwningCharacter->GetMesh()->SetCollisionEnabled( ECollisionEnabled::PhysicsOnly );
				OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName( TEXT( "NoCollision" ) );
				if( const auto& effectInfo = GetGgDataInfoManager().GetInfo<FCommonEffectInfo>( TEXT( "Die" ) ) )
				{
					GetGgObjectManager().SpawnParticle( effectInfo->Effect, OwningCharacter->GetRootComponent(), OwningCharacter->GetTransform() );
				}
				break;
			}
			case  EObjectType::PC:
			{
				OwningCharacter->GetMesh()->SetSimulatePhysics( true );
				OwningCharacter->GetMesh()->SetCollisionEnabled( ECollisionEnabled::PhysicsOnly );
				OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName( TEXT( "NoCollision" ) );
				break;
			}
		}

		_UpdateHpBar();
	}

	if( AnimState == EAnimState::DIE && DeathTime >= CONST::DEAD_ACTOR_DESTROY_TIME )
	{
		GetGgObjectManager().DestroyActor( OwningCharacter );

		if ( GetObjectType() == EObjectType::PC )
		{
			FString levelName = L"/Game/Maps/Stage1/Stage1";
			UGameplayStatics::OpenLevel( this, *levelName );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 피격 처리를 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
float UGgCharacterComp::_ProcessHit( FActorPtr InOtherActor )
{
	float totalDamage = Super::_ProcessHit( InOtherActor );
	if( totalDamage < 0 ) 
		return -1;

	auto othetObjectComp = InOtherActor.IsValid() ? InOtherActor->FindComponentByClass<UGgObjectComp>() : nullptr;
	if( !othetObjectComp )
		return -1;

	UGgAnimInstance* animInstance = Cast<UGgAnimInstance>( OwningCharacter->GetMesh()->GetAnimInstance() );
	if( !animInstance ) 
		return -1;

	ResetInfo( true );

	// 에어본
	float airbornePower = othetObjectComp->GetAttackCollInfo().AirbornePower;
	if( airbornePower > 0 || animInstance->IsAirborne )
	{
		_ResetAIController();
		animInstance->StopAllMontages( 0 );
		animInstance->IsAirborne = true;
		airbornePower = airbornePower > 0 ? airbornePower : CONST::DEFAULT_AIRBORNEPOWER;
		airbornePower = ( CONST::MAX_MASS - Stat.Weight ) * airbornePower;
		LookAt( Cast<ACharacter>( InOtherActor ) );
		OwningCharacter->LaunchCharacter( FVector( 0,0, airbornePower ), true, true );
	}
	else if( !animInstance->IsAirborne )
	{
		// 경직
		float addTimeToDamage = totalDamage / Stat.Hpm;
		float subTimeToMyStrength = Stat.Strength * 0.01f;
		float hitPlayRate = FMath::Clamp( 1.f - addTimeToDamage + subTimeToMyStrength, 0.3f, 1.f );
		   
		UAnimMontage* hitAnim = HitAnim.IsValid() ? HitAnim.Get() : HitAnim.LoadSynchronous();
		MontagePlay( hitAnim, hitPlayRate );
		LookAt( Cast<ACharacter>( InOtherActor ) );
		_ResetAIController();

		// 역경직 시간 추가
		HoldTime = othetObjectComp->GetAttackCollInfo().HitStopTime;
	}

	// 넉백
	float knockbackPower = othetObjectComp->GetAttackCollInfo().KnockBackPower - ( Stat.Strength * 1.5f );
	if( knockbackPower > 0 )
	{
		SetMovePos( knockbackPower, true );
		SetIsForceMove( true );
	}

	_UpdateHpBar();

	// 서브 웨폰 해제
	if( auto weaponComp = OwningActor->FindComponentByClass<UGgWeaponComp>() )
	{
		weaponComp->UnEquipSubWeapon();
	}

	return totalDamage;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 초기화 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_Init()
{
	OwningCharacter = Cast<ACharacter>( OwningActor );

	SetMoveSpeed( Stat.MoveSpeed );
	SetJumpPower( Stat.JumpPower );

	ResetInfo( true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 애니메이션 상태를 교체한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_AnimStateChange()
{
	if( Stat.Hp <= 0 )
	{
		AnimState = EAnimState::DIE;
		return;
	}

	if( !OwningCharacter.IsValid() )
		return;

	UGgAnimInstance* animInstance = Cast<UGgAnimInstance>( OwningCharacter->GetMesh()->GetAnimInstance() );
	if( !animInstance )
		return;

	EAnimState nextState = AnimState;

	if( auto curMontage = animInstance->GetCurrentActiveMontage() )
	{
		nextState = EAnimState::COMMON_ACTION;
	}
	else
	{
		if( auto moveComponent = OwningCharacter->GetMovementComponent(); moveComponent )
		{
			if( animInstance->IsJump )
			{
				nextState = EAnimState::JUMP;
			}
			else if( animInstance->IsAirborne )
			{
				nextState = EAnimState::AIRBORNE;
			}
			else
			{
				nextState = EAnimState::IDLE_RUN;
			}
		}
	}

	if( nextState != AnimState )
	{
		AnimState = nextState;
		animInstance->AnimState = AnimState;
		animInstance->AnimSubState = CurSkillInfo ? CurSkillInfo->AnimSubState : EAnimSubState::DEFAULT;

		// 서브 웨폰 해제
		if( auto weaponComp = OwningActor->FindComponentByClass<UGgWeaponComp>() )
		{
			if( AnimState != EAnimState::COMMON_ACTION )
			{
				weaponComp->UnEquipSubWeapon();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스킬 쿨타임을 등록한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_RegisterCoolTime( const FSkillInfo& InSkillInfo )
{
	if( InSkillInfo.CoolTime <= 0 )
		return;

	if( float* coolTime = CoolingSkills.Find( InSkillInfo.InfoId ) )
		*coolTime = InSkillInfo.CoolTime;
	else
		CoolingSkills.Add( InSkillInfo.InfoId, InSkillInfo.CoolTime );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 체력바를 업데이트 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_UpdateHpBar()
{
	auto floatingBarComp = Cast<UGgFloatingBarComp>( OwningCharacter->GetDefaultSubobjectByName( TEXT( "FloatingBarComp" ) ) );
	if ( !floatingBarComp )
		return;

	floatingBarComp->UpdateHpBar( Stat.Hp / Stat.Hpm );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스킬 쿨타임을 돌린다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_CoolingSkills( float InDeltaTime )
{
	for( auto& [_, remainCoolTime] : CoolingSkills )
	{
		if( remainCoolTime > 0 )
			remainCoolTime -= InDeltaTime;
		else
			continue;

		if( remainCoolTime < 0 )
			remainCoolTime = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 물에 빠지는 처리를 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_FallingWater( float InDeltaTime )
{
	if( !IsFallWater || Stat.Hp <= 0 )
		return;

	auto curMontage = OwningCharacter->GetMesh()->GetAnimInstance()->GetCurrentActiveMontage();
	bool isNotHitAnim = curMontage && HitAnim && curMontage->GetName() != HitAnim->GetName();
	if( !curMontage || isNotHitAnim )
	{
		UAnimMontage* hitAnim = HitAnim.IsValid() ? HitAnim.Get() : HitAnim.LoadSynchronous();
		MontagePlay( hitAnim, CONST::WATER_FALL_ANIM_TIME );
	}
	if( FallWaterTime <= 3.f )
	{
		FallWaterTime += InDeltaTime;
		FVector originPos = OwningCharacter->GetMesh()->GetRelativeLocation();
		OwningCharacter->GetMesh()->SetRelativeLocation( FVector( originPos.X, originPos.Y, originPos.Z - 1.7f ) );
	}
	else
	{
		Stat.Hp = 0;
		IsFallWater = false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 이동 관련 로직을 수행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_ProcessMove()
{
	if( IsHold() )
		return;

	auto characterMovement = OwningCharacter.IsValid() ? OwningCharacter->GetCharacterMovement() : nullptr;

	if( !characterMovement )
		return;

	if( AnimState == EAnimState::AIRBORNE )
	{
		float dest_X = characterMovement->GetActorLocation().X + AirborneDir.X;
		float dest_Y = characterMovement->GetActorLocation().Y + AirborneDir.Y;
		float dest_Z = characterMovement->GetActorLocation().Z;
		FVector dest = FVector( dest_X, dest_Y, dest_Z );
		OwningCharacter->SetActorLocation( dest, true );
	}
	else
	{
		if( IsForceMove )
		{
			float dest_X = FMath::Lerp( characterMovement->GetActorLocation().X, MovePos.X, GetWorld()->GetDeltaSeconds() * CONST::ANIM_LERP_MULITPLIER );
			float dest_Y = FMath::Lerp( characterMovement->GetActorLocation().Y, MovePos.Y, GetWorld()->GetDeltaSeconds() * CONST::ANIM_LERP_MULITPLIER );
			float dest_Z = characterMovement->GetActorLocation().Z;
			FVector dest = FVector( dest_X, dest_Y, dest_Z );

			OwningCharacter->SetActorLocation( dest, true );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 착지 로직을 실행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_ProcessLand()
{
	if( !OwningCharacter.IsValid() )
		return;

	auto animInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if( !animInstance )
		return;

	auto moveComponent = OwningCharacter->GetMovementComponent();
	if( moveComponent )
	{
		if( moveComponent->IsFalling() )
		{
			LandOnce = true;
		}
		else if( LandOnce )
		{
			UAnimMontage* randAnim = LandAnim.IsValid() ? LandAnim.Get() : LandAnim.LoadSynchronous();
			MontagePlay( randAnim );

			GetGgCameraManager().CameraShake( OwningCharacter, 1.f, true );
			LandOnce = false;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 역경직 로직을 수행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_ProcessHold( float InDeltaTime )
{
	if ( !IsHold() )
		return;

	if ( !OwningCharacter.IsValid() )
		return;

	auto animInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if ( !animInstance )
		return;

	auto curMontage = animInstance->GetCurrentActiveMontage();
	if ( !curMontage )
		return;

	HoldTime -= InDeltaTime;

	if ( HoldTime <= 0 )
	{
		HoldTime = 0;
		animInstance->Montage_SetPlayRate( curMontage, MontagePlayRate );
		return;
	}

	animInstance->Montage_SetPlayRate( curMontage, 0.01f );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Mp 로직을 수행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_ProcessMp( float InDeltaTime )
{
	if( AnimState == EAnimState::IDLE_RUN )
		Stat.Mp = FMath::Clamp( Stat.Mp + ( InDeltaTime * CONST::MP_RECOVERY_VALUE ), 0, Stat.Mpm );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 누적 시간 관련 로직 처리를 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_ProcessAccTime( float InDeltaTime )
{
	if ( AnimState == EAnimState::COMMON_ACTION )
	{
		MontagePlayTime += InDeltaTime;
	}
	else if ( AnimState == EAnimState::DIE )
	{
		DeathTime += InDeltaTime;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief AI를 리셋 시킨다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_ResetAIController()
{
	if( AGgAIController* aiController = Cast<AGgAIController>( OwningCharacter->GetController() ) )
	{
		aiController->Reset();
	}
}