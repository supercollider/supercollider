set( QT_COLLIDER_HDRS
  QtService.h
  QObjectProxy.h
  QWidgetProxy.h
  Slot.h
  Common.h
  QObjectDeclarations.cpp
  QcHelper.h
  QcObjectFactory.h
  widgets/QcAbstractLayout.h
  widgets/BasicWidgets.h
  widgets/QcScrollArea.h
  widgets/QcAbstractStepValue.h
  widgets/QcNumberBox.h
  widgets/QcSlider.h
  widgets/QcSlider2D.h
  widgets/QcRangeSlider.h
  widgets/QcMultiSlider.h
  widgets/QcTextEdit.h
  widgets/QcScope.h
)

set( QT_COLLIDER_SRCS
  Common.cpp
  QtService.cpp
  QObjectProxy.cpp
  Primitives.cpp
  PenPrimitives.cpp
  QcPen.cpp
  Slot.cpp
  QcObjectFactory.cpp
  widgets/QcAbstractLayout.cpp
  widgets/BasicWidgets.cpp
  widgets/QcScrollArea.cpp
  widgets/QcAbstractStepValue.cpp
  widgets/QcNumberBox.cpp
  widgets/QcSlider.cpp
  widgets/QcSlider2D.cpp
  widgets/QcRangeSlider.cpp
  widgets/QcMultiSlider.cpp
  widgets/QcTextEdit.cpp
  widgets/QcScope.cpp
)

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})

if( NOT SC_TRUNK )
  set( SC_TRUNK ${CMAKE_SOURCE_DIR} )
endif( NOT SC_TRUNK )

set( SC_HEADERS_DIR "${SC_TRUNK}/common/Headers" )
set( SC_INCLUDE_DIR "${SC_TRUNK}/common/include" )

include_directories(
  .
  "${SC_HEADERS_DIR}/common"
  "${SC_HEADERS_DIR}/lang"
  "${SC_HEADERS_DIR}/server"
  "${SC_HEADERS_DIR}/plugin_interface"
)

find_path( SNDFILE_INCLUDE_DIR sndfile.h )

if( SNDFILE_INCLUDE_DIR )
  include_directories( ${SNDFILE_INCLUDE_DIR} )
else( SNDFILE_INCLUDE_DIR )
  message( STATUS "QtCollider: libsndfile not found. Using header provided by SuperCollider." )
  include_directories( "${SC_INCLUDE_DIR}/libsndfile" )
endif( SNDFILE_INCLUDE_DIR )

qt4_wrap_cpp( QT_COLLIDER_MOC_SRCS ${QT_COLLIDER_HDRS} )

add_library( QtCollider SHARED ${QT_COLLIDER_SRCS} ${QT_COLLIDER_MOC_SRCS} )

set_property(
  TARGET QtCollider
  APPEND PROPERTY COMPILE_DEFINITIONS QT_NO_KEYWORDS
)

if(SCLANG_SERVER)
  message(STATUS "Compiling QtCollider for sclang with internal server.")
else(SCLANG_SERVER)
  message(STATUS "Compiling QtCollider for sclang without internal server.")
  set_property(
    TARGET QtCollider
    APPEND PROPERTY COMPILE_DEFINITIONS NO_INTERNAL_SERVER
  )
endif(SCLANG_SERVER)

if(APPLE)
  set_property(
    TARGET QtCollider
    APPEND PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
  )
endif(APPLE)

target_link_libraries( QtCollider ${QT_LIBRARIES} )

install(TARGETS QtCollider
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE
                    GROUP_READ GROUP_EXECUTE
                    WORLD_READ WORLD_EXECUTE)
