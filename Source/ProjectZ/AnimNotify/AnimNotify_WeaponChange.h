// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETC/GgConst.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_WeaponChange.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UAnimNotify_WeaponChange : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify" )
	int32 WeaponNum;

public:
	virtual FString GetNotifyName_Implementation() const override;

private:
	void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;
};
