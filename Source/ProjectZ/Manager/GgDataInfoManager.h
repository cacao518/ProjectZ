#pragma once

#include "ETC/GgConst.h"
#include "GgSingletonBase.h"
#include "CoreMinimal.h"
#include <unordered_map>
#include <list>


using PlayerDefaultSkillInfoMap = TMap< EInputKeyType,  FPlayerDefaultSkillInfo >;
using PlayerWeaponSkillInfoMap  = TMap< int,            FPlayerWeaponSkillInfo >;
using PlayerDefaultStatInfoMap  = TMap< int,            FPlayerDefaultStatInfo >;
using MaterialInfoMap           = TMap< EMaterialState, FMaterialInfo >;
using WeaponInfoMap             = TMap< int,            FWeaponInfo >;
using SkillInfoMap              = TMap< int,            FSkillInfo >;
using NPCInfoMap                = TMap< int,            FNPCInfo >;
using ProjectileInfoMap         = TMap< int,            FProjectileInfo >;
using StaticObjectInfoMap       = TMap< int,            FStaticObjectInfo >;


class FGgDataInfoManager final : public FGgSingletonBase< FGgDataInfoManager >
{
private:
	PlayerDefaultSkillInfoMap PlayerDefaultSkillInfos;  // 플레이어 기본 스킬 정보
	PlayerWeaponSkillInfoMap  PlayerWeaponSkillInfos;   // 플레이어 무기 스킬 정보
	PlayerDefaultStatInfoMap  PlayerDefaultStatInfos;   // 플레이어 기본 스텟 정보

	MaterialInfoMap           MaterialInfos;            // 물질 정보
	WeaponInfoMap             WeaponInfos;              // 무기 정보
	SkillInfoMap              SkillInfos;               // 스킬 정보
	NPCInfoMap                NPCInfos;                 // NPC 정보
	ProjectileInfoMap         ProjectileInfos;          // 투사체 정보
	StaticObjectInfoMap       StaticObjectInfos;        // 정적 오브젝트 정보

public:
	FGgDataInfoManager();
	~FGgDataInfoManager();

	// 데이터 생성
	void DataCreate();

	///////////////////////////////////////////////////////////////
	/// Get
	///////////////////////////////////////////////////////////////

	const PlayerDefaultSkillInfoMap& GetPlayerDefaultSkillInfos() { return PlayerDefaultSkillInfos; };
	const PlayerWeaponSkillInfoMap& GetPlayerWeaponSkillInfos() { return PlayerWeaponSkillInfos; };
	const PlayerDefaultStatInfoMap& GetPlayerDefaultStatInfos() { return PlayerDefaultStatInfos; };
	const MaterialInfoMap& GetMaterialInfos() { return MaterialInfos; };
	const WeaponInfoMap& GetWeaponInfos() { return WeaponInfos; };
	const SkillInfoMap& GetSkillInfos() { return SkillInfos; };
	const NPCInfoMap& GetNPCInfos() { return NPCInfos; };
	const ProjectileInfoMap& GetProjectileInfos() { return ProjectileInfos; };
	const StaticObjectInfoMap& GetStaticObjectInfos() { return StaticObjectInfos; };

private:
	//  데이터 테이블을 인포 맵에 불러온다.
	template<typename T1, typename T2>
	void _LoadDataTable( T2& InInfoMap, FString InPath )
	{
		if( UDataTable* dt = LoadObject<UDataTable>( NULL, *InPath, NULL, LOAD_None, NULL ) )
		{
			for( auto& [_, value] : dt->GetRowMap() )
			{
				T1* row = (T1*)value;
				InInfoMap.Add( row->GetKey(), *row );
			}
		}
	}
};
inline FGgDataInfoManager& GetGgDataInfoManager() { return FGgDataInfoManager::GetInstance(); };