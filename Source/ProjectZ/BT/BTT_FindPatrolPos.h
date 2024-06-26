// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UBTT_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, meta = ( ExposeOnSpawn = true ) )
	float PatrolRadius = 500.f;

public:
	UBTT_FindPatrolPos();

	virtual EBTNodeResult::Type ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory ) override;

};
