// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_CheckRange.h"
#include "GameFramework/Character.h"
#include "Actor/GgCharacterNPC.h"
#include "System/GgAIController.h"
#include "Manager/GgDataInfoManager.h"
#include "Component/GgCharacterComp.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBTS_CheckRange::UBTS_CheckRange()
{
	NodeName = TEXT( "CheckRange" );
	Interval = 0.1f;
}

void UBTS_CheckRange::TickNode( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds )
{
	Super::TickNode( OwnerComp, NodeMemory, DeltaSeconds );

	AGgCharacterNPC* controllingNPC = Cast< AGgCharacterNPC> ( OwnerComp.GetAIOwner()->GetPawn() );
	if( !controllingNPC )
		return;

	ACharacter* target = Cast<ACharacter>( OwnerComp.GetBlackboardComponent()->GetValueAsObject( AGgAIController::TargetKey ) );
	if( !target )
		return;

	auto characterComp = controllingNPC ? controllingNPC->FindComponentByClass<UGgCharacterComp>() : nullptr;
	if( !characterComp )
		return;

	const auto& npcInfo = GetGgDataInfoManager().GetNPCInfos().Find( controllingNPC->InfoId );
	if( !npcInfo )
		return;

	for( const auto& skillNum : npcInfo->SkillInfos )
	{
		const auto& skillInfo = GetGgDataInfoManager().GetSkillInfos().Find( skillNum );
		if ( !skillInfo )
			continue;

		if( characterComp->IsCoolingSkill( skillNum ) )
			continue;

		if( target->GetDistanceTo( controllingNPC ) <= skillInfo->ActivateRangeMax &&
			target->GetDistanceTo( controllingNPC ) >= skillInfo->ActivateRangeMin )
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsInt( AGgAIController::CurSkillNumKey, skillNum );
			return;
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsInt( AGgAIController::CurSkillNumKey, 0 );
	return;
}