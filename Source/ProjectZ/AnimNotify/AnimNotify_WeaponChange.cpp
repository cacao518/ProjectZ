// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_WeaponChange.h"
#include "Component/GgWeaponComp.h"

FString UAnimNotify_WeaponChange::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_WeaponChange::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgWeaponComp* weaponComp = MeshComp->GetOwner()->FindComponentByClass<UGgWeaponComp>();
	if( !weaponComp ) return;

	weaponComp->EquipWeapon( WeaponNum );
}