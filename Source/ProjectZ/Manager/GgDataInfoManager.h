#pragma once

#include "ETC/GgConst.h"
#include "GgSingletonBase.h"
#include "CoreMinimal.h"


class FGgDataInfoManager final : public FGgSingletonBase< FGgDataInfoManager >
{

private:
	TMap<FName, void*> DataMaps;

public:
	FGgDataInfoManager();
	~FGgDataInfoManager();

	// 데이터 생성
	void DataCreate();

	// 데이터 정리
	void DataClear();

public:
	// 해당 테이블 모든 데이터를 반환한다.
	template<typename T>
	const TMap<typename T::KeyType, T>& GetInfos()
	{
		void* found = DataMaps.FindRef( FName( typeid( T ).name() ) );
		return *( TMap<typename T::KeyType, T>* )found;
	}

	// 해당 테이블 키에 맞는 데이터 하나를 찾아서 반환한다.
	template<typename T>
	const T* GetInfo( T::KeyType InKey )
	{
		if( !DataMaps.Contains( FName( typeid( T ).name() ) ) ) return nullptr;

		void* found = DataMaps.FindRef( FName( typeid( T ).name() ) );
		TMap<typename T::KeyType, T>& map = *( TMap<typename T::KeyType, T>* )found;
		return map.Find( InKey );
	}

private:
	// 데이터 테이블을 읽는다.
	template<typename T>
	void _Load( FString InPath )
	{
		UDataTable* dt = LoadObject<UDataTable>( nullptr, *InPath );
		if( !dt ) return;

		auto* map = new TMap<typename T::KeyType, T>();
		for( auto& [_, value] : dt->GetRowMap() )
		{
			T* row = (T*)( value );
			map->Add( row->GetKey(), *row );
		}
		DataMaps.Add( FName( typeid( T ).name() ), map );
	}
};
inline FGgDataInfoManager& GetGgDataInfoManager() { return FGgDataInfoManager::GetInstance(); };