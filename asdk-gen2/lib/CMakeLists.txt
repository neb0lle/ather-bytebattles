Message("In lib")

ADD_SUBDIRECTORY(ring_buffer)
ADD_SUBDIRECTORY(printf)

ADD_LIBRARY(
    lib
    INTERFACE
)

TARGET_LINK_LIBRARIES(
    lib
    INTERFACE
        ring_buffer
        pico_printf
)
