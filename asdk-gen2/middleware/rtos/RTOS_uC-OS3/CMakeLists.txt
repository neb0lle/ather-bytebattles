Message("RTOS_uC-OS3")

enable_language(C ASM)

ADD_SUBDIRECTORY(uC-OS3/Ports)
ADD_SUBDIRECTORY(uC-CPU)
ADD_SUBDIRECTORY(uC-LIB/Ports)

# IF(${TRACE_DEBUGGER_EN} STREQUAL "ON")
    # ADD_SUBDIRECTORY(uC-OS3/Trace)
# ENDIF()

###RTOS specific
#Add source folders
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/uC-OS3/Source SRC)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../../../../app/config/rtos_cfg/uC-OS3/Cfg/Template  SRC)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/uC-CPU SRC)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/uC-LIB SRC)

#Add include paths
SET(INC ${CMAKE_CURRENT_SOURCE_DIR}/uC-OS3/Source 
		${CMAKE_CURRENT_SOURCE_DIR}/../../../../app/config/rtos_cfg/uC-OS3/Cfg/Template 
		${CMAKE_CURRENT_SOURCE_DIR}/uC-CPU 
		${CMAKE_CURRENT_SOURCE_DIR}/../../../../app/config/rtos_cfg/uC-CPU/Cfg/Template  
		${CMAKE_CURRENT_SOURCE_DIR}/uC-LIB 
		${CMAKE_CURRENT_SOURCE_DIR}/../../../../app/config/rtos_cfg/uC-LIB/Cfg/Template  
		${CMAKE_CURRENT_SOURCE_DIR}/uC-LIB/Ports 
		${TRACE_INC} 
		${PORT_INC} 
		${CPU_INC} 
		${LIB_PORT_INC})

SET(uCOS_SRC ${SRC} ${CPU_SRC} ${TRACE_SRC} ${PORT_SRC} ${LIB_PORT_SRC} ) 
SET(uCOS_INC ${INC} ${CPU_INC} ${TRACE_INC} ${PORT_INC} ${LIB_PORT_INC} )
SET(uCOS_ASM ${PORT_ASM} ${CPU_ASM} ${LIB_PORT_ASM} ${TRACE_ASM} )

INCLUDE_DIRECTORIES(${uCOS_INC})

### Compile RTOS as static library

ADD_LIBRARY(
	rtos STATIC 
	${uCOS_SRC} 
	${uCOS_ASM}
)

ADD_DEPENDENCIES(rtos platform)

TARGET_LINK_LIBRARIES(rtos platform)

TARGET_INCLUDE_DIRECTORIES(
	rtos
	INTERFACE	# share with application
	${uCOS_INC}
)

Message("INCLUDES:")
foreach (uCOS_INC ${uCOS_INC})
    message(STATUS "${uCOS_INC}=${${uCOS_INC}}")
endforeach()
Message("SRC:")
foreach (uCOS_SRC ${uCOS_SRC})
    message(STATUS "${uCOS_SRC}=${${uCOS_SRC}}")
endforeach()
Message("ASM:")
foreach (uCOS_ASM ${uCOS_ASM})
    message(STATUS "${uCOS_ASM}=${${uCOS_ASM}}")
endforeach()
