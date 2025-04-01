// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_Move.h"
#include "Component/GgObjectComp.h"
#include "Component/GgCharacterComp.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 애님노티파이 시작
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotifyState_Move::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgCharacterComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgCharacterComp>();
	if( !obj ) return;

	obj->SetMovePos( MovePower );
	obj->SetIsForceMove( true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 애님노티파이 종료
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotifyState_Move::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyEnd( MeshComp, Animation, EventReference );

	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgCharacterComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgCharacterComp>();
	if( !obj ) return;

	obj->SetIsForceMove( false );
}