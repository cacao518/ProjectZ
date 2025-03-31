// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETC/SDB.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_Attack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UAnimNotifyState_Attack : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = ( ExposeOnSpawn = true ) )
	ECollShapeType Shape;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = ( ExposeOnSpawn = true ) )
	FVector Size;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = ( ExposeOnSpawn = true ) )
	FVector Pos;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = ( ExposeOnSpawn = true ) )
	float Power;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = ( ExposeOnSpawn = true ) )
	float KnockBackPower;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = ( ExposeOnSpawn = true ) )
	float HitStopTime;

private:
	UWorld* CurWorld = nullptr;

protected:
	virtual void NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference ) override;
	virtual void NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference ) override;

#if WITH_EDITOR
	void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent );
#endif

private:
	// 히트 박스 모양을 그린다.
	void _DebugShape( UWorld* InWorld );
};
