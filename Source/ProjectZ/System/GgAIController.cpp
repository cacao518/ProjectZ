// Fill out your copyright notice in the Description page of Project Settings.


#include "GgAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"


const FName AGgAIController::PatrolPosKey( TEXT( "PatrolPos" ) );
const FName AGgAIController::TargetKey( TEXT( "Target" ) );
const FName AGgAIController::CurSkillNumKey( TEXT( "CurSkillNum" ) );
const FName AGgAIController::IsIdleKey( TEXT( "IsIdle" ) );


AGgAIController::AGgAIController( FObjectInitializer const& object_initializer )
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject( TEXT( "BehaviorTree'/Game/Blueprint/Monster/BT_Monster.BT_Monster'" ) );
	if( BTObject.Succeeded() )
	{
		BTAsset = BTObject.Object;
	}

	blackboard = object_initializer.CreateDefaultSubobject<UBlackboardComponent>( this, TEXT( "BlackboardComp" ) );
}

void AGgAIController::OnPossess( APawn* InPawn )
{
	Super::OnPossess( InPawn );
	if( blackboard )
	{
		//비헤이비어트리에 있는 블랙보드로 초기화
		blackboard->InitializeBlackboard( *BTAsset->BlackboardAsset );
	}

	RunAI();
}

void AGgAIController::RunAI()
{
	RunBehaviorTree( BTAsset );
}

void AGgAIController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>( BrainComponent );
	if( !BehaviorTreeComponent )
		return;

	BehaviorTreeComponent->StopTree( EBTStopMode::Safe );
}