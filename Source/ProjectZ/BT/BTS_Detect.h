// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_Detect.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UBTS_Detect : public UBTService
{
	GENERATED_BODY()
	
		
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( ExposeOnSpawn = true ) )
	float DetectRadius = 600.f;

public:
	UBTS_Detect();

	virtual void TickNode( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds ) override;
};
