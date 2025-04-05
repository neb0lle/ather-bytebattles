enable_language(C ASM)

###RTOS specific
#Add source folders
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/Generic/CCS SRC)
#Add include paths
SET(INC ${CMAKE_CURRENT_SOURCE_DIR}/Generic/CCS/)
#Add Assembly files
file(GLOB ASM Generic/CCS/*.asm)

SET(PORT_SRC ${SRC} PARENT_SCOPE)
SET(PORT_INC ${INC} PARENT_SCOPE)
SET(PORT_ASM ${ASM} PARENT_SCOPE)