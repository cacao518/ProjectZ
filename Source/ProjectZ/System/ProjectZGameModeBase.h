// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectZGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API AProjectZGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AProjectZGameModeBase();
	virtual void BeginPlay() override final;
	virtual void BeginDestroy() override final;
	virtual void Tick( float InDeltaTime ) override final;
};
