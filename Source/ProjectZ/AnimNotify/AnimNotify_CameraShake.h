// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_CameraShake.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UAnimNotify_CameraShake : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float Scale = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify" )
	bool ShakeByWeight = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify" )
	bool ShakeByIntensity = false;

public:
	virtual FString GetNotifyName_Implementation() const override;

private:
	void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;
	
};
