cmake_minimum_required(VERSION 3.0)
project(balakai)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
set(SOURCES
	src/parsing.cc
	src/tokens.cc
)
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -G")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(ICU 70.1 COMPONENTS uc io i18n REQUIRED)
find_package(Argumentum CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
	src/main.cc
	${SOURCES}
)
target_include_directories(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE ICU::uc ICU::io ICU::i18n
	PRIVATE Argumentum::argumentum
)

enable_testing()
add_executable(${PROJECT_NAME}_test
	test/parsing_test.cc
	${SOURCES}
)
target_include_directories(${PROJECT_NAME}_test
	PRIVATE ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCE_DIR}/src
	PRIVATE ${gtest_SOURCE_DIR}/include
	PRIVATE ${gtest_SOURCE_DIR}
)
target_link_libraries(${PROJECT_NAME}_test
	PRIVATE GTest::gtest_main
	PRIVATE ICU::uc ICU::io ICU::i18n
	PRIVATE Argumentum::argumentum
)
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)
add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)