// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ETC/SDB.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GgStaticObject.generated.h"


UCLASS()
class PROJECTZ_API AGgStaticObject final : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	class UGgObjectComp* ObjectComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UBoxComponent* HitColl;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UBoxComponent* AttackColl;

public:
	// Sets default values for this character's properties
	AGgStaticObject();

};
