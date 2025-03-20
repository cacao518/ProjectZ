// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_AttackStart.h"
#include "Component/GgObjectComp.h"

FString UAnimNotify_AttackStart::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_AttackStart::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgObjectComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgObjectComp>();
	if( !obj ) return;

	obj->SetAttackCollData( FCollisionData( Size, Pos, Power, KnockBackPower ) );
	obj->SetIsEnabledAttackColl( true );
}
