cmake_minimum_required(VERSION 3.1.0)
project(xaipass)

include(CheckCXXCompilerFlag)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

  # Tell CMake to run moc when necessary:
  set(CMAKE_AUTOMOC ON)

  set(CMAKE_AUTOUIC ON)
  # As moc files are generated in the binary dir, tell CMake
  # to always look for includes there:
  set(CMAKE_INCLUDE_CURRENT_DIR ON)


set ( RESOURCES
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

if(COMPILER_SUPPORTS_CXX11)
    SET_SOURCE_FILES_PROPERTIES(
        PROPERTIES
        COMPILE_FLAGS "-std=c++11"
    )
    add_definitions(-std=c++11)
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

include_directories(
    include
    dependencies/VAL/include
)


add_library(VAL
    dependencies/VAL/src/ptree.cpp
    dependencies/VAL/src/DebugWriteController.cpp
    dependencies/VAL/src/pddl+.cpp

)

add_library(xaipass
    src/structures/model/Model.cpp
    src/controller/XAIPController.cpp
    src/model_interface/KnowledgeBase.cpp
    src/model_interface/KnowledgeComparitor.cpp
    src/model_interface/PDDLKnowledgeBase.cpp
    src/model_interface/XAIPParser.cpp
    src/model_interface/VALVisitorProblem.cpp
    src/model_interface/VALVisitorOperator.cpp
    src/planning_interface/PlannerInterface.cpp
    src/utils/utils.cpp
)

target_link_libraries(xaipass VAL)

add_executable(app src/main.cpp)
target_link_libraries(app xaipass Threads::Threads)

target_link_libraries(app xaipass)