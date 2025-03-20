// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_MoveEnd.h"
#include "Component/GgCharacterComp.h"

FString UAnimNotify_MoveEnd::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_MoveEnd::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgCharacterComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgCharacterComp>();
	if( !obj ) return;

	obj->SetIsForceMove( false );
}