// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GgGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API AGgGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGgGameModeBase();
	virtual void BeginPlay() override final;
	virtual void BeginDestroy() override final;
	virtual void Tick( float InDeltaTime ) override final;
};
