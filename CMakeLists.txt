if(DEFINED ESP_PLATFORM)
  idf_component_register(
    SRCS
    src/lightweight_json.c

    INCLUDE_DIRS
    include
  )
else()
  cmake_minimum_required(VERSION 3.12)

  project(lightweight_json LANGUAGES C CXX)

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_C_STANDARD 99)
  set(CMAKE_CXX_STANDARD 11)

  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  add_library(
    ${PROJECT_NAME}
    src/lightweight_json.c
  )

  add_executable(
    testprog
    src/testprog.c
  )

  target_include_directories(${PROJECT_NAME} PUBLIC include)

  target_link_libraries(testprog PRIVATE ${PROJECT_NAME})

  add_executable(
    lightweight_json_test
    src/lightweight_json_test.cpp
  )
  target_link_libraries(lightweight_json_test PRIVATE GTest::gtest_main ${PROJECT_NAME})

  include(GoogleTest)
  gtest_discover_tests(lightweight_json_test)
endif()

