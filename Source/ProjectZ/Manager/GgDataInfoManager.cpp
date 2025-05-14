
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
	_LoadDataTable< FPlayerDefaultSkillInfo, PlayerDefaultSkillInfoMap >( PlayerDefaultSkillInfos, TEXT( "/Game/Data/DT_PlayerDefaultSkill" ) );
	_LoadDataTable< FPlayerWeaponSkillInfo,  PlayerWeaponSkillInfoMap > ( PlayerWeaponSkillInfos,  TEXT( "/Game/Data/DT_PlayerWeaponSkill" ) );
	_LoadDataTable< FPlayerDefaultStatInfo,  PlayerDefaultStatInfoMap > ( PlayerDefaultStatInfos,  TEXT( "/Game/Data/DT_PlayerDefaultStat" ) );
	_LoadDataTable< FMaterialInfo,           MaterialInfoMap >          ( MaterialInfos,           TEXT( "/Game/Data/DT_Material" ) );
	_LoadDataTable< FWeaponInfo,             WeaponInfoMap >            ( WeaponInfos,             TEXT( "/Game/Data/DT_Weapon" ) );
	_LoadDataTable< FSkillInfo,              SkillInfoMap >             ( SkillInfos,              TEXT( "/Game/Data/DT_Skill" ) );
	_LoadDataTable< FNPCInfo,                NPCInfoMap >               ( NPCInfos,                TEXT( "/Game/Data/DT_NPC" ) );
	_LoadDataTable< FProjectileInfo,         ProjectileInfoMap >        ( ProjectileInfos,         TEXT( "/Game/Data/DT_Projectile" ) );
	_LoadDataTable< FStaticObjectInfo,       StaticObjectInfoMap >      ( StaticObjectInfos,       TEXT( "/Game/Data/DT_StaticObject" ) );
}
