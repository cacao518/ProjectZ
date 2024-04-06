// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectComp.h"
#include "MaterialComp.h"
#include "WeaponComp.h"
#include "../ETC/SDB.h"
#include "../Actor/CharacterPC.h"
#include "../Actor/CharacterNPC.h"
#include "../Actor/StaticObject.h"
#include "../Actor/Projectile.h"
#include "../Manager/ObjectManager.h"
#include "../Manager/DataInfoManager.h"
#include "../Manager/CameraManager.h"
#include "../Manager/LockOnManager.h"
#include "../System/MonsterAIController.h"
#include "../System/MyGameInstance.h"
#include "../System/MyAnimInstance.h"
#include "../Util/UtilMaterial.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "WaterBodyComponent.h"
#include "LandscapeComponent.h"
#include "LandscapeProxy.h"
#include "Kismet/KismetMathLibrary.h"


UObjectComp::UObjectComp()
:
Id                    ( 0       ),
OwningActor           ( nullptr ),
TeamType              ( ETeamType::MAX ),
IsSpawnedInEditor     ( true    ),
IsDie                 ( false   ),
IsFallWater           ( false   )
{
	PrimaryComponentTick.bCanEverTick = true;
}

UObjectComp::~UObjectComp()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Begin
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::BeginPlay()
{
	Super::BeginPlay();

	if( IsSpawnedInEditor )
	{
		GetObjectManager().RegisterActorInEditor( OwningActor );
	}

	_Init();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Tick
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	_ProcessDie();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ����� �ٶ󺻴�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::LookAt( ACharacter* InTarget )
{
	if( !OwningActor || !InTarget )
		return;

	FRotator rotator = UKismetMathLibrary::FindLookAtRotation( OwningActor->GetActorLocation(), InTarget->GetActorLocation() );
	rotator.Pitch = OwningActor->GetActorRotation().Pitch;
	OwningActor->SetActorRotation( rotator );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ���� �ݸ��� ������ �����Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::SetAttackCollInfo( const FCollisionInfo& InAttackCollInfo )
{
	AttackCollInfo = InAttackCollInfo;

	auto attackColl = OwningActor ? Cast<UBoxComponent>( OwningActor->GetDefaultSubobjectByName( TEXT( "AttackColl" ) ) ) : nullptr;
	if( attackColl )
	{ 
		attackColl->SetBoxExtent( AttackCollInfo.Size );
		attackColl->SetRelativeLocation( AttackCollInfo.Pos );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief �̵��ӵ��� �����Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::SetMoveSpeed( float InMoveSpeed )
{
	Stat.MoveSpeed = InMoveSpeed;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief �������� �����Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::SetJumpPower( float InJumpPower )
{
	Stat.JumpPower = InJumpPower;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ���� �ݸ��� Ȱ��ȭ ���θ� �����Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::SetIsEnabledAttackColl( bool InIsEnabled )
{
	auto attackColl = OwningActor ? Cast<UBoxComponent>( OwningActor->GetDefaultSubobjectByName( TEXT( "AttackColl" ) ) ) : nullptr;
	if( attackColl )
	{ 
		attackColl->SetCollisionEnabled( InIsEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision );
		attackColl->SetVisibility( InIsEnabled );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ��Ʈ �ݸ��� Ȱ��ȭ ���θ� �����Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::SetIsEnabledHitColl( bool InIsEnabled )
{
	auto hitColl = OwningActor ? Cast<UBoxComponent>( OwningActor->GetDefaultSubobjectByName( TEXT( "HitColl" ) ) ) : nullptr;
	if( hitColl )
	{
		hitColl->SetCollisionEnabled( InIsEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ������Ʈ Ÿ���� �˾Ƴ���.
/////////////////////////////////////////////////////////////////////////////////////////////////////
EObjectType UObjectComp::GetObjectType()
{
	if( !OwningActor )
		return EObjectType::MAX;

	if( auto characterPC = Cast< ACharacterPC >( OwningActor ) )
		return EObjectType::PC;
	else if( auto characterNPC = Cast< ACharacterNPC >( OwningActor ) )
		return EObjectType::NPC;
	else if( auto staticObject = Cast< AStaticObject >( OwningActor ) )
		return EObjectType::STATIC_OBJECT;
	else if ( auto projectile = Cast< AProjectile >( OwningActor ) )
		return EObjectType::PROJECTILE;

	return EObjectType::MAX;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief �� �߷� ���� ���θ� ��ȯ�Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool UObjectComp::IsHeavyWeight()
{
	if ( Stat.Weight >= Const::HARD_RATE )
		return true;
	
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief �浹�� �����ҽÿ� ȣ��Ǵ� ��������Ʈ�� ����ϴ� �Լ�
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::HitCollBeginOverlap( UPrimitiveComponent* OverlappedComponent,
									   AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
									   bool bFromSweep, const FHitResult& SweepResult )
{
	if( !OwningActor || IsDie || IsFallWater ) 
		return;

	// �ڱ� �ڽ� �浹�� �����Ѵ�.
	if( Cast<AActor>( OtherActor ) == OwningActor ) 
		return;

	auto boxComponent = Cast<UBoxComponent>( OtherComp );
	if( boxComponent && boxComponent->GetName().Equals( TEXT( "AttackColl" ) ) )
	{
		_ProcessHit( OtherActor );
		return;
	}

	_ProcessWaterHit( OtherActor );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ī�޶� ����ũ ó���� �Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::_ProcessCameraShake( AActor* InOtherActor )
{
	// �� �÷��̾ �°ų�, ���� ��쿡�� ī�޶� ����ũ
	ACharacterPC* myPlayer = GetMyGameInstance().GetMyPlayer();
	if ( !myPlayer )
		return;

	if ( OwningActor == myPlayer || InOtherActor == myPlayer )
	{
		GetCameraManager().CameraShake( OwningActor );
		return;
	}

	// ������ ����ü�� �� ���� ��� ī�޶� ����ũ
	auto projectile = Cast< AProjectile >( InOtherActor );
	if ( !projectile )
		return;

	auto projObjComp = Cast<UObjectComp>( projectile->FindComponentByClass<UObjectComp>() );
	auto myPlayerObjComp = Cast<UObjectComp>( myPlayer->FindComponentByClass<UObjectComp>() );
	if ( projObjComp && myPlayerObjComp && myPlayerObjComp->GetTeamType() == projObjComp->GetTeamType() )
	{
		GetCameraManager().CameraShake( OwningActor );
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief ��� ���� ������ �����Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::_ProcessDie()
{
	if( !OwningActor )
		return;

	if( IsFallWater || Stat.Hp <= 0 )
	{
		IsDie = true;

		GetObjectManager().DestroyActor( OwningActor );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief �ǰ� ó���� �Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::_ProcessHit( AActor* InOtherActor )
{
	auto othetObjectComp = InOtherActor ? Cast<UObjectComp>( InOtherActor->FindComponentByClass<UObjectComp>() ) : nullptr;
	if( !othetObjectComp )
		return;

	if ( othetObjectComp->GetTeamType() == ETeamType::MAX || TeamType == ETeamType::MAX )
		return;

	if ( othetObjectComp->GetTeamType() == TeamType && !othetObjectComp->Stat.IsTyrant )
		return;

	othetObjectComp->OnAttackSuccess();

	// ü�� ����
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
//// @brief �ʱ�ȭ �Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::_Init()
{
	InitStat = Stat;

	OwningActor = Cast<AActor>( GetOwner() );

	auto hitColl = OwningActor ? Cast<UBoxComponent>( OwningActor->GetDefaultSubobjectByName( TEXT( "HitColl" ) ) ) : nullptr;
	if( hitColl )
		hitColl->OnComponentBeginOverlap.AddDynamic( this, &UObjectComp::HitCollBeginOverlap );

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
//// @brief �� ��Ʈ ó���� �Ѵ�.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UObjectComp::_ProcessWaterHit( AActor* InOtherActor )
{
	// ���� ������ ������.
	EMaterialState matState = UtilMaterial::ConvertMatAssetToMatState( UtilMaterial::GetMatrialInterface( InOtherActor ) );
	if ( matState != EMaterialState::DEEPWATER )
		return;

	/// ������ ������ ���� ������ �ʴ´�.
	if( Stat.Weight <= 0.7f )
		return;

	/// ���� ���� ������ �ʴ´�.
	auto charMatProperty = OwningActor ? Cast<UMaterialComp>( OwningActor->FindComponentByClass<UMaterialComp>() ) : nullptr;
	if( charMatProperty )
	{
		if( charMatProperty->GetMatState() == EMaterialState::WATER ||
			charMatProperty->GetMatState() == EMaterialState::DEEPWATER )
			return;
	}

	IsFallWater = true;
}