#
# Try to find GLFW library and include path.
# Once done this will define
#
# GLFW3_FOUND
# GLFW3_INCLUDE_PATH
# GLFW3_LIBRARY
#

IF(WIN32)
    FIND_PATH( GLFW3_INCLUDE_PATH GLFW/glfw3.h
		/mingw64/include
		$ENV{INCLUDE}
		$ENV{PROGRAMFILES}/GLFW/include
		${GLFW_ROOT_DIR}/include
		DOC "The directory where GLFW/glfw3.h resides")

    FIND_LIBRARY( GLFW3_LIBRARY
        NAMES libglfw3.a glfw3 GLFW
        PATHS
		/mingw64/lib
		$ENV{LIB}/GLFW
        $ENV{PROGRAMFILES}/GLFW/lib
		${GLFW_ROOT_DIR}/lib
        DOC "The GLFW library")
ELSE(WIN32)
	FIND_PATH( GLFW3_INCLUDE_PATH GLFW/glfw3.h
		$ENV{INCLUDE}
		/usr/include
		/usr/local/include
		/sw/include
        /opt/local/include
		${GLFW_ROOT_DIR}/include
		DOC "The directory where GLFW/glfw3.h resides")

	# Prefer the static library.
	FIND_LIBRARY( GLFW3_LIBRARY
        NAMES libGLFW.a GLFW libGLFW3.a GLFW3 libglfw.so libglfw.so.3 libglfw.so.3.0
		PATHS
		$ENV{LIB}/GLFW
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		${GLFW_ROOT_DIR}/lib
		DOC "The GLFW library")
ENDIF(WIN32)

SET(GLFW3_FOUND "NO")
IF(GLFW3_INCLUDE_PATH AND GLFW3_LIBRARY)
	SET(GLFW_LIBRARIES ${GLFW3_LIBRARY})
	SET(GLFW3_FOUND "YES")
    message(STATUS "Found GLFW")
	message(${GLFW3_LIBRARY})
ENDIF(GLFW3_INCLUDE_PATH AND GLFW3_LIBRARY)

