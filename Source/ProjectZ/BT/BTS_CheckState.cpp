// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_CheckState.h"
#include "GameFramework/Character.h"
#include "System/GgAIController.h"
#include "Component/GgCharacterComp.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CheckState::UBTS_CheckState()
{
	NodeName = TEXT( "CheckState" );
	Interval = 0.1f;
}

void UBTS_CheckState::TickNode( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds )
{
	Super::TickNode( OwnerComp, NodeMemory, DeltaSeconds );

	APawn* controllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if( !controllingPawn )
		return;

	auto characterComp = controllingPawn ? controllingPawn->FindComponentByClass<UGgCharacterComp>() : nullptr;
	if( !characterComp )
		return;

	if( characterComp->GetAnimState() != EAnimState::IDLE_RUN && OwnerComp.GetBlackboardComponent()->GetValueAsBool( AGgAIController::IsIdleKey ) )
		OwnerComp.GetBlackboardComponent()->SetValueAsBool( AGgAIController::IsIdleKey, false );
	else if( characterComp->GetAnimState() == EAnimState::IDLE_RUN && !OwnerComp.GetBlackboardComponent()->GetValueAsBool( AGgAIController::IsIdleKey ) )
		OwnerComp.GetBlackboardComponent()->SetValueAsBool( AGgAIController::IsIdleKey, true );

	return;
}