// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_InvincibleEnd.h"
#include "Component/GgObjectComp.h"

FString UAnimNotify_InvincibleEnd::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_InvincibleEnd::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgObjectComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgObjectComp>();
	if( !obj )
		return;

	obj->SetIsEnabledHitColl( true );
}
