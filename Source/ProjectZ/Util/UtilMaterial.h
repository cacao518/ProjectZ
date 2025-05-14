#pragma once

#include "ETC/GgConst.h"


namespace UtilMaterial
{
	// Actor 발 밑에 있는 MaterialInterface을 알아낸다.
	UMaterialInterface* GetSteppedMatrialInterface( FActorPtr InActor );

	// Actor가 가지고 있는 MaterialInterface를 알아낸다.
	UMaterialInterface* GetMatrialInterface( FActorPtr InActor );

	// MaterialInterface를 EMaterialState로 바꿔준다.
	EMaterialState ConvertMatAssetToMatState( UMaterialInterface* InMaterial );
}