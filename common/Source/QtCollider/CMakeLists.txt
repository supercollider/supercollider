set( QT_COLLIDER_HDRS
  QcApplication.h
  MainLoop.h
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
  QcApplication.cpp
  MainLoop.cpp
  Common.cpp
  QObjectProxy.cpp
  QcPen.cpp
  Slot.cpp
  QcObjectFactory.cpp
  primitives/primitives.cpp
  primitives/prim_QObject.cpp
  primitives/prim_QPen.cpp
  primitives/prim_misc.cpp
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

include(${QT_USE_FILE})

set( SC_HEADERS_DIR "${CMAKE_SOURCE_DIR}/common/Headers" )

include_directories(
  .
  "${SC_HEADERS_DIR}/common"
  "${SC_HEADERS_DIR}/lang"
  "${SC_HEADERS_DIR}/server"
  "${SC_HEADERS_DIR}/plugin_interface"
)

if( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
  add_definitions( -DQC_DEBUG )
endif( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )

find_package(Sndfile)

if(SNDFILE_FOUND)
  include_directories(${SNDFILE_INCLUDE_DIR})
else()
  message(SEND_ERROR "Cannot find libsndfile")
endif(SNDFILE_FOUND)

qt4_wrap_cpp( QT_COLLIDER_MOC_SRCS ${QT_COLLIDER_HDRS} )

add_library( QtCollider SHARED ${QT_COLLIDER_SRCS} ${QT_COLLIDER_MOC_SRCS} )

set_property(
  TARGET QtCollider
  APPEND PROPERTY COMPILE_DEFINITIONS QT_NO_KEYWORDS
)

if(NOT SCLANG_SERVER)
  set_property(
    TARGET QtCollider
    APPEND PROPERTY COMPILE_DEFINITIONS NO_INTERNAL_SERVER
  )
endif(NOT SCLANG_SERVER)

if(APPLE)
  set_property(
    TARGET QtCollider
    APPEND PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
  )
endif(APPLE)

target_link_libraries( QtCollider ${QT_LIBRARIES} )

if(APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
	include (../../cmake_modules/MacAppFolder.cmake)

	install(TARGETS QtCollider
			DESTINATION ${scappauxresourcesdir}
			PERMISSIONS OWNER_READ OWNER_EXECUTE
						GROUP_READ GROUP_EXECUTE
						WORLD_READ WORLD_EXECUTE)
else()
	install(TARGETS QtCollider
			DESTINATION "lib"
			PERMISSIONS OWNER_READ OWNER_EXECUTE
						GROUP_READ GROUP_EXECUTE
						WORLD_READ WORLD_EXECUTE)
endif()
