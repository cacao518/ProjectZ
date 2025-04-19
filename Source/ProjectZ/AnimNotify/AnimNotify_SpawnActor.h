// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "../ETC/SDB.h"
#include "AnimNotify_SpawnActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UAnimNotify_SpawnActor : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	UClass* Actor;                 

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify" )
	FVector Pos;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify" )
	FRotator Rotate;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "AnimNotify" )
	bool SetAsParentTeamType = true;

protected:
	UClass*  ResultActor;
	FVector  ResultPos;
	FRotator ResultRotate;

public:
	virtual FString GetNotifyName_Implementation() const override;

protected:
	virtual void SetProperty( AActor* InOwner );
	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;
};
