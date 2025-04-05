MESSAGE("In External EEPROM")

SET(EXTERNAL_EEPROM_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/asdk_external_eeprom.c
)

ADD_LIBRARY(external_eeprom STATIC ${EXTERNAL_EEPROM_SRC})

ADD_DEPENDENCIES(external_eeprom platform)

TARGET_INCLUDE_DIRECTORIES(
    external_eeprom
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

TARGET_COMPILE_DEFINITIONS(
    external_eeprom
    PUBLIC
        -DASDK_USE_EXTERNAL_EEPROM=${ASDK_USE_EXTERNAL_EEPROM}
)

TARGET_LINK_LIBRARIES(
    external_eeprom
    PRIVATE
        platform
)
