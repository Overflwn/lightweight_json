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
endif()

