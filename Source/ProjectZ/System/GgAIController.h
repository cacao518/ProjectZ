// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GgAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API AGgAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	static const FName PatrolPosKey;
	static const FName TargetKey;
	static const FName CurSkillNumKey;
	static const FName IsIdleKey;

private:
	class UBlackboardComponent* blackboard;

	UPROPERTY()
	class UBehaviorTree* BTAsset;

public:
	AGgAIController( FObjectInitializer const& object_initializer );

	virtual void OnPossess( APawn* InPawn ) override;

	void RunAI();
	void StopAI();
};
