IF(NOT DEV_ROOT_PATH)
	message(SEND_ERROR "No esta seteado DEV_ROOT_PATH")
endif()
include(${DEV_ROOT_PATH}/DevPaths.cmake)

set(CP /GameUnits)

set(SRCS
	${SRCS}
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/WorldContext/PlayerWorldContext.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/Backpack/BackpackItemBuilder.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/Backpack/Backpack.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/PlayerGroup/PlayerGroup.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerFSMBuilder.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerEngagingState.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerAttackingState.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerBombState.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerIdleState.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerMovingState.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerPickObjectState.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerReloadingState.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/PlayerUnit.cpp
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/PlayerUnitBuilder.cpp
	${DEV_ROOT_PATH}/GameUnits/UnitGroup/Formations/CircularFormation.cpp
	${DEV_ROOT_PATH}/GameUnits/UnitGroup/UnitGroup.cpp
	${DEV_ROOT_PATH}/GameUnits/GameItem/LifeGameItem/LifeGameItem.cpp
	${DEV_ROOT_PATH}/GameUnits/GameItem/GameItem.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/ZombieFactory/ZombieFactory.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/ZombieFactory/ZombieFactoryManager.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieBorningState.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieFollowSoundState.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieSpreadingState.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieIdleState.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieBeenHitState.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieDyingState.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieFSMBuilder.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieEngagingState.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieAttackingState.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/ZombieBuilder.cpp
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/ZombieUnit.cpp
	${DEV_ROOT_PATH}/GameUnits/CollectableObject/CollectableObject.cpp
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilEscapingState.cpp
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilIdleState.cpp
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilBeenRescuedState.cpp
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilFSMBuilder.cpp
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilDyingState.cpp
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilFollowingState.cpp
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilBeenHitState.cpp
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/CivilUnit.cpp
	${DEV_ROOT_PATH}/GameUnits/UnitPath/UnitPath.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/Shoot/Shoot.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/BodyWeapon/BodyWeapon.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/Proximitybomb/ProximityBomb.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/TimeBomb/TimeBomb.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/RemoteBomb/RemoteBomb.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/Bomb.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/BombBuilder.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/ShortWeapon/9mm/Gun9mm.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/ShortWeapon/ShortWeapon.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/LongWeapon/Shotgun/Shotgun.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/LongWeapon/LongWeapon.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/GrenadeWeapon/GrenadeWeapon.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/Weapon.cpp
	${DEV_ROOT_PATH}/GameUnits/Weapon/WeaponBuilder.cpp
	${DEV_ROOT_PATH}/GameUnits/GameUnit.cpp
	${DEV_ROOT_PATH}/GameUnits/GameObject.cpp
)

set(HDRS
	${HDRS}
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/WorldContext/PlayerWorldContext.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/Backpack/Backpack.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/Backpack/BackpackDefines.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/Backpack/BackpackItemBuilder.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/Backpack/BackpackItem.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/PlayerGroup/PlayerGroup.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerPickObjectState.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerMovingState.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerEngagingState.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerReloadingState.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerIdleState.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerFSMBuilder.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerAttackingState.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine/PlayerBombState.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/PlayerUnit.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/PlayerUnitBuilder.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/PlayerDefs.h
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/PlayerUnitManager.h
	${DEV_ROOT_PATH}/GameUnits/UnitGroup/Formations/CircularFormation.h
	${DEV_ROOT_PATH}/GameUnits/UnitGroup/UnitGroup.h
	${DEV_ROOT_PATH}/GameUnits/UnitGroup/UnitAttackStrategy.h
	${DEV_ROOT_PATH}/GameUnits/UnitGroup/UnitGroupFormation.h
	${DEV_ROOT_PATH}/GameUnits/GameItem/LifeGameItem/LifeGameItem.h
	${DEV_ROOT_PATH}/GameUnits/GameItem/GameItem.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/ZombieFactory/ZombieFactory.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/ZombieFactory/ZombieFactoryManager.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieEngagingState.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieSpreadingState.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieDyingState.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieIdleState.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieFSMBuilder.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieFollowSoundState.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieBeenHitState.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieAttackingState.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine/ZombieBorningState.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/ZombieUnit.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/ZombieDefs.h
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/ZombieBuilder.h
	${DEV_ROOT_PATH}/GameUnits/CollectableObject/CollectableObjTypes.h
	${DEV_ROOT_PATH}/GameUnits/CollectableObject/CollectableObject.h
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilBeenRescuedState.h
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilBeenHitState.h
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilFollowingState.h
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilIdleState.h
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilEscapingState.h
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilFSMBuilder.h
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine/CivilDyingState.h
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/CivilDefs.h
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/CivilUnit.h
	${DEV_ROOT_PATH}/GameUnits/UnitPath/UnitPath.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/Shoot/Shoot.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/BodyWeapon/BodyWeapon.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/Proximitybomb/ProximityBomb.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/TimeBomb/TimeBomb.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/RemoteBomb/RemoteBomb.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/RemoteBomb/RemoteBombTrigger.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/Bomb.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/BombBuilder.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/ShortWeapon/9mm/Gun9mm.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/ShortWeapon/ShortWeapon.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/LongWeapon/Shotgun/Shotgun.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/LongWeapon/LongWeapon.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/GrenadeWeapon/GrenadeWeapon.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/WeaponIDs.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/WeaponBuilder.h
	${DEV_ROOT_PATH}/GameUnits/Weapon/Weapon.h
	${DEV_ROOT_PATH}/GameUnits/GameObject.h
	${DEV_ROOT_PATH}/GameUnits/GameUnitDefines.h
	${DEV_ROOT_PATH}/GameUnits/GameUnit.h
	${DEV_ROOT_PATH}/GameUnits/UnitWorldContext.h
)

set(ACTUAL_DIRS
	${DEV_ROOT_PATH}/GameUnits
	${DEV_ROOT_PATH}/GameUnits/Weapon
	${DEV_ROOT_PATH}/GameUnits/Weapon/GrenadeWeapon
	${DEV_ROOT_PATH}/GameUnits/Weapon/LongWeapon
	${DEV_ROOT_PATH}/GameUnits/Weapon/LongWeapon/Shotgun
	${DEV_ROOT_PATH}/GameUnits/Weapon/ShortWeapon
	${DEV_ROOT_PATH}/GameUnits/Weapon/ShortWeapon/9mm
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/RemoteBomb
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/TimeBomb
	${DEV_ROOT_PATH}/GameUnits/Weapon/Bombs/Proximitybomb
	${DEV_ROOT_PATH}/GameUnits/Weapon/BodyWeapon
	${DEV_ROOT_PATH}/GameUnits/Weapon/Shoot
	${DEV_ROOT_PATH}/GameUnits/UnitPath
	${DEV_ROOT_PATH}/GameUnits/CivilUnit
	${DEV_ROOT_PATH}/GameUnits/CivilUnit/StateMachine
	${DEV_ROOT_PATH}/GameUnits/CollectableObject
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/StateMachine
	${DEV_ROOT_PATH}/GameUnits/ZombieUnit/ZombieFactory
	${DEV_ROOT_PATH}/GameUnits/GameItem
	${DEV_ROOT_PATH}/GameUnits/GameItem/LifeGameItem
	${DEV_ROOT_PATH}/GameUnits/UnitGroup
	${DEV_ROOT_PATH}/GameUnits/UnitGroup/Formations
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/StateMachine
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/PlayerGroup
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/Backpack
	${DEV_ROOT_PATH}/GameUnits/PlayerUnit/WorldContext
)

include_directories(${ACTUAL_DIRS})
