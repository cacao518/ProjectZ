// Fill out your copyright notice in the Description page of Project Settings.

#include "GgCharacterComp.h"
#include "GgMaterialComp.h"
#include "GgWeaponComp.h"
#include "GgFloatingBarComp.h"
#include "ETC/SDB.h"
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
		MontagePlayTime = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 몽타주를 플레이한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::MontagePlay( const FString& InMontagePath, float InScale )
{
	UAnimMontage* anim = LoadObject<UAnimMontage>( NULL, *InMontagePath, NULL, LOAD_None, NULL );
	if ( !anim )
		return;

	MontagePlay( anim, InScale );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 몽타주를 플레이한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::MontagePlay( UAnimMontage* InMontage, float InScale )
{
	if( !OwningCharacter )
		return;

	if ( !OwningCharacter->GetMesh() )
		return;

	UAnimInstance* animInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if( !animInstance || !InMontage )
		return;

	ResetInfo( true );

	animInstance->Montage_Play( InMontage, InScale );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 스킬을 플레이한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool UGgCharacterComp::SkillPlay( int InSkillNum )
{
	const auto& skillInfo = GetGgDataInfoManager().GetSkillInfos().Find( InSkillNum );
	if ( !skillInfo )
		return false;

	// 파생스킬을 발동 시킬 것인지 확인
	if( CurSkillInfo )
	{
		if( skillInfo->DerivedSkillNum != 0 && IsEnableDerivedKey )
		{
			if( CurSkillInfo->Num == skillInfo->Num )
			{
				IsEnableDerivedKey = false;
			}

			return SkillPlay( skillInfo->DerivedSkillNum );
		}
	}

	// Mp 확인
	if( skillInfo->CostMP > Stat.Mp )
		return false;

	// 쿨타임 확인
	if ( IsCoolingSkill( skillInfo->Num ) )
		return false;

	// 현재 스킬 사용 가능한 AnimState / SkillNum 인지 확인
	bool isEmptyEnableState = skillInfo->PlayEnableState.IsEmpty();
	bool isEmptyEnableSkillNum = skillInfo->PlayEnableSkillNum.IsEmpty();
	bool isFindEnableState = skillInfo->PlayEnableState.Find( AnimState ) != INDEX_NONE;
	bool isFindEnableSkillNum = skillInfo->PlayEnableSkillNum.Find( CurSkillInfo ? CurSkillInfo->Num : 0 ) != INDEX_NONE;

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

	if ( skillInfo->PlaySpeedType == ESkillPlaySpeedType::DEFAULT )
		MontagePlay( skillInfo->AnimPath );
	else
		MontagePlay( skillInfo->AnimPath, skillInfo->PlaySpeedType == ESkillPlaySpeedType::ATTACK_SPEED ? Stat.AttackSpeed : Stat.MoveSpeed );

	_RegisterCoolTime( *skillInfo );

	CurSkillInfo = skillInfo;

	Stat.Mp = FMath::Clamp( Stat.Mp - skillInfo->CostMP, 0, Stat.Mpm );

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 공격 성공 처리를 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::OnAttackSuccess()
{
	auto WeaponComp = OwningActor ? OwningActor->FindComponentByClass<UGgWeaponComp>() : nullptr;
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

	auto characterMovement = OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr;
	if( characterMovement )
		characterMovement->MaxWalkSpeed = Stat.MoveSpeed * CONST::DEFAULT_MOVE_SPEED;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 점프력을 설정한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::SetJumpPower( float InJumpPower )
{
	Stat.JumpPower = InJumpPower;

	auto characterMovement = OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr;
	if( characterMovement )
		characterMovement->JumpZVelocity = Stat.JumpPower * CONST::DEFAULT_JUMP_POWER;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 이동할 위치를 셋팅한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::SetMovePos( float InMovePower, bool InIsKnockBack )
{
	if( !OwningCharacter )
		return;

	const FRotator rotation = OwningCharacter->GetActorRotation();
	const FRotator yawRotation( 0, rotation.Yaw, 0 );
	const FVector  direction = FRotationMatrix( yawRotation ).GetUnitAxis( EAxis::X );

	if( InIsKnockBack )
		MovePos = OwningCharacter->GetActorLocation() - ( direction * InMovePower );
	else
		MovePos = OwningCharacter->GetActorLocation() + ( direction * ( InMovePower * Stat.MoveSpeed ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 현재 몽타주 이름을 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
FString UGgCharacterComp::GetCurMontageName()
{
	auto curMontage = OwningCharacter ? OwningCharacter->GetMesh()->GetAnimInstance()->GetCurrentActiveMontage() : nullptr;
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
	if( !OwningCharacter )
		return;

	if( !IsDie && AnimState == EAnimState::DIE )
	{
		IsDie = true;

		switch( GetObjectType() )
		{
		case  EObjectType::NPC:
		{
			AGgAIController* characterNPCController = Cast< AGgAIController >( OwningCharacter->GetController() );
			if( characterNPCController )
				characterNPCController->StopAI();

			OwningCharacter->GetMesh()->SetSimulatePhysics( true );
			OwningCharacter->GetMesh()->SetCollisionEnabled( ECollisionEnabled::PhysicsOnly );
			GetGgObjectManager().SpawnParticle( TEXT( "Die" ), OwningCharacter, OwningCharacter->GetActorLocation(), OwningCharacter->GetActorRotation() );
		}
		break;
		case  EObjectType::PC:
		{
			OwningCharacter->GetMesh()->SetSimulatePhysics( true );
			OwningCharacter->GetMesh()->SetCollisionEnabled( ECollisionEnabled::PhysicsOnly );
			GetGgObjectManager().SpawnParticle( TEXT( "Die" ), OwningCharacter, OwningCharacter->GetActorLocation(), OwningCharacter->GetActorRotation() );
			/*if( UMyAnimInstance* animInstance = Cast<UMyAnimInstance>( OwningCharacter->GetMesh()->GetAnimInstance() ) )
			{
				auto curMontage = OwningCharacter->GetMesh()->GetAnimInstance()->GetCurrentActiveMontage();
				animInstance->Montage_Stop( 0.f, curMontage );
				animInstance->IsDie = true;
			}*/
		}
		break;
		case  EObjectType::STATIC_OBJECT:
		{
			GetGgObjectManager().DestroyActor( OwningCharacter );
		}
		break;
		}

		if( UGgAnimInstance* animInstance = Cast<UGgAnimInstance>( OwningCharacter->GetMesh()->GetAnimInstance() ) )
		{
			auto curMontage = OwningCharacter->GetMesh()->GetAnimInstance()->GetCurrentActiveMontage();
			animInstance->Montage_Stop( 0.f, curMontage );
		}
		OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName( TEXT( "NoCollision" ) );
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
void UGgCharacterComp::_ProcessHit( AActor* InOtherActor )
{
	auto othetObjectComp = InOtherActor ? InOtherActor->FindComponentByClass<UGgObjectComp>() : nullptr;
	if( !othetObjectComp )
		return;

	auto othetMatComp = InOtherActor ? InOtherActor->FindComponentByClass<UGgMaterialComp>() : nullptr;
	auto myMatComp    = OwningActor  ? OwningActor->FindComponentByClass<UGgMaterialComp>() : nullptr;

	if ( othetObjectComp->GetTeamType() == ETeamType::MAX || TeamType == ETeamType::MAX )
		return;

	if ( othetObjectComp->GetTeamType() == TeamType && !othetObjectComp->Stat.IsTyrant )
		return;

	othetObjectComp->OnAttackSuccess();

	// 체력 감소
	float totalDamage = othetObjectComp->GetAttackCollInfo().Power * othetObjectComp->GetStat().AttackPower;
	totalDamage -= Stat.DefensePower;
	totalDamage = totalDamage > 0 ? totalDamage : 1;

	float decrease = Stat.Hp - totalDamage;
	Stat.Hp = decrease > 0 ? decrease : 0;

	float myIntensity    = myMatComp    ? myMatComp->GetIntensity()    : 1.f;
	float otherIntensity = othetMatComp ? othetMatComp->GetIntensity() : 1.f;

	// 경직
	if( otherIntensity >= myIntensity )
	{
		float addTimeToDamage = totalDamage / Stat.Hpm;
		float subTimeToMyStrength = Stat.Strength * 0.01f;
		float hitPlayRate = FMath::Clamp( 1.f - addTimeToDamage + subTimeToMyStrength, 0.3f, 1.f );
		MontagePlay( HitAnim, hitPlayRate );
		LookAt( Cast<ACharacter>( InOtherActor ) );

		// 넉백
		float knockbackPower = othetObjectComp->GetAttackCollInfo().KnockBackPower - ( Stat.Strength * 1.5f );
		if( knockbackPower > 0 )
		{
			SetMovePos( knockbackPower, true );
			SetIsForceMove( true );
		}

		// 역경직 시간 추가
		HoldTime += othetObjectComp->GetAttackCollInfo().HitStopTime;
	}

	_ProcessCameraShake( InOtherActor );

	_UpdateHpBar();

	//FString str = OwningActor->GetName() + TEXT( " : HitColl -> HP : " ) + FString::FromInt( (int)Stat.Hp );
	//if ( GEngine )
	//	GEngine->AddOnScreenDebugMessage( -1, 3.0f, FColor::Yellow, str );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 초기화 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_Init()
{
	OwningCharacter = Cast<ACharacter>( OwningActor );

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

	if( !OwningCharacter )
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
			moveComponent->IsFalling() ? nextState = EAnimState::JUMP : nextState = EAnimState::IDLE_RUN;
	}

	if( nextState != AnimState )
	{
		AnimState = nextState;
		animInstance->AnimState = AnimState;
		animInstance->AnimSubState = CurSkillInfo ? CurSkillInfo->AnimSubState : EAnimSubState::DEFAULT;

		if( auto weaponComp = OwningActor->FindComponentByClass<UGgWeaponComp>() )
		{
			if( AnimState == EAnimState::COMMON_ACTION )
			{
				CurSkillInfo && CurSkillInfo->EquipSubWeapon ? weaponComp->EquipSubWeapon() : weaponComp->UnEquipSubWeapon();
			}
			else
			{
				weaponComp->UnEquipSubWeapon();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 이동 관련 로직을 수행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_ProcessMove()
{
	if ( IsHold() )
		return;

	auto characterMovement = OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr;

	if( !characterMovement )
		return;

	if( AnimState == EAnimState::IDLE_RUN || AnimState == EAnimState::JUMP )
	{
		characterMovement->MaxWalkSpeed = Stat.MoveSpeed * ( IsDash ? CONST::DEFAULT_DASH_SPEED : CONST::DEFAULT_MOVE_SPEED );
	}
	else
	{
		if( IsForceMove )
		{
			float dest_X = FMath::Lerp( characterMovement->GetActorLocation().X, MovePos.X, GetWorld()->GetDeltaSeconds() * CONST::ANIM_LERP_MULITPLIER );
			float dest_Y = FMath::Lerp( characterMovement->GetActorLocation().Y, MovePos.Y, GetWorld()->GetDeltaSeconds() * CONST::ANIM_LERP_MULITPLIER );
			float dest_Z = FMath::Lerp( characterMovement->GetActorLocation().Z, MovePos.Z, GetWorld()->GetDeltaSeconds() * CONST::ANIM_LERP_MULITPLIER );
			FVector dest = FVector( dest_X, dest_Y, dest_Z );

			OwningCharacter->SetActorLocation( dest, true );
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

	if( float* coolTime = CoolingSkills.Find( InSkillInfo.Num ) )
		*coolTime = InSkillInfo.CoolTime;
	else
		CoolingSkills.Add( InSkillInfo.Num, InSkillInfo.CoolTime );
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
		MontagePlay( HitAnim, CONST::WATER_FALL_ANIM_TIME );

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
//// @brief 착지 로직을 실행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgCharacterComp::_ProcessLand()
{
	if( !OwningCharacter )
		return;

	auto animInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if( !animInstance )
		return;

	auto curMontage = OwningCharacter ? OwningCharacter->GetMesh()->GetAnimInstance()->GetCurrentActiveMontage() : nullptr;

	auto moveComponent = OwningCharacter->GetMovementComponent();
	if( moveComponent )
	{
		if( moveComponent->IsFalling() )
		{
			LandOnce = true;
		}
		else if( LandOnce )
		{
			if( !curMontage )
				MontagePlay( LandAnim );
			
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

	if ( !OwningCharacter )
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
		animInstance->Montage_Resume( curMontage );
		return;
	}

	animInstance->Montage_Pause( curMontage );
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