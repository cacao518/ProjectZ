// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ETC/SDB.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GgProjectile.generated.h"


UCLASS()
class PROJECTZ_API AGgProjectile final : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	class UGgProjectileComp* ProjectileComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UBoxComponent* HitColl;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UProceduralMeshComponent* AttackColl;

public:
	// Sets default values for this character's properties
	AGgProjectile();

};
