// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ETC/GgConst.h"
#include "Animation/AnimInstance.h"
#include "GgAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UGgAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	float        Speed;                 // 이동속도   

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	bool         IsJump;                // 점프 중 여부

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	bool         IsAerial;              // 공중에 띄워짐 여부

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	bool         IsFly;                 // 날기 중 여부

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	bool         IsDie;                 // 사망 여부

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	EAnimState   AnimState;             // 애니메이션 상태

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	EAnimSubState   AnimSubState;       // 애니메이션 보조 상태

};
