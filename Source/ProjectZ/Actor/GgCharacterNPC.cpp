// Fill out your copyright notice in the Description page of Project Settings.


#include "GgCharacterNPC.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/GgCharacterComp.h"
#include "Component/GgFloatingBarComp.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "ProceduralMeshComponent.h"


// Sets default values
AGgCharacterNPC::AGgCharacterNPC()
{
	//AIControllerClass = AMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 회전을 부드럽게 하기 위한 코드
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Configure character movement
	GetCharacterMovement()->RotationRate = FRotator( 0.0f, CONST::DEFAULT_ROTATION_RATE, 0.0f );

	// CharacterComp
	CharacterComp = CreateDefaultSubobject<UGgCharacterComp>( TEXT( "CharacterComp" ) );

	// SpawnPosComp Component
	SpawnPosComp = CreateDefaultSubobject<USceneComponent>( TEXT( "SpawnPosComp" ) );
	SpawnPosComp->SetupAttachment( RootComponent );

	// LockOnMark Component
	static ConstructorHelpers::FClassFinder<UUserWidget> lockOnWidget( TEXT( "/Game/UI/WBP_LockOn" ) );
	LockOnMarkComp = CreateDefaultSubobject<UWidgetComponent>( TEXT( "LockOnMarkComp" ) );
	LockOnMarkComp->SetupAttachment( RootComponent );
	LockOnMarkComp->SetWidgetClass( lockOnWidget.Class );
	LockOnMarkComp->SetWidgetSpace( EWidgetSpace::Screen );
	LockOnMarkComp->SetDrawSize( FVector2D( 30.f, 30.f ) );

	// HpBar Component
	static ConstructorHelpers::FClassFinder<UUserWidget> floatingBarWidget( TEXT( "/Game/UI/WBP_FloatingBar" ) );
	FloatingBarComp = CreateDefaultSubobject<UGgFloatingBarComp>( TEXT( "FloatingBarComp" ) );
	FloatingBarComp->SetupAttachment( RootComponent );
	FloatingBarComp->SetWidgetClass( floatingBarWidget.Class );
	FloatingBarComp->SetWidgetSpace( EWidgetSpace::Screen );
	FloatingBarComp->SetRelativeLocation( FVector( 0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() ) );

	// Box Component
	HitColl = CreateDefaultSubobject<UBoxComponent>( TEXT( "HitColl" ) );
	HitColl->SetupAttachment( GetMesh() );
	HitColl->SetCollisionProfileName( TEXT( "HitColl" ) );

	// AttackBox Component
	AttackColl = CreateDefaultSubobject<UProceduralMeshComponent>( TEXT( "AttackColl" ) );
	AttackColl->SetupAttachment( GetMesh() );
	AttackColl->SetCollisionProfileName( TEXT( "AttackColl" ) );
}