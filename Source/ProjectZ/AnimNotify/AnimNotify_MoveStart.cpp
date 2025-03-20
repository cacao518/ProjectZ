// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_MoveStart.h"
#include "Component/GgCharacterComp.h"

FString UAnimNotify_MoveStart::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_MoveStart::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgCharacterComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgCharacterComp>();
	if( !obj ) return;

	obj->SetMovePos( MovePower );
	obj->SetIsForceMove( true );
}