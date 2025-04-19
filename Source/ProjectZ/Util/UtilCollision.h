#pragma once

#include "../ETC/SDB.h"
#include "ProceduralMeshComponent.h"


namespace UtilCollision
{
	// UProceduralMeshComponent 를 CollisionData에 맞게 콜리전을 설정한다.
	void SetProceduralMeshCollision( UProceduralMeshComponent* InPMC, const FCollisionData& InCollData, bool InDebugShape = false );
}