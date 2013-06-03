SET(BULLET_DIR /usr/local/include/bullet)
SET(BULLET_LIB_DIR /usr/local/lib)

include_directories(
	# Los include aca
	/usr/include/
	${BULLET_DIR}
	${BULLET_DIR}/BulletCollision/
	${BULLET_DIR}/BulletCollision/CollisionDispatch/
	${BULLET_DIR}/BulletDynamics/

)

SET(BULLET_LIBRARIES ${BULLET_LIB_DIR}/libBulletDynamics.a 
	${BULLET_LIB_DIR}/libBulletCollision.a 
	${BULLET_LIB_DIR}/libLinearMath.a 
	${BULLET_LIB_DIR}/libconvexdecomposition.a
	) 

#target_link_libraries(OgreApp ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGREBULLET_LIBRARIES} ${BULLET_LIBRARIES} )#tinyxml)
