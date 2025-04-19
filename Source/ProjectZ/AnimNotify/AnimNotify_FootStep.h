// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ETC/SDB.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FootStep.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UAnimNotify_FootStep : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify" )
	FVector FootLocation;

public:
	virtual FString GetNotifyName_Implementation() const override;

private:
	void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;
};
