// Fill out your copyright notice in the Description page of Project Settings.


#include "GgAnimNotify_PlayNiagaraEffect.h"
#include "GameFramework/Character.h"


void UGgAnimNotify_PlayNiagaraEffect::SetProperty( AActor* InOwner )
{
	if( !InOwner )
		return;
}

void UGgAnimNotify_PlayNiagaraEffect::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	if( !MeshComp || !( MeshComp->GetOwner() ) )
		return;

	UWorld* world = MeshComp->GetWorld();
	if( !world ) return;

	if( world && world->WorldType != EWorldType::EditorPreview )
	{
		// 캐릭터 메쉬의 스케일 값 적용
		ACharacter* ownerChar = Cast<ACharacter>( MeshComp->GetOwner() );
		if( !ownerChar ) return;
		
		FVector scale = ownerChar->GetMesh()->GetRelativeScale3D();
		
		FVector originScale = Scale;

		Scale *= scale;

		Super::Notify( MeshComp, Animation, EventReference );

		Scale = originScale;
	}
	else
	{
		Super::Notify( MeshComp, Animation, EventReference );
	}
}