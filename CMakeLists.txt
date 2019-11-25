cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME SteamGiftsBot)
set(TARGET_NAME steamgiftsbot)

if(CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
else()
set(CMAKE_BUILD_TYPE "Release")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})



find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

message(INFO ${CMAKE_CURRENT_SOURCE_DIR} )
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/dependencies/cxxopts/include/cxxopts.hpp")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()






#set(CMAKE_CXX_COMPILER g++-7)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "-O3 -s ${CMAKE_CXX_FLAGS}")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "-O0 -g ${CMAKE_CXX_FLAGS}")
endif()


project(${PROJECT_NAME} 
						DESCRIPTION "SteamGifts bot"
						LANGUAGES CXX)
						


add_executable(
	${TARGET_NAME}
	main.cpp
	SteamGiftAcc.cpp
)

if(SNAP_BUILD)
target_compile_definitions(${TARGET_NAME} PRIVATE SNAP_BUILD=1)
endif()


target_include_directories(${TARGET_NAME} PUBLIC 
                            ${PROJECT_SOURCE_DIR}/dependencies)
                            
target_link_libraries(${TARGET_NAME} curl htmlcxx)

if(SNAP_BUILD)
install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION bin
        )
endif()

