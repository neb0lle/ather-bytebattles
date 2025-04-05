# ----- User confiugration starts ----- #

### configure project name
SET(APP_NAME "Byte battles application")

### assign name to generate the elf, srec and hex file
SET(APP_ELF_NAME "bb_app_m0")

### configure LD file
IF(${TARGET_PLATFORM} STREQUAL "CYT2B75_M0PLUS")
    SET(APP_LINKER_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/app/linker_files/preprocessed_linker_file_cm0plus.ld
    )
ELSE()
    SET(APP_LINKER_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/app/linker_files/preprocessed_linker_file_cm4.ld
    )
ENDIF()

### add source files and include paths

# NOTE: you must add include paths under 'APP_USER_INC' variable
SET(APP_USER_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/arsenal
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/sensors/ultrasonic
    # add other includes from here
    # example:
    # ${CMAKE_CURRENT_SOURCE_DIR}/app/app_imu
)

# NOTE: you must add source paths under 'APP_USER_SRC' variable
# add other source paths from here
# example:
# AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/app/app_imu APP_USER_SRC)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/arsenal APP_USER_SRC)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/config APP_USER_SRC)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/sensors/ultrasonic APP_USER_SRC)
# ----- User confiugration ends ----- #
