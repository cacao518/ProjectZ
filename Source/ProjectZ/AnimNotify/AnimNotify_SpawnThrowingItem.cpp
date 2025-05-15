// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnThrowingItem.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Actor/GgProjectile.h"
#include "Manager/GgObjectManager.h"
#include "Manager/GgDataInfoManager.h"
#include "Manager/GgLockOnManager.h"
#include "Component/GgObjectComp.h"
#include "Component/GgWeaponComp.h"
#include "UObject/ConstructorHelpers.h"

FString UAnimNotify_SpawnThrowingItem::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_SpawnThrowingItem::SetProperty( AActor* InOwner )
{
	if ( !InOwner )
		return;

	/* Set Actor */
	UGgWeaponComp* weaponComp = InOwner->FindComponentByClass<UGgWeaponComp>();
	if ( !weaponComp )
		return;

	const auto& weaponInfo = GetGgDataInfoManager().GetWeaponInfos().Find( weaponComp->GetCurWeaponInfoId() );
	if ( !weaponInfo )
		return;

	UClass* thorwingBP = weaponInfo->ThorwingBPClass.LoadSynchronous();
	ResultActorClass = thorwingBP;

	/* Set Pos */
	UGgObjectComp* objComp = InOwner->FindComponentByClass<UGgObjectComp>();
	if ( !objComp )
		return;

	auto spawnPosComp = Cast<USceneComponent>( InOwner->GetDefaultSubobjectByName( TEXT( "SpawnPosComp" ) ) );
	if ( !spawnPosComp )
		return;

	FVector relativeSpawnPos = spawnPosComp->GetRelativeLocation() + Pos;
	FVector worldSpawnPos = UKismetMathLibrary::TransformLocation( spawnPosComp->GetComponentTransform(), relativeSpawnPos );

	ResultPos = worldSpawnPos;

	/* Set Rotate */
	ACharacter* lockOnTarget = GetGgLockOnManager().GetLockOnTarget().Get();
	if ( lockOnTarget )
		ResultRotate = UKismetMathLibrary::FindLookAtRotation( InOwner->GetActorLocation(), lockOnTarget->GetActorLocation() );
	else
		ResultRotate = InOwner->GetActorRotation() + Rotate;
}

void UAnimNotify_SpawnThrowingItem::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if ( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	AActor* owner = Cast<AActor>( MeshComp->GetOwner() );
	if ( !owner )
		return;

	UGgWeaponComp* weaponComp = Cast<UGgWeaponComp>( owner->FindComponentByClass<UGgWeaponComp>() );
	if ( !weaponComp )
		return;

	Super::Notify( MeshComp, Animation );

	weaponComp->UnEquipWeapon();
}