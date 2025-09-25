
#include "UtilCollision.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "System/GgGameInstance.h"
#include "Manager/GgDataInfoManager.h"

namespace UtilCollision
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//// @brief  UProceduralMeshComponent 를 CollisionData에 맞게 콜리전을 설정한다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetProceduralMeshCollision( UProceduralMeshComponent* InPMC, const FCollisionData& InCollData, bool InDebugShape /*= false*/ )
	{
		if( !InPMC ) return;

		if( InCollData.Shape == ECollShapeType::BOX )
		{
			InPMC->ClearAllMeshSections();
			InPMC->ClearCollisionConvexMeshes();

			FQuat rotationQuat = InCollData.Rotation.Quaternion();
			FVector boxPos = rotationQuat.RotateVector( InCollData.Pos );
			FVector boxExtent = InCollData.Extent;

			// 기준 회전과 위치를 하나의 FTransform으로
			FTransform boxTransform( rotationQuat, boxPos );

			// 로컬 기준 8개 꼭짓점
			const FVector localCorners[ 8 ] = {
				FVector( -1, -1, -1 ),
				FVector( 1, -1, -1 ),
				FVector( 1,  1, -1 ),
				FVector( -1,  1, -1 ),
				FVector( -1, -1,  1 ),
				FVector( 1, -1,  1 ),
				FVector( 1,  1,  1 ),
				FVector( -1,  1,  1 ),
			};

			TArray<FVector> ConvexVerts;
			for( int32 i = 0; i < 8; ++i )
			{
				FVector corner = localCorners[ i ] * boxExtent;
				ConvexVerts.Add( boxTransform.TransformPosition( corner ) );
			}

			// 충돌용 Convex Mesh 추가
			InPMC->bUseComplexAsSimpleCollision = false;
			InPMC->AddCollisionConvexMesh( ConvexVerts );

#if WITH_EDITOR
			if( InDebugShape )
			{
				TArray<FVector> Vertices = ConvexVerts;
				TArray<int32> Triangles;

				// 단순한 시각화용 삼각형 (12개 삼각형)
				const int32 indices[ 36 ] = {
					0,2,1, 0,3,2, // 아래
					4,5,6, 4,6,7, // 위
					0,1,5, 0,5,4, // 앞
					2,3,7, 2,7,6, // 뒤
					1,2,6, 1,6,5, // 오른쪽
					3,0,4, 3,4,7  // 왼쪽
				};
				for( int32 i = 0; i < 36; ++i )
				{
					Triangles.Add( indices[ i ] );
				}

				TArray<FVector> Normals;
				TArray<FVector2D> UV0;
				TArray<FLinearColor> VertexColors;
				TArray<FProcMeshTangent> Tangents;

				Normals.SetNumZeroed( Vertices.Num() );
				UV0.SetNumZeroed( Vertices.Num() );
				VertexColors.Init( FLinearColor::White, Vertices.Num() );
				Tangents.SetNumZeroed( Vertices.Num() );

				InPMC->CreateMeshSection_LinearColor( 0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, false );
			}
#endif
		}
		else if( InCollData.Shape == ECollShapeType::FAN )
		{
			InPMC->ClearAllMeshSections();
			InPMC->ClearCollisionConvexMeshes();

			int numSegments = 10;
			TArray<FVector> Vertices;

			float halfAngleRad = FMath::DegreesToRadians( InCollData.Angle * 0.5f );
			float angleStepDeg = InCollData.Angle / numSegments;
			FQuat RotQuat = InCollData.Rotation.Quaternion();

			// 1. 바닥 중심점 (로컬 → 회전 → 위치 반영)
			FVector center = RotQuat.RotateVector( FVector( 0, 0, 0 ) + InCollData.Pos );
			Vertices.Add( center );

			// 2. 부채꼴 외곽 점들 (바닥)
			for( int32 i = 0; i <= numSegments; ++i )
			{
				float angle = -halfAngleRad + i * FMath::DegreesToRadians( angleStepDeg );
				float x = InCollData.Radius * FMath::Cos( angle );
				float y = InCollData.Radius * FMath::Sin( angle );
				FVector point = FVector( x, y, 0 );
				Vertices.Add( RotQuat.RotateVector( point + InCollData.Pos ) );
			}

			// 3. 윗면 점 추가 (Height 위로 올림)
			int32 baseVertexCount = Vertices.Num();
			for( int32 i = 0; i < baseVertexCount; ++i )
			{
				FVector topVertex = Vertices[ i ] + RotQuat.RotateVector( FVector( 0, 0, InCollData.Height ) );
				Vertices.Add( topVertex );
			}

			InPMC->bUseComplexAsSimpleCollision = false;
			InPMC->AddCollisionConvexMesh( Vertices );

#if WITH_EDITOR
			if( InDebugShape )
			{
				TArray<int32> Triangles;

				// 1. 바닥 삼각형 인덱스
				for( int32 i = 1; i <= numSegments; ++i )
				{
					Triangles.Add( 0 );
					Triangles.Add( i );
					Triangles.Add( i + 1 );
				}

				// 2. 옆면 삼각형 (부채꼴 벽면)
				for( int32 i = 1; i <= numSegments; ++i )
				{
					int32 bl = i;
					int32 br = i + 1;
					int32 tl = bl + baseVertexCount;
					int32 tr = br + baseVertexCount;

					// 쿼드 → 삼각형 2개
					Triangles.Add( bl );
					Triangles.Add( tl );
					Triangles.Add( tr );

					Triangles.Add( bl );
					Triangles.Add( tr );
					Triangles.Add( br );
				}

				// 3. 위쪽 부채꼴 삼각형도 추가 (옵션)
				// 상단 중심 인덱스는 baseVertexCount (== 위쪽 부채꼴 중심)
				for( int32 i = 1; i <= numSegments; ++i )
				{
					Triangles.Add( baseVertexCount ); // 윗면 중심
					Triangles.Add( baseVertexCount + i + 1 );
					Triangles.Add( baseVertexCount + i );
				}

				// 4. 부가 정보
				TArray<FVector> Normals;
				TArray<FVector2D> UV0;
				TArray<FLinearColor> VertexColors;
				TArray<FProcMeshTangent> Tangents;

				Normals.SetNumZeroed( Vertices.Num() );
				UV0.SetNumZeroed( Vertices.Num() );
				VertexColors.Init( FLinearColor::White, Vertices.Num() );
				Tangents.SetNumZeroed( Vertices.Num() );

				// 5. 메쉬 생성
				InPMC->CreateMeshSection_LinearColor( 0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, false );
			}
#endif
		}
		else if( InCollData.Shape == ECollShapeType::CYLINDER )
		{
			InPMC->ClearAllMeshSections();
			InPMC->ClearCollisionConvexMeshes();

			TArray<FVector> Vertices;

			int numSegments = 10;
			float angleStepRad = 2 * PI / numSegments;
			FQuat RotQuat = InCollData.Rotation.Quaternion();

			// 중심점
			FVector bottomCenter = RotQuat.RotateVector( FVector( 0, 0, 0 ) + InCollData.Pos );
			FVector topCenter = RotQuat.RotateVector( FVector( 0, 0, InCollData.Height ) + InCollData.Pos );

			Vertices.Add( bottomCenter ); // 0
			Vertices.Add( topCenter );    // 1

			int32 baseIndex = Vertices.Num();

			// 원 둘레 점 생성
			for( int32 i = 0; i <= numSegments; ++i )
			{
				float angle = i * angleStepRad;
				float x = InCollData.Radius * FMath::Cos( angle );
				float y = InCollData.Radius * FMath::Sin( angle );
				FVector bottom = RotQuat.RotateVector( FVector( x, y, 0 ) + InCollData.Pos );
				FVector top = RotQuat.RotateVector( FVector( x, y, InCollData.Height ) + InCollData.Pos );

				Vertices.Add( bottom );  // i*2 + 2
				Vertices.Add( top );     // i*2 + 3
			}

			// 충돌용 Convex 설정
			InPMC->bUseComplexAsSimpleCollision = false;
			InPMC->AddCollisionConvexMesh( Vertices );

#if WITH_EDITOR
			if( InDebugShape )
			{
				TArray<int32> Triangles;

				// 바닥 삼각형
				for( int32 i = 0; i < numSegments; ++i )
				{
					int32 bi = baseIndex + i * 2;
					int32 bi_next = baseIndex + ( ( i + 1 ) % numSegments ) * 2;

					Triangles.Add( 0 );
					Triangles.Add( bi_next );
					Triangles.Add( bi );
				}

				// 윗면 삼각형
				for( int32 i = 0; i < numSegments; ++i )
				{
					int32 ti = baseIndex + i * 2 + 1;
					int32 ti_next = baseIndex + ( ( i + 1 ) % numSegments ) * 2 + 1;

					Triangles.Add( 1 );
					Triangles.Add( ti );
					Triangles.Add( ti_next );
				}

				// 옆면 (사각형 2개 → 삼각형 2개)
				for( int32 i = 0; i < numSegments; ++i )
				{
					int32 b0 = baseIndex + i * 2;
					int32 t0 = b0 + 1;
					int32 b1 = baseIndex + ( ( i + 1 ) % numSegments ) * 2;
					int32 t1 = b1 + 1;

					Triangles.Add( b0 );
					Triangles.Add( t0 );
					Triangles.Add( t1 );

					Triangles.Add( b0 );
					Triangles.Add( t1 );
					Triangles.Add( b1 );
				}

				// 부가 정보
				TArray<FVector> Normals;
				TArray<FVector2D> UV0;
				TArray<FLinearColor> VertexColors;
				TArray<FProcMeshTangent> Tangents;

				Normals.SetNumZeroed( Vertices.Num() );
				UV0.SetNumZeroed( Vertices.Num() );
				VertexColors.Init( FLinearColor::White, Vertices.Num() );
				Tangents.SetNumZeroed( Vertices.Num() );

				// 메쉬 생성
				InPMC->CreateMeshSection_LinearColor( 0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, false );
			}
#endif
		}
		else
		{
			InPMC->ClearAllMeshSections();
			InPMC->ClearCollisionConvexMeshes();
		}
	}
}