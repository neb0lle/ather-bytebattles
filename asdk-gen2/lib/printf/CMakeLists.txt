Message("In pico printf")

SET(PICO_PRINTF_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/printf.c
)

ADD_LIBRARY(pico_printf STATIC ${PICO_PRINTF_SRC})

TARGET_INCLUDE_DIRECTORIES(
    pico_printf
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

TARGET_LINK_LIBRARIES(
    pico_printf
    PRIVATE
        platform
)