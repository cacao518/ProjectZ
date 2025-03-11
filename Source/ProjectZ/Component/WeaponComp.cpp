#pragma once

#include "WeaponComp.h"
#include "MaterialComp.h"
#include "CharacterComp.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Manager/DataInfoManager.h"

UWeaponComp::UWeaponComp()
{
	PrimaryComponentTick.bCanEverTick = true;
	OwningCharacter                   = nullptr;
	WeaponMesh                     = nullptr;
	DissolveMaterial                  = nullptr;
	DissovleMaterialInstance          = nullptr;

	ConstructorHelpers::FObjectFinder<UMaterialInterface> materialAsset( TEXT( "/Game/Shader/Dissolve_Inst.Dissolve_Inst" ) );
	if( materialAsset.Succeeded() ) DissolveMaterial = materialAsset.Object;
}

UWeaponComp::~UWeaponComp()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Begin
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UWeaponComp::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>( GetOwner() );

	_InitWeaponMesh();
	EquipWeapon( WeaponNum, false );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief Tick
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UWeaponComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if( DissolveAmount <= 0.1f )
	{ 
		_DissovleAnimEnd();
	}
	else
	{ 
		DissolveAmount = FMath::Lerp( DissolveAmount, 0.f, GetWorld()->GetDeltaSeconds() * 5.f );
		if( DissovleMaterialInstance )
			DissovleMaterialInstance->SetScalarParameterValue( FName( TEXT( "Amount" ) ), DissolveAmount );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 무기를 변경한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UWeaponComp::EquipWeapon( int32 InWeaponNum, bool InChangeAnim )
{
	if( WeaponMeshes.IsEmpty() )
		return;

	const auto& curWeaponInfo = GetDataInfoManager().GetWeaponInfos().Find( InWeaponNum );
	if( !curWeaponInfo )
		return;

	WeaponNum = InWeaponNum;
	WeaponType = curWeaponInfo->Type;
	WeaponMesh = WeaponMeshes.FindRef( InWeaponNum );

	if( !WeaponMesh )
		return;

	for( auto& [_, mesh] : WeaponMeshes )
		mesh->SetVisibility( false );

	WeaponMesh->SetVisibility( true );

	if( InChangeAnim )
	{ 
		DissolveAmount = 1.f;
		DissovleMaterialInstance = UMaterialInstanceDynamic::Create( DissolveMaterial, this );
		WeaponMesh->SetMaterial( 0, DissovleMaterialInstance );
	}
	else
	{
		_DissovleAnimEnd();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 무기를 해제한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UWeaponComp::UnEquipWeapon()
{
	const auto& curWeaponInfo = GetDataInfoManager().GetWeaponInfos().Find( WeaponNum );
	if( !curWeaponInfo )
		return;

	WeaponNum = 0;
	WeaponMesh = nullptr;
	WeaponType = EWeaponType::MAX;

	for( auto& [_, mesh] : WeaponMeshes )
		mesh->SetVisibility( false );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 보조 무기를 장착한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UWeaponComp::EquipSubWeapon()
{
	if( !SubWeaponMesh )
		return;

	SubWeaponMesh->SetVisibility( true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 보조 무기를 해제한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UWeaponComp::UnEquipSubWeapon()
{
	if( !SubWeaponMesh )
		return;

	SubWeaponMesh->SetVisibility( false );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 무기 메쉬 주소를 저장해놓는다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UWeaponComp::_InitWeaponMesh()
{
	for( const auto& [num, weaponInfo] : GetDataInfoManager().GetWeaponInfos() )
	{
		auto staticMesh = Cast<UStaticMeshComponent>( OwningCharacter->GetDefaultSubobjectByName( weaponInfo.ComponentName ) );
		if( staticMesh )
			WeaponMeshes.Add( num, staticMesh );
	}

	auto subWeaponMesh = Cast<UStaticMeshComponent>( OwningCharacter->GetDefaultSubobjectByName( TEXT( "SubBlade" ) ) );
	if( subWeaponMesh )
	{
		SubWeaponMesh = subWeaponMesh;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 디졸브 애니메이션을 종료 처리한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UWeaponComp::_DissovleAnimEnd()
{
	DissolveAmount = 0.f;
}
