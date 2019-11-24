cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME SteamGiftsBot)
set(EXE_NAME steamgiftsbot)

set(CMAKE_BUILD_TYPE Release)
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

project(${PROJECT_NAME} 
						DESCRIPTION "SteamGifts bot"
						LANGUAGES CXX)
						



add_executable(
	${EXE_NAME}
	main.cpp
	SteamGiftAcc.cpp
)

target_include_directories(${EXE_NAME} PUBLIC 
                            ${PROJECT_SOURCE_DIR}/dependencies)
                            
target_link_libraries(${EXE_NAME} curl htmlcxx)

#install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}"
#		DESTINATION /bin)

install(TARGETS ${EXE_NAME}
        RUNTIME DESTINATION bin
        )

