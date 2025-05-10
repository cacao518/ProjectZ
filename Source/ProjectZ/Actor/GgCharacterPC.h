// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ETC/GgConst.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GgCharacterPC.generated.h"


class UAnimMontage;
class USkeletalMeshComponent;


UCLASS(config=Game)
class AGgCharacterPC final : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	class UGgCharacterComp* CharacterComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Gameplay, meta = ( AllowPrivateAccess = "true" ) )
	class UGgWeaponComp* WeaponComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = ( AllowPrivateAccess = "true" ) )
	class USceneComponent* SpawnPosComp;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UBoxComponent* HitColl;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = ( AllowPrivateAccess = "true" ) )
	class UProceduralMeshComponent* AttackColl;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	AGgCharacterPC();

};
