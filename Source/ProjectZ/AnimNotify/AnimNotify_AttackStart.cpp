// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_AttackStart.h"
#include "Engine/World.h"
#include "Component/GgObjectComp.h"

FString UAnimNotify_AttackStart::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_AttackStart::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	if( GetWorld() && GetWorld()->WorldType == EWorldType::EditorPreview )
	{
		CurWorld = GetWorld();
		_DebugShape( CurWorld );
	}

	UGgObjectComp* obj = MeshComp->GetOwner()->FindComponentByClass<UGgObjectComp>();
	if( !obj ) return;

	obj->SetAttackCollData( FCollisionData( Shape, Size, Pos, Power, KnockBackPower, HitStopTime ) );
	obj->SetIsEnabledAttackColl( true );
}

#if WITH_EDITOR
void UAnimNotify_AttackStart::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
	Super::PostEditChangeProperty( PropertyChangedEvent );

	_DebugShape( CurWorld );
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 히트 박스 모양을 그린다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotify_AttackStart::_DebugShape( UWorld* InWorld )
{
	if( Shape == ECollShapeType::BOX )
	{
		DrawDebugBox( InWorld, Pos, Size, FColor::Red, false, 1.0f, 0, 2.0f );
	}
}