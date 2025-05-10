// Fill out your copyright notice in the Description page of Project Settings.

#include "GgObjectComp.h"
#include "GgMaterialComp.h"
#include "GgWeaponComp.h"
#include "Actor/GgCharacterPC.h"
#include "Actor/GgCharacterNPC.h"
#include "Actor/GgStaticObject.h"
#include "Actor/GgProjectile.h"
#include "Manager/GgObjectManager.h"
#include "Manager/GgDataInfoManager.h"
#include "Manager/GgCameraManager.h"
#include "Manager/GgLockOnManager.h"
#include "System/GgAIController.h"
#include "System/GgGameInstance.h"
#include "System/GgAnimInstance.h"
#include "Util/UtilCollision.h"
#include "Util/UtilMaterial.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProceduralMeshComponent.h"
#include "WaterBodyComponent.h"
#include "LandscapeComponent.h"
#include "LandscapeProxy.h"
#include "Kismet/KismetMathLibrary.h"


UGgObjectComp::UGgObjectComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UGgObjectComp::~UGgObjectComp()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Begin
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::BeginPlay()
{
	Super::BeginPlay();

	if( IsSpawnedInEditor )
	{
		GetGgObjectManager().RegisterActorInEditor( OwningActor );
	}

	_Init();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Tick
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	_ProcessDie();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 대상을 바라본다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::LookAt( ACharacter* InTarget )
{
	if( !OwningActor || !InTarget )
		return;

	FRotator rotator = UKismetMathLibrary::FindLookAtRotation( OwningActor->GetActorLocation(), InTarget->GetActorLocation() );
	rotator.Pitch = OwningActor->GetActorRotation().Pitch;
	OwningActor->SetActorRotation( rotator );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 공격 콜리전 정보를 셋팅한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::SetAttackCollData( const FCollisionData& InAttackCollData )
{
	AttackCollData = InAttackCollData;

	auto attackColl = OwningActor ? Cast<UProceduralMeshComponent>( OwningActor->GetDefaultSubobjectByName( TEXT( "AttackColl" ) ) ) : nullptr;
	if( !attackColl ) return;

	UtilCollision::SetProceduralMeshCollision( attackColl, InAttackCollData );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 이동속도를 설정한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::SetMoveSpeed( float InMoveSpeed )
{
	Stat.MoveSpeed = InMoveSpeed;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 점프력을 설정한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::SetJumpPower( float InJumpPower )
{
	Stat.JumpPower = InJumpPower;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 공격 콜리전 활성화 여부를 셋팅한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::SetIsEnabledAttackColl( bool InIsEnabled )
{
	auto attackColl = OwningActor ? Cast<UProceduralMeshComponent>( OwningActor->GetDefaultSubobjectByName( TEXT( "AttackColl" ) ) ) : nullptr;
	if( !attackColl ) return;

	bEnableAttackColl = InIsEnabled;

	attackColl->SetCollisionEnabled( InIsEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 히트 콜리전 활성화 여부를 셋팅한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::SetIsEnabledHitColl( bool InIsEnabled )
{
	auto hitColl = OwningActor ? Cast<UBoxComponent>( OwningActor->GetDefaultSubobjectByName( TEXT( "HitColl" ) ) ) : nullptr;
	if( !hitColl ) return;

	hitColl->SetCollisionEnabled( InIsEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 오브젝트 타입을 알아낸다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
EObjectType UGgObjectComp::GetObjectType()
{
	if( !OwningActor )
		return EObjectType::MAX;

	if( auto characterPC = Cast< AGgCharacterPC >( OwningActor ) )
		return EObjectType::PC;
	else if( auto characterNPC = Cast< AGgCharacterNPC >( OwningActor ) )
		return EObjectType::NPC;
	else if( auto staticObject = Cast< AGgStaticObject >( OwningActor ) )
		return EObjectType::STATIC_OBJECT;
	else if ( auto projectile = Cast< AGgProjectile >( OwningActor ) )
		return EObjectType::PROJECTILE;

	return EObjectType::MAX;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 고 중량 인지 여부를 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool UGgObjectComp::IsHeavyWeight()
{
	if ( Stat.Weight >= CONST::HARD_RATE )
		return true;
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 충돌이 시작할시에 호출되는 델리게이트에 등록하는 함수
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::HitCollBeginOverlap( UPrimitiveComponent* OverlappedComponent,
									   AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									   bool bFromSweep, const FHitResult& SweepResult )
{
	if( !OwningActor || IsDie || IsFallWater ) 
		return;

	// 자기 자신 충돌은 무시한다.
	if( Cast<AActor>( OtherActor ) == OwningActor ) 
		return;

	auto component = Cast<UProceduralMeshComponent>( OtherComp );
	if( component && component->GetName().Equals( TEXT( "AttackColl" ) ) )
	{
		_ProcessHit( OtherActor );
		return;
	}

	_ProcessWaterHit( OtherActor );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 카메라 쉐이크 처리를 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::_ProcessCameraShake( AActor* InOtherActor )
{
	// 내 플레이어가 맞거나, 때린 경우에만 카메라 쉐이크
	AGgCharacterPC* myPlayer = GetGgGameInstance().GetMyPlayer();
	if ( !myPlayer )
		return;

	if ( OwningActor == myPlayer || InOtherActor == myPlayer )
	{
		GetGgCameraManager().CameraShake( OwningActor );
		return;
	}

	// 공격한 투사체가 내 팀일 경우 카메라 쉐이크
	auto projectile = Cast< AGgProjectile >( InOtherActor );
	if ( !projectile )
		return;

	auto projObjComp = projectile->FindComponentByClass<UGgObjectComp>();
	auto myPlayerObjComp = myPlayer->FindComponentByClass<UGgObjectComp>();
	if ( projObjComp && myPlayerObjComp && myPlayerObjComp->GetTeamType() == projObjComp->GetTeamType() )
	{
		GetGgCameraManager().CameraShake( OwningActor );
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 사망 관련 로직을 수행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::_ProcessDie()
{
	if( !OwningActor )
		return;

	if( IsFallWater || Stat.Hp <= 0 )
	{
		IsDie = true;

		GetGgObjectManager().DestroyActor( OwningActor );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 피격 처리를 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::_ProcessHit( AActor* InOtherActor )
{
	auto othetObjectComp = InOtherActor ? InOtherActor->FindComponentByClass<UGgObjectComp>() : nullptr;
	if( !othetObjectComp )
		return;

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

	_ProcessCameraShake( InOtherActor );

	/*FString str = OwningActor->GetName() + TEXT( " : HitColl -> HP : " ) + FString::FromInt( (int)Stat.Hp );
	if ( GEngine )
		GEngine->AddOnScreenDebugMessage( -1, 3.0f, FColor::Yellow, str );*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 초기화 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::_Init()
{
	OwningActor = Cast<AActor>( GetOwner() );

	if( AGgCharacterPC* pc = Cast<AGgCharacterPC>( OwningActor ) )
	{
		if( const auto& pcInfo = GetGgDataInfoManager().GetPlayerDefaultStatInfos().Find( 0 ) )
		{
			Stat = pcInfo->Stat;
		}
	}
	else if( AGgCharacterNPC* npc = Cast<AGgCharacterNPC>( OwningActor ) )
	{
		if( const auto& npcInfo = GetGgDataInfoManager().GetNPCInfos().Find( npc->InfoId ) )
		{
			Stat = npcInfo->Stat;
		}
	}
	else if( AGgProjectile* proj = Cast<AGgProjectile>( OwningActor ) )
	{
		if( const auto& projInfo = GetGgDataInfoManager().GetProjectileInfos().Find( proj->InfoId ) )
		{
			Stat = projInfo->Stat;
		}
	}
	else if( AGgStaticObject* so = Cast<AGgStaticObject>( OwningActor ) )
	{
		if( const auto& soInfo = GetGgDataInfoManager().GetProjectileInfos().Find( so->InfoId ) )
		{
			Stat = soInfo->Stat;
		}
	}

	InitStat = Stat;

	auto hitColl = OwningActor ? Cast<UBoxComponent>( OwningActor->GetDefaultSubobjectByName( TEXT( "HitColl" ) ) ) : nullptr;
	if( hitColl )
		hitColl->OnComponentBeginOverlap.AddDynamic( this, &UGgObjectComp::HitCollBeginOverlap );

	SetMoveSpeed( Stat.MoveSpeed );
	SetJumpPower( Stat.JumpPower );
	
	if( GetTeamType() == ETeamType::MAX )
	{
		if( GetObjectType() == EObjectType::PC )
			SetTeamType( ETeamType::A );
		else 
			SetTeamType( ETeamType::NEUTRAL );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 물 히트 처리를 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgObjectComp::_ProcessWaterHit( AActor* InOtherActor )
{
	// 깊은 물에만 빠진다.
	EMaterialState matState = UtilMaterial::ConvertMatAssetToMatState( UtilMaterial::GetMatrialInterface( InOtherActor ) );
	if ( matState != EMaterialState::DEEPWATER )
		return;

	/// 가벼운 재질은 물에 빠지지 않는다.
	if( Stat.Weight <= 0.7f )
		return;

	/// 물은 물에 빠지지 않는다.
	auto charMatProperty = OwningActor ? OwningActor->FindComponentByClass<UGgMaterialComp>() : nullptr;
	if( charMatProperty )
	{
		if( charMatProperty->GetMatState() == EMaterialState::WATER ||
			charMatProperty->GetMatState() == EMaterialState::DEEPWATER )
			return;
	}

	IsFallWater = true;
}
