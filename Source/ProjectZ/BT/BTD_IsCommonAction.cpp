// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_IsCommonAction.h"
#include "GameFramework/Character.h"
#include "System/GgAIController.h"
#include "Component/GgCharacterComp.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_IsCommonAction::UBTD_IsCommonAction()
{
	NodeName = TEXT("IsCommonAction");
}

bool UBTD_IsCommonAction::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* controllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if( !controllingPawn )
		return false;

	auto characterComp = controllingPawn ? controllingPawn->FindComponentByClass<UGgCharacterComp>() : nullptr;
	if( !characterComp )
		return false;

	bool bResult = characterComp->GetAnimState() == EAnimState::COMMON_ACTION;
	return bResult;
}