Message("In ring buffer")

SET(RING_BUFFER_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/ring_buffer.c
)

ADD_LIBRARY(ring_buffer STATIC ${RING_BUFFER_SRC})

TARGET_INCLUDE_DIRECTORIES(
    ring_buffer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

TARGET_LINK_LIBRARIES(
    ring_buffer
    PRIVATE
        platform
)