
#include "GgDataInfoManager.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"


FGgDataInfoManager::FGgDataInfoManager()
{
	DataCreate();
}

FGgDataInfoManager::~FGgDataInfoManager()
{
	PlayerDefaultSkillInfos.Empty();
	PlayerWeaponSkillInfos.Empty();
	PlayerDefaultStatInfos.Empty();
	MaterialInfos.Empty();
	WeaponInfos.Empty();
	SkillInfos.Empty();
	NPCInfos.Empty();
	ProjectileInfos.Empty();
	StaticObjectInfos.Empty();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 데이터 생성
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgDataInfoManager::DataCreate()
{
	_LoadDataTable< FPlayerDefaultSkillInfo, PlayerDefaultSkillInfoMap >( PlayerDefaultSkillInfos, TEXT( "/Game/Data/DT_PlayerDefaultSkillInfo" ) );
	_LoadDataTable< FPlayerWeaponSkillInfo,  PlayerWeaponSkillInfoMap > ( PlayerWeaponSkillInfos,  TEXT( "/Game/Data/DT_PlayerWeaponSkillInfo" ) );
	_LoadDataTable< FPlayerDefaultStatInfo,  PlayerDefaultStatInfoMap > ( PlayerDefaultStatInfos,  TEXT( "/Game/Data/DT_PlayerDefaultStatInfo" ) );
	_LoadDataTable< FMaterialInfo,           MaterialInfoMap >          ( MaterialInfos,           TEXT( "/Game/Data/DT_MaterialInfo" ) );
	_LoadDataTable< FWeaponInfo,             WeaponInfoMap >            ( WeaponInfos,             TEXT( "/Game/Data/DT_WeaponInfo" ) );
	_LoadDataTable< FSkillInfo,              SkillInfoMap >             ( SkillInfos,              TEXT( "/Game/Data/DT_SkillInfo" ) );
	_LoadDataTable< FNPCInfo,                NPCInfoMap >               ( NPCInfos,                TEXT( "/Game/Data/DT_NPCInfo" ) );
	_LoadDataTable< FProjectileInfo,         ProjectileInfoMap >        ( ProjectileInfos,         TEXT( "/Game/Data/DT_ProjectileInfo" ) );
	_LoadDataTable< FStaticObjectInfo,       StaticObjectInfoMap >      ( StaticObjectInfos,       TEXT( "/Game/Data/DT_StaticObjectInfo" ) );
}
