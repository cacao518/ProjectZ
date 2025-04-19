// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_Attack.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "Component/GgObjectComp.h"
#include "Component/GgCharacterComp.h"
#include "Components/BoxComponent.h"
#include "ProceduralMeshComponent.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 애님노티파이 시작
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotifyState_Attack::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	CurWorld = MeshComp->GetWorld();
	Owner = MeshComp->GetOwner();

	if( CurWorld && CurWorld->WorldType == EWorldType::EditorPreview )
		_DebugShape();
	
	UGgCharacterComp* charComp = MeshComp->GetOwner()->FindComponentByClass<UGgCharacterComp>();
	if( !charComp ) return;

	if( charComp->IsHold() )
	{
		return;
	}

	if( charComp->IsEnableAttackColl() )
	{
		return;
	}

	charComp->SetAttackCollData( Coll );
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

	_DebugShape();
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 히트 박스 모양을 그린다.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UAnimNotifyState_Attack::_DebugShape()
{
	if( !Owner.IsValid() ) return;

	UWorld* world = CurWorld;
	if( !world ) return;

	FVector scale = FVector( 1, 1, 1 );
	FVector pos = Owner->GetActorLocation();

	if( CurWorld->WorldType != EWorldType::EditorPreview )
	{
		// 캐릭터 메쉬의 스케일/위치 값 적용
		ACharacter* ownerChar = Cast<ACharacter>( Owner );
		if( ownerChar )
		{
			FVector meshOffset = ownerChar->GetMesh()->GetComponentLocation() - Owner->GetActorLocation();
			pos += meshOffset;
			scale = ownerChar->GetMesh()->GetRelativeScale3D();
		}
	}

	if( Coll.Shape == ECollShapeType::BOX )
	{
		FQuat rotationQuat = Owner->GetActorQuat() * Coll.Rotation.Quaternion();
		FVector boxPos     = pos + rotationQuat.RotateVector( Coll.Pos * scale );
		FVector boxSize    = Coll.Extent * scale;

		DrawDebugBox( world, boxPos, boxSize, rotationQuat, FColor::Red, false, 1.0f, 0, 2.0f );
	}
	else if( Coll.Shape == ECollShapeType::FAN )
	{
		float halfAngle = Coll.Angle * 0.5f;
		int numSegments = 10;

		float scaledRadius = Coll.Radius * FMath::Max( scale.X, scale.Y ); // 회전 평면이 XY이므로 XY 중 큰 값 사용
		float scaledHeight = Coll.Height * scale.Z; // 높이는 Z축 스케일 적용

		// 🔹 모든 회전값을 포함한 Rotator → Quat 변환
		FQuat RotationQuat = Owner->GetActorQuat() * Coll.Rotation.Quaternion();

		// 좌표에 회전 및 스케일 적용
		pos += RotationQuat.RotateVector( Coll.Pos * scale );

		// 🔹 부채꼴 방향 벡터 계산
		FVector BaseDirection = FVector( 1, 0, 0 ); // 기본 전방
		FVector LeftDirection = BaseDirection.RotateAngleAxis( -halfAngle, FVector::UpVector );
		FVector RightDirection = BaseDirection.RotateAngleAxis( halfAngle, FVector::UpVector );

		// 회전 적용
		FVector BottomLeft = pos + RotationQuat.RotateVector( LeftDirection ) * scaledRadius;
		FVector BottomRight = pos + RotationQuat.RotateVector( RightDirection ) * scaledRadius;

		FVector UpOffset = RotationQuat.RotateVector( FVector( 0, 0, scaledHeight ) );
		FVector TopLeft = BottomLeft + UpOffset;
		FVector TopRight = BottomRight + UpOffset;

		// 바닥 부채꼴 선
		DrawDebugLine( world, pos, BottomLeft, FColor::Green, false, 2.0f );
		DrawDebugLine( world, pos, BottomRight, FColor::Green, false, 2.0f );
		//DrawDebugLine( world, BottomLeft, BottomRight, FColor::Blue, false, 2.0f );

		// 윗면 부채꼴 선
		FVector TopCenter = pos + UpOffset;
		DrawDebugLine( world, TopCenter, TopLeft, FColor::Green, false, 2.0f );
		DrawDebugLine( world, TopCenter, TopRight, FColor::Green, false, 2.0f );
		//DrawDebugLine( world, TopLeft, TopRight, FColor::Blue, false, 2.0f );

		// 상하 연결선
		DrawDebugLine( world, BottomLeft, TopLeft, FColor::Yellow, false, 2.0f );
		DrawDebugLine( world, BottomRight, TopRight, FColor::Yellow, false, 2.0f );
		DrawDebugLine( world, pos, TopCenter, FColor::Yellow, false, 2.0f );

		// 부채꼴의 원호 부분 (위/아래)
		for( int i = 0; i < numSegments; i++ )
		{
			float Angle1 = -halfAngle + ( Coll.Angle / numSegments ) * i;
			float Angle2 = -halfAngle + ( Coll.Angle / numSegments ) * ( i + 1 );

			FVector Dir1 = FVector( 1, 0, 0 ).RotateAngleAxis( Angle1, FVector::UpVector );
			FVector Dir2 = FVector( 1, 0, 0 ).RotateAngleAxis( Angle2, FVector::UpVector );

			FVector Point1_Bottom = pos + RotationQuat.RotateVector( Dir1 ) * scaledRadius;
			FVector Point2_Bottom = pos + RotationQuat.RotateVector( Dir2 ) * scaledRadius;
			FVector Point1_Top = Point1_Bottom + UpOffset;
			FVector Point2_Top = Point2_Bottom + UpOffset;

			DrawDebugLine( world, Point1_Bottom, Point2_Bottom, FColor::Red, false, 2.0f );
			DrawDebugLine( world, Point1_Top, Point2_Top, FColor::Red, false, 2.0f );
			DrawDebugLine( world, Point1_Bottom, Point1_Top, FColor::Cyan, false, 2.0f );
		}
	}
}
