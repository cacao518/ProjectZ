#pragma once

#include "GgMaterialComp.h"
#include "GgObjectComp.h"
#include "GgWeaponComp.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialInterface.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "WaterBodyComponent.h"
#include "LandscapeComponent.h"
#include "LandscapeProxy.h"
#include "NiagaraSystem.h"
#include "Actor/GgCharacterPC.h"
#include "Manager/GgDataInfoManager.h"
#include "Manager/GgObjectManager.h"
#include "System/GgAnimInstance.h"
#include "Util/UtilMaterial.h"

UGgMaterialComp::UGgMaterialComp()
{
	PrimaryComponentTick.bCanEverTick = true;
	OwningCharacter = nullptr;	
}

UGgMaterialComp::~UGgMaterialComp()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Begin
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgMaterialComp::BeginPlay()
{
	Super::BeginPlay();

	_Init();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Tick
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgMaterialComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	_ProcessGrass( DeltaTime );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 물질을 변경한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgMaterialComp::SetMatState( UMaterialInterface* InMatInterface, bool InIsInit )
{
	if( InIsInit )
		InMatInterface = InitMaterial;

	if ( !InMatInterface )
		return;

	EMaterialState matState = UtilMaterial::ConvertMatAssetToMatState( InMatInterface );
	if( matState == EMaterialState::DEFAULT || matState == MatState )
		return;

	if( !OwningCharacter )
		return;

	auto curMesh = OwningCharacter->GetMesh();
	if( !curMesh )
		return;

	curMesh->SetMaterial( 0, InMatInterface );

	auto weaponComp = OwningCharacter ? OwningCharacter->FindComponentByClass<UGgWeaponComp>() : nullptr;
	if( weaponComp && weaponComp->GetCurWeaponMesh() )
		weaponComp->GetCurWeaponMesh()->SetMaterial( 0, InMatInterface );
	
	MatState = matState;

	_InitStatus();

	if ( !InIsInit )
	{
		_PlayChangeEffect();

	/*	FString str = OwningCharacter->GetName() + TEXT( ": Material Change -> " ) + InMatInterface->GetName();
		if ( GEngine )
			GEngine->AddOnScreenDebugMessage( -1, 3.0f, FColor::Blue, str );*/
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 강도를 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
float UGgMaterialComp::GetIntensity()
{
	const auto& matInfo = GetGgDataInfoManager().GetInfo<FMaterialInfo>( MatState );
	if( matInfo )
	{
		return matInfo->Intensity;
	}

	return 1.f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 중간 강도 인지 여부를 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool UGgMaterialComp::IsMiddleIntensity()
{
	const auto& matInfo = GetGgDataInfoManager().GetInfo<FMaterialInfo>( MatState );
	if( matInfo )
	{
		if( matInfo->Intensity >= CONST::MIDDLE_RATE )
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 고 강도 인지 여부를 반환한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool UGgMaterialComp::IsHardIntensity()
{
	const auto& matInfo = GetGgDataInfoManager().GetInfo<FMaterialInfo>( MatState );
	if( matInfo )
	{
		if( matInfo->Intensity >= CONST::HARD_RATE )
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief  초기화 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgMaterialComp::_Init()
{
	MatState = EMaterialState::DEFAULT;
	
	OwningCharacter = Cast<ACharacter>( GetOwner() );

	SetMatState( nullptr, true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief  머티리얼에 맞는 능력치를 초기화한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgMaterialComp::_InitStatus()
{
	auto objectComp = OwningCharacter ? OwningCharacter->FindComponentByClass<UGgObjectComp>() : nullptr;
	if( !objectComp )
		return;

	auto characterMovement = OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr;
	if( !characterMovement )
		return;

	const auto& matInfo = GetGgDataInfoManager().GetInfo<FMaterialInfo>( MatState );
	if( matInfo )
	{
		objectComp->SetMoveSpeed   ( ( CONST::MAX_MASS - matInfo->Mass ) * objectComp->GetInitStat().MoveSpeed );
		objectComp->SetAttackSpeed ( ( CONST::MAX_MASS - matInfo->Mass ) * objectComp->GetInitStat().AttackSpeed );
		objectComp->SetJumpPower   ( ( CONST::MAX_MASS - matInfo->Mass ) * objectComp->GetInitStat().JumpPower );
		objectComp->SetAttackPower ( matInfo->Intensity              * objectComp->GetInitStat().AttackPower );
		objectComp->SetDefensePower( matInfo->Intensity              * objectComp->GetInitStat().DefensePower );
		objectComp->SetStrength    ( matInfo->Intensity              * objectComp->GetInitStat().Strength  );
		objectComp->SetWeight      ( matInfo->Mass                                                         );
		OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName( matInfo->CollisonName );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 물질 변경 이펙트를 실행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgMaterialComp::_PlayChangeEffect()
{
	const auto& matInfo = GetGgDataInfoManager().GetInfo<FMaterialInfo>( MatState );
	if( !matInfo )
		return;

	GetGgObjectManager().SpawnParticle( matInfo->ChangeParticleName, OwningCharacter, OwningCharacter->GetActorLocation(), OwningCharacter->GetActorRotation() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 풀재질 관련 로직을 실행한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UGgMaterialComp::_ProcessGrass( float InDeltaTime )
{
	if( MatState != EMaterialState::GRASS )
		return;

	UGgAnimInstance* animInstance = Cast< UGgAnimInstance >( OwningCharacter->GetMesh()->GetAnimInstance() );
	if( !animInstance )
		return;

	auto charMovement = OwningCharacter->GetCharacterMovement();
	if( !charMovement )
		return;

	if( animInstance->IsFly )
	{
		if( charMovement->Velocity.Z > 0 )
			charMovement->Velocity.Z = 0;

		charMovement->GravityScale = CONST::FLY_GRAVITY_SCALE;
	}
	else
		charMovement->GravityScale = CONST::DEFAULT_GRAVITY_SCALE;
}