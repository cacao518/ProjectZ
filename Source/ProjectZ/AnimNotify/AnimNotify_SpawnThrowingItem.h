// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotify_SpawnActor.h"
#include "AnimNotify_SpawnThrowingItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UAnimNotify_SpawnThrowingItem : public UAnimNotify_SpawnActor
{
	GENERATED_BODY()

public:
	virtual FString GetNotifyName_Implementation() const override;

protected:
	virtual void SetProperty( AActor* InOwner ) override;
	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;
};
