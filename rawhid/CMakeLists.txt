CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(rawhid)

### =================== SOURCES =================== ###

SET(SOURCES
    hid.h
    hiddevice.h
    hiddevice.cpp
)

SET(FILES
    hid_libusb.c
    hid_WINDOWS.c
)

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
    SET(SOURCES ${SOURCES}
        hid_WINDOWS.c
    )
ELSE()
    SET(SOURCES ${SOURCES}
        hid_libusb.c
    )

    SET(LIBUSB_1_FIND_REQUIRED TRUE)

    INCLUDE(
        ../FindLibUSB-1.0.cmake
    )
ENDIF()

### =================== CONFIG =================== ###

IF(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # Find IOKit on Mac OS
    FIND_LIBRARY(IOKIT_LIBRARY IOKit REQUIRED)
    FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
ENDIF()

### =================== BUILD =================== ###

ADD_LIBRARY(rawhid STATIC ${SOURCES})
SET_PROPERTY(TARGET rawhid PROPERTY CXX_STANDARD 11)
TARGET_LINK_LIBRARIES(rawhid PRIVATE chaos-static)
TARGET_COMPILE_OPTIONS(rawhid PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -std=gnu99
    >
)

IF(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # Mac OS
    TARGET_LINK_LIBRARIES(rawhid PRIVATE ${IOKIT_LIBRARY} ${COREFOUNDATION_LIBRARY})
ENDIF()

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Windows
    TARGET_LINK_LIBRARIES(rawhid PRIVATE hid setupapi)
ELSE()
    # Linux/BSD/macOS
    TARGET_INCLUDE_DIRECTORIES(rawhid PRIVATE ${LIBUSB_1_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(rawhid PRIVATE ${LIBUSB_1_LIBRARIES})
ENDIF()


# install
install(TARGETS rawhid DESTINATION lib)
