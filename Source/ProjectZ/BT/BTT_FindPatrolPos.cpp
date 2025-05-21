// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FindPatrolPos.h"
#include "System/GgAIController.h"
#include "Actor/GgCharacterNPC.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Manager/GgDataInfoManager.h"
#include "NavigationSystem.h"

UBTT_FindPatrolPos::UBTT_FindPatrolPos()
{
	NodeName = TEXT( "FindPatrolPos" );
}

EBTNodeResult::Type UBTT_FindPatrolPos::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	APawn* controllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if( !controllingPawn )
		return EBTNodeResult::Failed;

	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetNavigationSystem( controllingPawn->GetWorld() );
	if( !navSystem )
		return EBTNodeResult::Failed;

	AGgCharacterNPC* controllingCharacterNPC = Cast< AGgCharacterNPC >( OwnerComp.GetAIOwner()->GetPawn() );
	if( !controllingCharacterNPC )
		return EBTNodeResult::Failed;

	const auto& npcInfo = GetGgDataInfoManager().GetInfo<FNPCInfo>( controllingCharacterNPC->InfoId );
	if( !npcInfo )
		return EBTNodeResult::Failed;

	PatrolRadius = npcInfo->PatrolRange;
	FVector origin = controllingCharacterNPC->GetActorLocation();
	FNavLocation nextPatrol;
	if( navSystem->GetRandomPointInNavigableRadius( origin, PatrolRadius, nextPatrol ) )
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector( AGgAIController::PatrolPosKey, nextPatrol.Location );
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}