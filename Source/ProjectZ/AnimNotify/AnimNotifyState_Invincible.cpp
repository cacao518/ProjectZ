// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_Invincible.h"
#include "Component/GgObjectComp.h"
#include "Component/GgCharacterComp.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 애님노티파이 시작
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotifyState_Invincible::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgCharacterComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgCharacterComp>();
	if( !obj )
		return;

	if( obj->IsHold() )
	{
		return;
	}

	obj->SetIsEnabledHitColl( false );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 애님노티파이 종료
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotifyState_Invincible::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyEnd( MeshComp, Animation, EventReference );

	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgCharacterComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgCharacterComp>();
	if( !obj )
		return;

	if( obj->IsHold() )
	{
		return;
	}

	obj->SetIsEnabledHitColl( true );
}