// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"
#include "ETC/GgConst.h"
#include "System/GgAIController.h"
#include "Component/GgCharacterComp.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Attack::UBTT_Attack()
{
	NodeName = TEXT( "Attack" );
}

EBTNodeResult::Type UBTT_Attack::ExecuteTask( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory )
{
	APawn* controllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if( !controllingPawn )
		return EBTNodeResult::Failed;

	ACharacter* target = Cast<ACharacter>( OwnerComp.GetBlackboardComponent()->GetValueAsObject( AGgAIController::TargetKey ) );
	if( !target )
		return EBTNodeResult::Failed;

	auto characterComp = controllingPawn ? controllingPawn->FindComponentByClass<UGgCharacterComp>() : nullptr;
	if( !characterComp )
		return EBTNodeResult::Failed;

	characterComp->LookAt( target );

	bool result = characterComp->SkillPlay( OwnerComp.GetBlackboardComponent()->GetValueAsInt( AGgAIController::CurSkillNumKey ) );

	OwnerComp.GetBlackboardComponent()->SetValueAsInt( AGgAIController::CurSkillNumKey, 0 );

	return result ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}