// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotify_PlayNiagaraEffect.h"
#include "GgAnimNotify_PlayNiagaraEffect.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTZ_API UGgAnimNotify_PlayNiagaraEffect : public UAnimNotify_PlayNiagaraEffect
{
	GENERATED_BODY()

protected:
	virtual void SetProperty( AActor* InOwner );
	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference ) override;
};
