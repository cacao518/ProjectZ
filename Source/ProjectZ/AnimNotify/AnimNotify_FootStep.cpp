// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_FootStep.h"
#include "ETC/GgConst.h"
#include "Component/GgCharacterComp.h"
#include "Manager/GgObjectManager.h"
#include "Manager/GgDataInfoManager.h"
#include "Util/UtilMaterial.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

FString UAnimNotify_FootStep::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_FootStep::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	if ( !MeshComp )
		return;

	AActor* owner = Cast<AActor>( MeshComp->GetOwner() );
	if ( !owner )
		return;

	auto spawnPosComp = Cast<USceneComponent>( MeshComp->GetOwner()->GetDefaultSubobjectByName( TEXT( "SpawnPosComp" ) ) );
	if ( !spawnPosComp )
		return;

	FVector relativeSpawnPos = spawnPosComp->GetRelativeLocation() + FootLocation;
	FVector worldSpawnPos = UKismetMathLibrary::TransformLocation( spawnPosComp->GetComponentTransform(), relativeSpawnPos );

	EMaterialState matState = UtilMaterial::ConvertMatAssetToMatState( UtilMaterial::GetSteppedMatrialInterface( owner ) );
	const auto& matInfo = GetGgDataInfoManager().GetInfo<FMaterialInfo>( matState );
	if( !matInfo )
		return;

	//GetGgObjectManager().SpawnParticle( matInfo->FootStepParticleName, owner, worldSpawnPos, owner->GetActorRotation() );
}
