// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ETC/GgConst.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GgWeaponComp.generated.h"


using WeaponMeshMap = TMap< int, UStaticMeshComponent* >;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UGgWeaponComp final : public UActorComponent
{
	GENERATED_BODY()

public:
	// 무기 식별자 
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	int32                           WeaponInfoId;       	        

private:
	ACharacter*                     OwningCharacter;             // 부모 캐릭터 클래스

	EWeaponType                     WeaponType;                  // 무기 종류
	class UStaticMeshComponent*     WeaponMesh;                  // 현재 무기 스태틱매쉬
	class UStaticMeshComponent*     SubWeaponMesh;               // 보조 무기 스태틱매쉬

	class UMaterialInstanceDynamic* DissovleMaterialInstance;    // 디졸브 머티리얼 인스턴스
	UMaterialInterface*             DissolveMaterial;            // 디졸브 머티리얼 주소
	float                           DissolveAmount;              // 디졸브 변환 값

	WeaponMeshMap                   WeaponMeshes;                // 무기 스태틱 메시 애셋들

public:	
	// 생성자
	UGgWeaponComp();

	// 소멸자
	virtual ~UGgWeaponComp();

	// Begin 함수
	virtual void BeginPlay() override;

	// Tick 함수
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	
	///////////////////////////////////////////////////////////////////////
	// Setter
	///////////////////////////////////////////////////////////////////////
	
	// 무기를 변경한다.
	void EquipWeapon( int32 InWeaponInfoId, bool InChangeAnim = true );

	// 무기를 해제한다.
	void UnEquipWeapon();

	// 보조 무기를 장착한다.
	void EquipSubWeapon();

	// 보조 무기를 해제한다.
	void UnEquipSubWeapon();

	///////////////////////////////////////////////////////////////////////
	// Getter
	///////////////////////////////////////////////////////////////////////

	// 현재 무기 타입을 반환한다.
	EWeaponType GetWeaponType() { return WeaponType; };

	// 현재 무기 식별자를 반환한다.
	int GetCurWeaponInfoId() { return WeaponInfoId; };

	// 현재 무기 스태틱매쉬를 반환한다.
	UStaticMeshComponent* GetCurWeaponMesh() { return WeaponMesh; };

private:
	// 무기 메쉬 주소를 저장해놓는다.
	void _InitWeaponMesh();

	// 디졸브 애니메이션을 종료 처리한다.
	void _DissovleAnimEnd();
};
