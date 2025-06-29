// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ETC/GgConst.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GgCharacterNPC.generated.h"


class UAnimMontage;


UCLASS()
class PROJECTZ_API AGgCharacterNPC final : public ACharacter
{
	GENERATED_BODY()

public:
	// NPC 인포 식별자
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	int InfoId;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	class UGgCharacterComp* CharacterComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = ( AllowPrivateAccess = "true" ) )
	class UGgFloatingBarComp* FloatingBarComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = ( AllowPrivateAccess = "true" ) )
	class USceneComponent* SpawnPosComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = ( AllowPrivateAccess = "true" ) )
	class UWidgetComponent* LockOnMarkComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = ( AllowPrivateAccess = "true" ) )
	class UGgAirborneDecalComp* AirborneDecalComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UBoxComponent* HitColl;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UProceduralMeshComponent* AttackColl;

public:
	// Sets default values for this character's properties
	AGgCharacterNPC();

};
