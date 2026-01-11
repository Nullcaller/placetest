cmake_minimum_required(VERSION 3.20)
project(placetest C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
#set(CMAKE_CXX_STANDARD 23)

add_executable(placetest)

target_include_directories(placetest PRIVATE src)

#target_compile_options(placetest PRIVATE -Wall -Wextra -Werror -pedantic)

# Link libsodium
target_link_libraries(placetest libsodium.a)
# Link math lib
target_link_libraries(placetest -lm)

target_sources(placetest
	PRIVATE
	src/placetest.c
)
