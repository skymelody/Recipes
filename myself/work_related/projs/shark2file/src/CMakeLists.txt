SET(PROJECT_HEADER_INCLUDE_DIR ../include)
SET(LIB_WIN_HEADER_INCLUDE_DIR ../include/win)
SET(LIB_LINUX_HEADER_INCLUDE_DIR ../include/linux)
SET(WIN_LINK_LIB ../lib/win)
SET(LINUX_LINK_LIB ../lib/linux)

SET(BUILD_USE_64BITS on)
SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -pthread -fPIC -O3 -s -fvisibility=hidden -Wl,--exclude-libs=ALL -Wl,-Bsymbolic ${CMAKE_CXX_FLAGS_RELEASE}")

include_directories(${PROJECT_HEADER_INCLUDE_DIR})
aux_source_directory(. SRC)

IF(UNIX)
    list(REMOVE_ITEM SRC "./WinProgram.cc")
    include_directories(${LIB_LINUX_HEADER_INCLUDE_DIR})
    link_directories(${LINUX_LINK_LIB})
ELSEIF(WIN32)
    list(REMOVE_ITEM SRC "./LinuxProgram.cc")
    include_directories(${LIB_WIN_HEADER_INCLUDE_DIR})
    link_directories(${WIN_LINK_LIB})


    SET(WINPCAP_HEADER ../include/win/winpcap)
    include_directories(${WINPCAP_HEADER})

    SET(WINPCAP_LIB ../lib/winpcap/Lib)
    link_directories(${WINPCAP_LIB})
ENDIF()

add_executable(shark2file ${SRC})

IF(UNIX)
    target_link_libraries(shark2file libPcap++.a libPacket++.a libCommon++.a pcap pthread)
ELSEIF(WIN32)
    target_link_libraries(shark2file ws2_32.lib wpcap.lib Packet.lib Pcap++.lib Packet++.lib Common++.lib)
ENDIF()

