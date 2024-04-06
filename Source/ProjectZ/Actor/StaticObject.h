// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../ETC/SDB.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StaticObject.generated.h"


UCLASS()
class PROJECTZ_API AStaticObject final : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	class UObjectComp* ObjectComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UBoxComponent* HitColl;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UBoxComponent* AttackColl;

public:
	// Sets default values for this character's properties
	AStaticObject();

};
