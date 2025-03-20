// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_SpawnActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Manager/GgObjectManager.h"
#include "Component/GgObjectComp.h"

FString UAnimNotify_SpawnActor::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_SpawnActor::SetProperty( AActor* InOwner )
{
	if ( !InOwner )
		return;

	UGgObjectComp* objComp = InOwner->FindComponentByClass<UGgObjectComp>();
	if ( !objComp )
		return;

	auto spawnPosComp = Cast<USceneComponent>( InOwner->GetDefaultSubobjectByName( TEXT( "SpawnPosComp" ) ) );
	if ( !spawnPosComp )
		return;

	FVector relativeSpawnPos = spawnPosComp->GetRelativeLocation() + Pos;
	FVector worldSpawnPos = UKismetMathLibrary::TransformLocation( spawnPosComp->GetComponentTransform(), relativeSpawnPos );

	ResultActor  = Actor;
	ResultPos    = worldSpawnPos;
	ResultRotate = InOwner->GetActorRotation() + Rotate;
}

void UAnimNotify_SpawnActor::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	if ( !FGgObjectManager::IsVaild() )
		return;

	AActor* owner = Cast<AActor>( MeshComp->GetOwner() );
	if ( !owner )
		return;

	UGgObjectComp* objComp = owner->FindComponentByClass<UGgObjectComp>();
	if ( !objComp )
		return;

	SetProperty( owner );

	if( SetAsParentTeamType )
		GetGgObjectManager().SpawnActor( ResultActor, ResultPos, ResultRotate, objComp->GetTeamType() );
	else
		GetGgObjectManager().SpawnActor( ResultActor, ResultPos, ResultRotate );
}