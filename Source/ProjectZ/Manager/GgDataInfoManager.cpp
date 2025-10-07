
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
	DataClear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 데이터 생성
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgDataInfoManager::DataCreate()
{
	_Load< FPlayerDefaultSkillInfo >( TEXT( "/Game/Data/DT_PlayerDefaultSkill" ) );
	_Load< FPlayerWeaponSkillInfo  >( TEXT( "/Game/Data/DT_PlayerWeaponSkill" ) );
	_Load< FPlayerDefaultStatInfo  >( TEXT( "/Game/Data/DT_PlayerDefaultStat" ) );
	_Load< FMaterialInfo           >( TEXT( "/Game/Data/DT_Material" ) );
	_Load< FWeaponInfo             >( TEXT( "/Game/Data/DT_Weapon" ) );
	_Load< FSkillInfo              >( TEXT( "/Game/Data/DT_Skill" ) );
	_Load< FNPCInfo                >( TEXT( "/Game/Data/DT_NPC" ) );
	_Load< FProjectileInfo         >( TEXT( "/Game/Data/DT_Projectile" ) );
	_Load< FStaticObjectInfo       >( TEXT( "/Game/Data/DT_StaticObject" ) );
	_Load< FFoliageInfo            >( TEXT( "/Game/Data/DT_Foliage" ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// @brief 데이터 정리
/////////////////////////////////////////////////////////////////////////////////////////////////////
void FGgDataInfoManager::DataClear()
{
	for( auto& [_, value] : DataMaps )
	{
		delete value;
	}

	DataMaps.Empty();
}