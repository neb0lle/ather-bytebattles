Message("In RTOS")

enable_language(C ASM)

#Todo: These variable should be part of global configuration when creating the build parameters.
#		Right now there are declared here till the golbal build configuration is decided.
SET(TRACE_DEBUGGER "trace_sv")

#TODO: This should come from global configurations when creating the build parameters.
IF(${TARGET_PLATFORM} STREQUAL "CYT2B75_M0PLUS")
	SET(TARGET_PLATFORM_CORE "CM0")		#Select CM0+ core
ELSEIF(${TARGET_PLATFORM} STREQUAL "CYT2B75_M4")
	SET(TARGET_PLATFORM_CORE "CM4")		#Select CM4 core
ENDIF()

IF(${TARGET_RTOS} STREQUAL "UCOS3")
	SET(RTOS "RTOS_uC-OS3")
ELSEIF(${TARGET_RTOS} STREQUAL "FREERTOS")
	SET(RTOS "RTOS_FreeRTOS")
ENDIF()

IF(${RTOS} STREQUAL "RTOS_uC-OS3")
	ADD_SUBDIRECTORY(RTOS_uC-OS3)
	SET(OS_SRC ${uCOS_SRC} PARENT_SCOPE) 
	SET(OS_INC ${uCOS_INC} PARENT_SCOPE)
	SET(OS_ASM ${uCOS_ASM} PARENT_SCOPE)
ELSEIF(${RTOS} STREQUAL "RTOS_FreeRTOS")
	# ADD_SUBDIRECTORY(RTOS_FreeRTOS)
	Message( FATAL_ERROR "FreeRTOS not supported, CMake will exit!" )
ENDIF()
