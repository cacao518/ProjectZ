// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Launch.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UAnimNotify_Launch : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify" )
	FVector Vec;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify" )
	bool bUseOwnerBaseDir = false;

public:
	virtual FString GetNotifyName_Implementation() const override;

private:
	void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;
	
};
