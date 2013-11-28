# Define global sources here
set(HDRS ${HDRS}
    # Camera controller
	${DEV_ROOT_PATH}/cz/main_player/camera_controller/CameraController.h
	
	# Weapons
	${DEV_ROOT_PATH}/cz/main_player/weapon/Weapon.h
	${DEV_ROOT_PATH}/cz/main_player/weapon/WeaponBuilder.h
	${DEV_ROOT_PATH}/cz/main_player/weapon/WeaponDefs.h
	${DEV_ROOT_PATH}/cz/main_player/weapon/bullet_based/BulletBasedWeapon.h
	${DEV_ROOT_PATH}/cz/main_player/weapon/bullet_based/Weapon9MM.h
	${DEV_ROOT_PATH}/cz/main_player/weapon/bullet_based/WeaponFAL.h
	
	# main player
	${DEV_ROOT_PATH}/cz/main_player/MainPlayer.h
	${DEV_ROOT_PATH}/cz/main_player/MainPlayerWeaponEvent.h
)

set(SRCS ${SRCS}
	# Weapons
	${DEV_ROOT_PATH}/cz/main_player/weapon/Weapon.cpp
	${DEV_ROOT_PATH}/cz/main_player/weapon/WeaponBuilder.cpp
	${DEV_ROOT_PATH}/cz/main_player/weapon/bullet_based/BulletBasedWeapon.cpp
	${DEV_ROOT_PATH}/cz/main_player/weapon/bullet_based/Weapon9MM.cpp
	${DEV_ROOT_PATH}/cz/main_player/weapon/bullet_based/WeaponFAL.cpp
	
	# main player
	${DEV_ROOT_PATH}/cz/main_player/MainPlayer.cpp
)


