if(Qt5Widgets_FOUND)
  file (GLOB modeltest_sources ${PROJECT_SOURCE_DIR}/src/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/model/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/optimize/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/service/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/thread/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/gui/*.ui
                             ${PROJECT_SOURCE_DIR}/src/gui/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/gui2/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/genesis/*.cpp)
else()
  file (GLOB modeltest_sources ${PROJECT_SOURCE_DIR}/src/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/model/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/optimize/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/service/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/thread/*.cpp
                             ${PROJECT_SOURCE_DIR}/src/genesis/*.cpp)
endif()

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# enable static linking
if(STATIC_BUILD)
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  SET(BUILD_SHARED_LIBRARIES OFF)
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

  if(ENABLE_PTHREADS)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
  endif()
endif()

add_executable        (modeltest_module ${modeltest_sources})

target_link_libraries (modeltest_module ${MODELTEST_LOCALDEPS_DIR}/lib/libpll_algorithm.a)
target_link_libraries (modeltest_module ${MODELTEST_LOCALDEPS_DIR}/lib/libpll_binary.a)
target_link_libraries (modeltest_module ${MODELTEST_LOCALDEPS_DIR}/lib/libpll_optimize.a)
target_link_libraries (modeltest_module ${MODELTEST_LOCALDEPS_DIR}/lib/libpll_msa.a)
target_link_libraries (modeltest_module ${MODELTEST_LOCALDEPS_DIR}/lib/libpll_tree.a)
target_link_libraries (modeltest_module ${MODELTEST_LOCALDEPS_DIR}/lib/libpll_util.a)
target_link_libraries (modeltest_module ${MODELTEST_LOCALDEPS_DIR}/lib/libpll.a)
target_link_libraries (modeltest_module m)

if(Qt5Widgets_FOUND)

  # Find the QtWidgets library
  find_package(Qt5Widgets REQUIRED)

  # Generate code from ui files
  qt5_wrap_ui(UI_HEADERS xmodeltest.ui)
  #add_library(window mainwindow.cpp ${UI_HEADERS})

  # Generate rules for building source files from the resources
  qt5_add_resources(QRCS resources.qrc)

  #  target_link_libraries (modeltest_module ${Qt5Widgets_LIBRARIES })
  target_link_libraries (modeltest_module Qt5::Widgets)
elseif(MPI_FOUND)
  target_link_libraries (modeltest_module ${MPI_CXX_LIBRARIES})
endif()

if(MPI_COMPILE_FLAGS)
  set_target_properties(modeltest_module PROPERTIES
  COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(modeltest_module PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

if(Qt5Widgets_FOUND)
  set_target_properties (modeltest_module PROPERTIES OUTPUT_NAME modeltest-gui)
elseif(MPI_FOUND)
  set_target_properties (modeltest_module PROPERTIES OUTPUT_NAME modeltest-ng-mpi)
elseif(STATIC_BUILD)
  set_target_properties (modeltest_module PROPERTIES OUTPUT_NAME modeltest-ng-static)
else()
  set_target_properties (modeltest_module PROPERTIES OUTPUT_NAME modeltest-ng)
endif()

set_target_properties (modeltest_module PROPERTIES PREFIX "")
