cmake_minimum_required(VERSION 3.16)

project(mini_redis VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

add_executable(mini_redis
  src/main.cpp
  src/server.cpp
  src/session.cpp
  src/resp_parser.cpp
  src/command_executor.cpp
  src/database.cpp
  src/presistence_manager.cpp
)

target_include_directories(mini_redis
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(mini_redis
  PRIVATE
    Boost::system
    Threads::Threads
)
