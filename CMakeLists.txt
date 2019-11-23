cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME SteamGiftsBot)
set(EXE_NAME steamgiftsbot)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#set(CMAKE_CXX_COMPILER g++-7)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "-O3 ${CMAKE_CXX_FLAGS}")
endif()

project(${PROJECT_NAME} VERSION 0.1.0
						DESCRIPTION "SteamGifts bot"
						LANGUAGES CXX)

add_executable(
	${EXE_NAME}
	main.cpp
	SteamGiftAcc.cpp
)


target_link_libraries(${EXE_NAME} curl htmlcxx)

#install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}"
#		DESTINATION /bin)

install(TARGETS ${EXE_NAME}
        RUNTIME DESTINATION bin
        )

