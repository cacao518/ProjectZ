// Copyright Epic Games, Inc. All Rights Reserved.

#include "GgCharacterPC.h"
#include "Component/GgCharacterComp.h"
#include "Component/GgWeaponComp.h"
#include "Component/GgAirborneDecalComp.h"
#include "Animation/AnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "ProceduralMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"


AGgCharacterPC::AGgCharacterPC()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; 	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, CONST::DEFAULT_ROTATION_RATE, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>( TEXT( "CameraBoom" ) );
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = CONST::DEFAULT_TARGET_ARM_LENGTH;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>( TEXT( "FollowCamera" ) );
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Create a GameObejct Component
	CharacterComp = CreateDefaultSubobject<UGgCharacterComp>( TEXT( "CharacterComp" ) );

	// Create a WeaponComp Component
	WeaponComp = CreateDefaultSubobject<UGgWeaponComp>( TEXT( "WeaponComp" ) );

	// SpawnPosComp Component
	SpawnPosComp = CreateDefaultSubobject<USceneComponent>( TEXT( "SpawnPosComp" ) );
	SpawnPosComp->SetupAttachment( RootComponent );

	// AirborneMark Component
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> decalMat( TEXT( "/Game/Material/M_AirborneMark_MyPC" ) );
	AirborneDecalComp = CreateDefaultSubobject<UGgAirborneDecalComp>( TEXT( "AirborneDecalComp" ) );
	AirborneDecalComp->SetupAttachment( RootComponent );
	AirborneDecalComp->DecalSize = FVector( 10, 50, 50 );
	AirborneDecalComp->SetRelativeRotation( FRotator( 90, 0, 0 ) );
	AirborneDecalComp->SetDecalMaterial( decalMat.Object );

	// HitBox Component
	HitColl = CreateDefaultSubobject<UBoxComponent>( TEXT( "HitColl" ) );
	HitColl->SetupAttachment( GetMesh() );
	HitColl->SetCollisionProfileName( TEXT( "HitColl" ) );

	// AttackBox Component
	AttackColl = CreateDefaultSubobject<UProceduralMeshComponent>( TEXT( "AttackColl" ) );
	AttackColl->SetupAttachment( GetMesh() );
	AttackColl->SetCollisionProfileName( TEXT( "AttackColl" ) );
}
