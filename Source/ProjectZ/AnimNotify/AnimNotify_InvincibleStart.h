// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_InvincibleStart.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UAnimNotify_InvincibleStart : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual FString GetNotifyName_Implementation() const override;

private:
	void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;
};
