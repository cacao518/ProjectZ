// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../ETC/SDB.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterNPC.generated.h"


class UAnimMontage;


UCLASS()
class PROJECTZ_API ACharacterNPC final : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	class UCharacterComp* CharacterComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = ( AllowPrivateAccess = "true" ) )
	class USceneComponent* SpawnPosComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = ( AllowPrivateAccess = "true" ) )
	class UWidgetComponent* LockOnMarkComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = ( AllowPrivateAccess = "true" ) )
	class UFloatingBarComp* FloatingBarComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UBoxComponent* HitColl;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UBoxComponent* AttackColl;

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	TArray<int> SkillInfos;             

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	float PatrolRange;

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	float DetectRange;

public:
	// Sets default values for this character's properties
	ACharacterNPC();

};