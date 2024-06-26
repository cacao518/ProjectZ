// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	static const FName PatrolPosKey;
	static const FName TargetKey;
	static const FName CurSkillNumKey;
	static const FName IsCommonActionKey;

private:
	class UBlackboardComponent* blackboard;

	UPROPERTY()
	class UBehaviorTree* BTAsset;

public:
	AMonsterAIController( FObjectInitializer const& object_initializer );

	virtual void OnPossess( APawn* InPawn ) override;

	void RunAI();
	void StopAI();
};
