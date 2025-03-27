// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETC/SDB.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackStart.generated.h"


class UWorld;


UCLASS()
class PROJECTZ_API UAnimNotify_AttackStart : public UAnimNotify
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

public:
	virtual FString GetNotifyName_Implementation() const override;

protected:
	void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;

#if WITH_EDITOR
	void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent );
#endif

private:
	// 히트 박스 모양을 그린다.
	void _DebugShape( UWorld* InWorld );
};
