// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_Attack.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Component/GgObjectComp.h"
#include "Component/GgCharacterComp.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 애님노티파이 시작
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotifyState_Attack::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;
	
	CurWorld = MeshComp->GetWorld();
	if( CurWorld && CurWorld->WorldType == EWorldType::EditorPreview )
	{
		_DebugShape( CurWorld );
	}

	UGgCharacterComp* charComp = MeshComp->GetOwner()->FindComponentByClass<UGgCharacterComp>();
	if( !charComp ) return;

	if( charComp->IsHold() )
	{
		return;
	}

	charComp->SetAttackCollData( FCollisionData( Shape, Size, Pos, Power, KnockBackPower, HitStopTime ) );
	charComp->SetIsEnabledAttackColl( true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 애님노티파이 종료
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotifyState_Attack::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyEnd( MeshComp, Animation, EventReference );

	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UGgCharacterComp* charComp = MeshComp->GetOwner()->FindComponentByClass<UGgCharacterComp>();
	if( !charComp ) return;

	if( charComp->IsHold() )
	{
		return;
	}

	charComp->SetIsEnabledAttackColl( false );
}

#if WITH_EDITOR
/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 에디터 편집창 디테일 패널 값이 바뀌었을 때 처리를 한다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotifyState_Attack::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
	Super::PostEditChangeProperty( PropertyChangedEvent );

	_DebugShape( CurWorld );
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 히트 박스 모양을 그린다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotifyState_Attack::_DebugShape( UWorld* InWorld )
{
	if( Shape == ECollShapeType::BOX )
	{
		DrawDebugBox( InWorld, Pos, Size, FColor::Red, false, 1.0f, 0, 2.0f );
	}
}