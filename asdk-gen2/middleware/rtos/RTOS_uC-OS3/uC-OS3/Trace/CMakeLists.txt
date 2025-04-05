enable_language(C ASM)

###RTOS specific

#this is for Systemview
IF(${TRACE_DEBUGGER} STREQUAL "trace_sv")
        #Add source folders
        AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/SystemView/SEGGER SRC)
        AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/SystemView/SEGGER/Syscalls SRC)
        AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/SystemView/Sample/uCOS-III SRC)
        AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../app/config/rtos_cfg/SystemView/Sample/uCOS-III/Config/Cortex-M SRC)

        #Add include paths
        SET(INC ${CMAKE_CURRENT_SOURCE_DIR}/SystemView/Config/ 
                ${CMAKE_CURRENT_SOURCE_DIR}/SystemView/SEGGER 
                ${CMAKE_CURRENT_SOURCE_DIR}/SystemView/Sample/uCOS-III
                ${CMAKE_CURRENT_SOURCE_DIR}/SystemView/Sample/uCOS-III/Config )

        #Add Assembly files
        file(GLOB ASM SystemView/SEGGER/*.S)

ENDIF()
#this is for Tracerecorder
IF(${TRACE_DEBUGGER} STREQUAL "trace_tr")
        #Add source folders
        AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/TraceRecorder/ SRC)
        AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/TraceRecorder/extra/SDK/ SRC)
        AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/TraceRecorder/extra/TraceRecorderInit/ SRC)
        AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/TraceRecorder/streamports/Jlink_RTT/ SRC)

        #Add include paths
        SET(INC ${CMAKE_CURRENT_SOURCE_DIR}/TraceRecorder/config/ 
                ${CMAKE_CURRENT_SOURCE_DIR}/TraceRecorder/include/ 
                ${CMAKE_CURRENT_SOURCE_DIR}/TraceRecorder/extra/SDK/include/
                ${CMAKE_CURRENT_SOURCE_DIR}/TraceRecorder/extra/TraceRecorderInit/include/
                ${CMAKE_CURRENT_SOURCE_DIR}/TraceRecorder/streamports/Jlink_RTT/include/ )

        #Add Assembly files
        
ENDIF()

#Add Assembly files
SET(TRACE_SRC ${SRC} PARENT_SCOPE)
SET(TRACE_INC ${INC} PARENT_SCOPE)
SET(TRACE_ASM ${ASM} PARENT_SCOPE)
