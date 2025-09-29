#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NiagaraSystem.h"
#include "GgConst.generated.h"


using namespace std;


class AGgActorSpawner;
class AGgProjectile;
class AGgCharacterNPC;
class AGgStaticObject;


// 오브젝트 종류
UENUM( BlueprintType )
enum class EObjectType : uint8
{
	PC				  UMETA( DisplayName = "PC" ),
	NPC				  UMETA( DisplayName = "NPC" ),
	PROJECTILE        UMETA( DisplayName = "PROJECTILE" ),
	STATIC_OBJECT     UMETA( DisplayName = "STATIC_OBJECT" ),
	MAX,
};

// 애니메이션 상태
UENUM( BlueprintType )
enum class EAnimState : uint8
{
	IDLE_RUN            UMETA( DisplayName = "Idle_Run" ),
	JUMP                UMETA( DisplayName = "Jump" ),
	AIRBORNE            UMETA( DisplayName = "Airborne" ),
	DIE                 UMETA( DisplayName = "Die" ),
	COMMON_ACTION       UMETA( DisplayName = "Common_Action" ),

	MAX,             
};

// 애니메이션 보조 상태
UENUM( BlueprintType )
enum class EAnimSubState : uint8
{
	DEFAULT             UMETA( DisplayName = "Default" ),
	MOVABLE             UMETA( DisplayName = "Movable" ),
	UPPER_LOWER_BLEND   UMETA( DisplayName = "Upper_Lower_Blend" ),

	MAX,
};

// 물질 상태
UENUM( BlueprintType )
enum class EMaterialState : uint8
{
	DEFAULT          UMETA( DisplayName = "Default" ),
	JELLY            UMETA( DisplayName = "Jelly" ),
	GRASS            UMETA( DisplayName = "Grass" ),
	ROCK             UMETA( DisplayName = "Rock" ),
	GRAVEL           UMETA( DisplayName = "Gravel" ),
	WATER            UMETA( DisplayName = "Water" ),
	DEEPWATER        UMETA( DisplayName = "DeepWater" ),
	MARBLE           UMETA( DisplayName = "Marble" ),

	MAX,
};

// 무기 종류
UENUM( BlueprintType )
enum class EWeaponType : uint8
{
	BEAM_SWORD          UMETA( DisplayName = "BeamSword" ),
	GREAT_SWORD         UMETA( DisplayName = "GreatSword" ),
	KATANA              UMETA( DisplayName = "Katana" ),
	CLUB                UMETA( DisplayName = "Club" ),
	SMALL_SWORD         UMETA( DisplayName = "SmallSword" ),

	MAX,
};

// 콜리전 모양 종류
UENUM( BlueprintType )
enum class ECollShapeType : uint8
{
	BOX          UMETA( DisplayName = "Box" ),
	FAN          UMETA( DisplayName = "Fan" ),
	CYLINDER     UMETA( DisplayName = "Cylinder" ),

	MAX,
};

// 입력 키 종류
UENUM( BlueprintType )
enum class EInputKeyType : uint8
{
	LEFT_MOUSE            UMETA( DisplayName = "LeftMouse" ),
	RIGHT_MOUSE           UMETA( DisplayName = "RightMouse" ),
	BOTH_MOUSE            UMETA( DisplayName = "BothMouse" ),
	SHIFT_MOVING          UMETA( DisplayName = "Shift_MOVING" ),
	SHIFT_STANDING        UMETA( DisplayName = "Shift_STANDING" ),
	SPACE                 UMETA( DisplayName = "Space" ),
	ALT                   UMETA( DisplayName = "Alt" ),
	Tab                   UMETA( DisplayName = "Tab" ),
	F                     UMETA( DisplayName = "F" ),
	R                     UMETA( DisplayName = "R" ),
	Q                     UMETA( DisplayName = "Q" ),
	E                     UMETA( DisplayName = "E" ),
	Num1                  UMETA( DisplayName = "Num1" ),
	Num2                  UMETA( DisplayName = "Num2" ),
	Num3                  UMETA( DisplayName = "Num3" ),

	MAX,
};

// 스킬 플레이 재생속도 종류
UENUM( BlueprintType )
enum class ESkillPlaySpeedType : uint8
{
	DEFAULT              UMETA( DisplayName = "Default" ),
	ATTACK_SPEED         UMETA( DisplayName = "AttackSpeed" ),
	MOVE_SPEED           UMETA( DisplayName = "MoveSpeed" ),

	MAX,
};

// 팀 종류
UENUM( BlueprintType )
enum class ETeamType : uint8
{
	NEUTRAL              UMETA( DisplayName = "Neutral" ),
	A                    UMETA( DisplayName = "A" ),
	B                    UMETA( DisplayName = "B" ),
	C                    UMETA( DisplayName = "C" ),
	D                    UMETA( DisplayName = "D" ),

	MAX,
};

// 충돌체 정보
USTRUCT( Atomic, BlueprintType )
struct FCollisionData
{
	GENERATED_BODY()

public:
	/* Common */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Common" )
	ECollShapeType Shape;

	/* Common */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Common" )
	FVector Pos;

	/* Common */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Common" )
	FRotator Rotation;

	/* Box */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Box", meta = ( EditCondition = "Shape == ECollShapeType::BOX" ) )
	FVector Extent;

	/* Fan & Cylinder */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Fan & Cylinder", meta = ( EditCondition = "Shape == ECollShapeType::FAN || Shape == ECollShapeType::CYLINDER" ) )
	float Radius;

	/* Fan & Cylinder */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Fan & Cylinder", meta = ( EditCondition = "Shape == ECollShapeType::FAN || Shape == ECollShapeType::CYLINDER" ) )
	float Height;

	/* Fan */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Fan", meta = ( ClampMin = "0", ClampMax = "180" ), meta = ( EditCondition = "Shape == ECollShapeType::FAN" ) )
	float Angle;

	// 공격력
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Common" )
	float Power;

	// 넉백 파워
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Common" )
	float KnockBackPower;

	// 공중 띄우기 파워
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Common" )
	float AirbornePower;

	// 역경직 시간
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Common" )
	float HitStopTime;

	// 무기 타격 이펙트를 출력할 지 여부
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Common" )
	bool bUseWeaponEffect;

	// 이펙트 방향
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Common" )
	FVector EffectDir;

	// 타격시 추가로 출력할 이펙트
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Common" )
	TSoftObjectPtr<UNiagaraSystem> HitEffect;

public:
	FCollisionData()
	{
		Shape = ECollShapeType::MAX;
		Pos = FVector();
		Rotation = FRotator();
		Power = 0;
		KnockBackPower = 0;
		AirbornePower = 0;
		HitStopTime = 0;
		Extent = FVector();
		Radius = 0;
		Height = 0;
		Angle = 0;
		bUseWeaponEffect = false;
		EffectDir = FVector();
		HitEffect = nullptr;
	}
};

// 스텟 정보
USTRUCT( Atomic, BlueprintType )
struct FStatusData
{
	GENERATED_BODY()

public:
	// 레벨
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int Level; 	

	// 체력
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float Hp; 

	// 체력
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float Hpm;

	// 스태미너
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float Mp; 	

	// 스태미너
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float Mpm; 	

	// 이동속도 ( 0 ~ 2 )
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float MoveSpeed;

	// 공격속도 ( 0 ~ 2 )
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float AttackSpeed;

	// 공격력
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float AttackPower;

	// 방어력
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float DefensePower;

	// 점프력 ( 0 ~ 2 )
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float JumpPower;

	// 강인함 ( 0 ~ 100 ) ( 경직시간, 넉백거리 관련 ) 
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float Strength;

	// 무게 ( 0 ~ 2 )
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float Weight;

	// 폭군 여부 (팀 상관 없이 타격)
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	bool IsTyrant;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// DataInfo
/////////////////////////////////////////////////////////////////////////////////////////////////////

// 물질 정보
USTRUCT( Atomic, BlueprintType )
struct FMaterialInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	EMaterialState State;                            // 물질 상태

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float Mass;                                      // 질량 ( 0 ~ 2 ) (공격속도,이동속도,점프력 비율)

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float Intensity;                                 // 강도 ( 0 ~ 2 ) (공격력,방어력,강인함 비율 )

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float MatEnergyMax;                              // 최대 물질 에너지

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FName CollisonName;                              // 물질 변경 시 콜리전이름

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TArray < FString > MaterialAssetPaths;         // 머터리얼 애셋경로 ( 해당 하는 머터리얼은 State가 될 수 있다 )

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FString FootStepParticleName;                // 재질 밟을 때 스폰할 파티클 이름

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FString ChangeParticleName;                  // 해당 재질로 변경되었을 때 스폰할 파티클 이름

	/// 맵 키를 반환한다.
	using KeyType = EMaterialState;
	KeyType GetKey(){ return State; };
};

// 무기 정보
USTRUCT( Atomic, BlueprintType )
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 식별자
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int InfoId;                    

	// 무기 종류
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	EWeaponType Type;        

	// 스테틱 메쉬 컴포넌트 이름
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FName ComponentName;       

	// 타격시 출력 할 이펙트
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSoftObjectPtr<UNiagaraSystem> HitEffect;

	// 무기 투척시 소환할 BP Class
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSoftClassPtr<AGgProjectile> ThorwingBPClass;    

	/// 맵 키를 반환한다.
	using KeyType = int;
	KeyType GetKey(){ return InfoId; };
};


// 스킬 정보
USTRUCT( Atomic, BlueprintType )
struct FSkillInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int InfoId;                                           // 식별자

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float CoolTime;                                    // 쿨타임

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float CostMP;                                      // 소모 값 MP

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	ESkillPlaySpeedType PlaySpeedType;                 // 스킬 재생속도 타입

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	EAnimSubState AnimSubState;                        // 애니메이션 보조 상태

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TArray<EAnimState> PlayEnableState;                // 스킬 사용 가능한 상태

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TArray<int> PlayEnableSkillId;                    // 스킬 사용 가능한 스킬아이디

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int DerivedSkillId;                               // 파생 스킬 식별자

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	bool LockOnLookAt;                                 // 락온 상태에서 스킬 시전 시 바라볼 것인지 여부 

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	bool EquipSubWeapon;                               // 보조 무기 장착 여부

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSoftObjectPtr<UAnimMontage> AnimMontage;          // 몽타주

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float ActivateRangeMin;                            // 스킬 발동 범위 최소 (몬스터 전용)

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float ActivateRangeMax;                            // 스킬 발동 범위 최대 (몬스터 전용)

	/// 맵 키를 반환한다.
	using KeyType = int;
	KeyType GetKey(){ return InfoId; };
};

// 플레이어 기본 스텟 정보
USTRUCT( Atomic, BlueprintType )
struct FPlayerDefaultStatInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int InfoId;                            // 식별자

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FStatusData Stat;                   // 스텟

	/// 맵 키를 반환한다.
	using KeyType = int;
	KeyType GetKey(){ return InfoId; };
};

// 플레이어 기본 스킬 정보
USTRUCT( Atomic, BlueprintType )
struct FPlayerDefaultSkillInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	EInputKeyType InputKey;                          // 입력 키

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int SkillId;                                    // 입력 키에 해당하는 스킬 넘버

	/// 맵 키를 반환한다.
	using KeyType = EInputKeyType;
	KeyType GetKey(){ return InputKey; };
};

// 플레이어 무기 스킬 정보
USTRUCT( Atomic, BlueprintType )
struct FPlayerWeaponSkillInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int WeaponInfoId;                         // 무기 식별자

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int L_SkillId;                          // 약공격 

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int R_SkillId;                          // 강공격 

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int L_JumpSkillId;                       // 점프 약공격

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int R_JumpSkillId;                       // 점프 강공격

	/// 맵 키를 반환한다.
	using KeyType = int;
	KeyType GetKey(){ return WeaponInfoId; };
};

// NPC 정보
USTRUCT( Atomic, BlueprintType )
struct FNPCInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int InfoId;                            // 식별자

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FStatusData Stat;                   // 스텟

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TArray<int> SkillInfos;             // 스킬

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float PatrolRange;                  // 정찰 이동 범위

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float DetectRange;                  // 탐지 범위

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSoftClassPtr<AGgCharacterNPC> BPClass;    // BP Class

	/// 맵 키를 반환한다.
	using KeyType = int;
	KeyType GetKey(){ return InfoId; };
};

// 정적 오브젝트 정보
USTRUCT( Atomic, BlueprintType )
struct FStaticObjectInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int InfoId;                            // 식별자

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FStatusData Stat;                   // 스텟

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSoftClassPtr<AGgStaticObject> BPClass;    // BP Class

	/// 맵 키를 반환한다.
	using KeyType = int;
	KeyType GetKey(){ return InfoId; };
};

// 투사체 정보
USTRUCT( Atomic, BlueprintType )
struct FProjectileInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int InfoId;                            // 식별자

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FStatusData Stat;                   // 스텟

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FCollisionData     AttackCollData; // 공격 콜리전 정보

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float LifeTime = 0;                // 지속 시간

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSoftClassPtr<AGgProjectile> BPClass;    // BP Class

	/// 맵 키를 반환한다.
	using KeyType = int;
	KeyType GetKey(){ return InfoId; };
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// Const
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace CONST
{ 
    // Common
	constexpr float ANIM_LERP_MULITPLIER      = 5.f;       // 애니메이션 중 이동 속도 보간 값
	constexpr float DEFAULT_MOVE_SPEED        = 600.f;     // 기본 이동속도
	constexpr float DEFAULT_DASH_SPEED		  = 1500.f;    // 기본 대쉬 이동속도
	constexpr float DEFAULT_JUMP_POWER        = 750.f;     // 기본 점프력
	constexpr float DEFAULT_ROTATION_RATE     = 1000.f;    // 기본 캐릭터 방향 전환 비율
	constexpr float FLY_ROTATION_RATE         = 100.f;     // 날기 캐릭터 방향 전환 비율
	constexpr float DEFAULT_AIRBORNEPOWER     = 50.f;      // 기본 에어본 파워
	constexpr float AIRBORNE_KNOCKBACK_MULITPLIER = 0.03f; // 에어본 넉백 보정 값 
	constexpr float WATER_FALL_ANIM_TIME      = 0.01f;     // 물에 빠지는 애니메이션 속도
	constexpr float READY_SKILL_ROTATION_RATE = 100000.f;  // 발동 대기 스킬 보유 캐릭터 방향 전환 비율
	constexpr float READY_SKILL_DURATION      = 0.4f;      // 발동 대기중인 스킬 저장 지속시간
	constexpr float MONTAGE_INITIAL_TIME      = 0.05f;     // 몽타주 발동 후 초기시간 ( 방향전환가능 및 LookAt 시간)
	constexpr float DEAD_ACTOR_DESTROY_TIME   = 3.f;       // 사망 후 해당 초 이후 액터 삭제
	constexpr float HOLD_TIME_INCREASE_VALUE  = 0.1f;      // 역경직 시간 증가 값
	constexpr float MP_RECOVERY_VALUE         = 20.f;      // MP 회복 상수 

	constexpr float DEFAULT_TARGET_ARM_LENGTH = 280.f;     // 기본 카메라 암 길이
	constexpr float FLY_TARGET_ARM_LENGTH     = 400.f;     // 날기 카메라 암 길이

	constexpr float DEFAULT_GRAVITY_SCALE     = 2.0f;      // 기본 중력
	constexpr float FLY_GRAVITY_SCALE         = 0.3f;      // 날기 중력

	constexpr float LOCKON_RANGE              = 800.f;      // 락온 범위
	constexpr float LOCKON_CAMERA_ROTAION_LAG_SPEED = 5.f;  // 락온 카메라 회전 딜레이 속도
	constexpr float LOCKON_CAMERA_FIX_PITCH   = 5.f;      // 락온 카메라 pitch 고정 값

	constexpr float MAX_MASS                  = 2.f;       // 최대 질량
	constexpr float MAX_INTENSITY             = 2.f;       // 최대 강도

	constexpr float MIDDLE_RATE               = 1.2f;      // 중간 강도/질량 기준
	constexpr float HARD_RATE                 = 1.5f;      // 고 강도/질량 기준
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//// Type def
/////////////////////////////////////////////////////////////////////////////////////////////////////

using FActorPtr        = TWeakObjectPtr<AActor>;
using FCharacterPtr    = TWeakObjectPtr<ACharacter>;
using FActorSpawnerPtr = TWeakObjectPtr<AGgActorSpawner>;