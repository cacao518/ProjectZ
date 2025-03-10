// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../ETC/SDB.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComp.generated.h"


using WeaponMeshMap     = TMap< int, UStaticMeshComponent* >;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTZ_API UWeaponComp final : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay )
	int32                           WeaponNum;       	         // ���� �ĺ��� 

private:
	ACharacter*                     OwningCharacter;             // �θ� ĳ���� Ŭ����

	EWeaponType                     WeaponType;                  // ���� ����
	class UStaticMeshComponent*     WeaponMesh;                  // ���� ���� ����ƽ�Ž�
	class UStaticMeshComponent*     SubWeaponMesh;               // ���� ���� ����ƽ�Ž�

	class UMaterialInstanceDynamic* DissovleMaterialInstance;    // ������ ��Ƽ���� �ν��Ͻ�
	UMaterialInterface*             DissolveMaterial;            // ������ ��Ƽ���� �ּ�
	float                           DissolveAmount;              // ������ ��ȯ ��

	WeaponMeshMap                   WeaponMeshes;                // ���� ����ƽ �޽� �ּµ�

public:	
	// ������
	UWeaponComp();

	// �Ҹ���
	virtual ~UWeaponComp();

	// Begin �Լ�
	virtual void BeginPlay() override;

	// Tick �Լ�
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	
	///////////////////////////////////////////////////////////////////////
	// Setter
	///////////////////////////////////////////////////////////////////////
	
	// ���⸦ �����Ѵ�.
	void EquipWeapon( int32 InWeaponNum, bool InChangeAnim = true );

	// ���⸦ �����Ѵ�.
	void UnEquipWeapon();

	///////////////////////////////////////////////////////////////////////
	// Getter
	///////////////////////////////////////////////////////////////////////

	// ���� ���� Ÿ���� ��ȯ�Ѵ�.
	EWeaponType GetWeaponType() { return WeaponType; };

	// ���� ���� �ĺ��ڸ� ��ȯ�Ѵ�.
	int GetCurWeaponNum() { return WeaponNum; };

	// ���� ���� ����ƽ�Ž��� ��ȯ�Ѵ�.
	UStaticMeshComponent* GetCurWeaponMesh() { return WeaponMesh; };

private:
	// ���� �޽� �ּҸ� �����س��´�.
	void _InitWeaponMesh();

	// ������ �ִϸ��̼��� ���� ó���Ѵ�.
	void _DissovleAnimEnd();
};
