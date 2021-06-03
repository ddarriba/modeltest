if(Qt5Widgets_FOUND)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTORCC ON)

  file (GLOB modeltest_sources ${PROJECT_SOURCE_DIR}/src/*.cpp
                               ${PROJECT_SOURCE_DIR}/src/model/*.cpp
                               ${PROJECT_SOURCE_DIR}/src/optimize/*.cpp
                               ${PROJECT_SOURCE_DIR}/src/service/*.cpp
                               ${PROJECT_SOURCE_DIR}/src/thread/*.cpp
                               ${PROJECT_SOURCE_DIR}/src/gui/*.ui
                               ${PROJECT_SOURCE_DIR}/src/gui/*.cpp
                               ${PROJECT_SOURCE_DIR}/src/gui/mydebugstream.h
                               ${PROJECT_SOURCE_DIR}/src/gui2/*.cpp
                               ${PROJECT_SOURCE_DIR}/src/genesis/*.cpp)

  # Generate code from ui files
  qt5_wrap_ui(UI_HEADERS
              gui/progressdialog.ui
              gui/resultsexportdialog.ui
              gui/resultsdialog.ui
              gui/modelsdialog.ui
              gui/xmodeltest.ui
              gui/datainfodialog.ui
              gui2/xmodeltestfancy.ui
              gui2/aboutdialog.ui
  )
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
  set (CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  set (BUILD_SHARED_LIBRARIES OFF)
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

  if(ENABLE_MULTITHREAD)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
  endif()
endif()

if (OSX_BUNDLE)
  add_executable(modeltest_module MACOSX_BUNDLE ${modeltest_sources} ${UI_HEADERS})
else()
  add_executable(modeltest_module ${modeltest_sources} ${UI_HEADERS})
endif()

set(MODELTEST_LIBS ${PLLMODULES_LIBRARIES} m)
target_link_libraries(modeltest_module ${MODELTEST_LIBS})

if(Qt5Widgets_FOUND)
  qt5_use_modules(modeltest_module Widgets)
  qt5_add_resources(QRCS resources.qrc)
  target_link_libraries (modeltest_module Qt5::Widgets Qt5::Gui)
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
  if (OSX_BUNDLE)
    set_target_properties (modeltest_module PROPERTIES OUTPUT_NAME modeltest-gui
                                            MACOSX_BUNDLE_BUNDLE_NAME "modeltest")
    set(exe modeltest-gui.app)

    include(InstallRequiredSystemLibraries)

    get_target_property(build_dir modeltest_module BINARY_DIR)
    get_target_property(target_name modeltest_module NAME)
    get_target_property(isbundle modeltest_module MACOSX_BUNDLE)
  else()
    set_target_properties (modeltest_module PROPERTIES OUTPUT_NAME modeltest-gui)
  endif()
elseif(MPI_CXX_FOUND)
  set_target_properties (modeltest_module PROPERTIES OUTPUT_NAME modeltest-ng-mpi)
elseif(STATIC_BUILD)
  set_target_properties (modeltest_module PROPERTIES OUTPUT_NAME modeltest-ng-static)
else()
  set_target_properties (modeltest_module PROPERTIES OUTPUT_NAME modeltest-ng)
endif()

set_target_properties (modeltest_module PROPERTIES PREFIX "")

install(TARGETS modeltest_module RUNTIME DESTINATION bin COMPONENT applications)
install(FILES ${PROJECT_SOURCE_DIR}/man/modeltest-ng.1 DESTINATION man/man1)
