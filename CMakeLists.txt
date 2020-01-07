cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME SteamGiftsBot)
set(TARGET_NAME steamgiftsbot)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Source dir is: ${CMAKE_CURRENT_SOURCE_DIR}")

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()


set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)



project(${PROJECT_NAME} 
						DESCRIPTION "SteamGifts bot"
						LANGUAGES CXX)
add_executable(
	${TARGET_NAME}
	main.cpp
	SteamGiftAcc.cpp
)

message(STATUS "Build for \"${CMAKE_BUILD_TYPE}\" configuration.")
message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
if(NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "64" AND WIN32)
    message(FATAL_ERROR "Only x64 build allowed on Windows.")
endif()

#->#################### compiler options section #################################

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /O2 /GL")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -flto ${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_DEBUG "-Wall -O0 -g ${CMAKE_CXX_FLAGS_DEBUG}")
else()
    message(FATAL_ERROR "Unknown compiler.")
endif()
#<-################### end compiler options section #############################<-




#->####################### deps sections #############################->
target_include_directories(${TARGET_NAME} PUBLIC 
                            "${PROJECT_SOURCE_DIR}/deps/include")

if(MSVC)
    target_compile_definitions(${TARGET_NAME} PUBLIC "WIN32")

    add_library(htmlcxx STATIC IMPORTED)
    set_target_properties(htmlcxx PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/deps/lib/htmlcxx.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/deps/include/htmlcxx"
    )

    add_library(curl STATIC IMPORTED)
    set_target_properties(curl PROPERTIES
        IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/deps/lib/curl.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/deps/include/curl"
    )

elseif(UNIX)
    target_compile_definitions(${TARGET_NAME} PUBLIC "UNIX")
    if(SNAP_BUILD)
        target_compile_definitions(${TARGET_NAME} PUBLIC "SNAP_BUILD")
    endif()
endif()

target_link_libraries(${TARGET_NAME} htmlcxx curl)
#<-##################### end deps sections ###########################<-

#find_library(
#        CURL_LIB_FILE 
#        curl
#        PATHS 
#            "${PROJECT_SOURCE_DIR}/deps/lib")
#find_library(
#        HTMLCXX_LIB_FILE 
#        htmlcxx
#        PATHS 
#            "${PROJECT_SOURCE_DIR}/deps/lib")
#message(FATAL_ERROR "htmlcxx lib: ${HTMLCXX_LIB_FILE}")
#message(FATAL_ERROR "curl lib: ${CURL_LIB_FILE}")
#target_link_libraries(${TARGET_NAME} ${HTMLCXX_LIB_FILE} ${CURL_LIB_FILE})
#target_link_libraries(${TARGET_NAME}  ${CURL_LIB_FILE} ${HTMLCXX_LIB_FILE})

install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION bin
        )
