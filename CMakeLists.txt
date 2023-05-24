cmake_minimum_required(VERSION 3.0)
project(balakai)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
set(SOURCES
	src/main.cc
	src/parsing.cc
)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -G")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")

find_package(ICU 70.1 COMPONENTS uc io i18n REQUIRED)
find_package(Argumentum CONFIG REQUIRED)

add_executable(balakai ${SOURCES})
target_include_directories(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE ICU::uc ICU::io ICU::i18n
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE Argumentum::argumentum
)