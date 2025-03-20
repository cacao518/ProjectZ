// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_AttackEnd.h"
#include "Component/GgObjectComp.h"

FString UAnimNotify_AttackEnd::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_AttackEnd::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgObjectComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgObjectComp>();
	if( !obj ) return;

	obj->SetIsEnabledAttackColl( false );
}
