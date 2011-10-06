find_package (Qt4 4.5)
if (NOT QT4_FOUND)
    message(FATAL_ERROR
        "You are trying to compile with Qt GUI support, but either Qt version on your"
        " system is not compatible, or Qt cannot be found."
        " Please either install Qt (version 4.5 or greater), or compile without Qt GUI support"
        " (add -DSC_QT=OFF to the cmake command line).")
endif()

find_package (Sndfile REQUIRED)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DQC_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -DQC_DEBUG")

set( QT_COLLIDER_HDRS
  QcApplication.h
  QObjectProxy.h
  QWidgetProxy.h
  widgets/BasicWidgets.h
  widgets/QcTreeWidget.h
  widgets/QcScrollArea.h
  widgets/QcNumberBox.h
  widgets/QcSlider.h
  widgets/QcSlider2D.h
  widgets/QcRangeSlider.h
  widgets/QcMultiSlider.h
  widgets/QcTextEdit.h
  widgets/QcScope.h
  widgets/QcGraph.h
  widgets/QcLevelIndicator.h
  widgets/QcCanvas.h
  widgets/QcSoundFileView.h
  widgets/QcFileDialog.h
  widgets/QcWebView.h
  widgets/QcWindow.h
  layouts/layouts.hpp
)

set( QT_COLLIDER_SRCS
  debug.cpp
  interface.cpp
  QcApplication.cpp
  Common.cpp
  QObjectProxy.cpp
  QWidgetProxy.cpp
  Slot.cpp
  QcObjectFactory.cpp
  hacks/hacks_x11.cpp
  primitives/primitives.cpp
  primitives/prim_QObject.cpp
  primitives/prim_QPen.cpp
  primitives/prim_misc.cpp
  primitives/prim_QWidget.cpp
  widgets/BasicWidgets.cpp
  widgets/QcTreeWidget.cpp
  widgets/QcScrollArea.cpp
  widgets/QcAbstractStepValue.cpp
  widgets/QcNumberBox.cpp
  widgets/QcSlider.cpp
  widgets/QcSlider2D.cpp
  widgets/QcRangeSlider.cpp
  widgets/QcMultiSlider.cpp
  widgets/QcTextEdit.cpp
  widgets/QcScope.cpp
  widgets/QcLevelIndicator.cpp
  widgets/QcWindow.cpp
  widgets/QcGraph.cpp
  widgets/QcCanvas.cpp
  widgets/QcSoundFileView.cpp
  widgets/QcFileDialog.cpp
  widgets/QcWebView.cpp
  layouts/classic_layouts.cpp
  layouts/layouts.cpp
  style/ProxyStyle.cpp
)

set( QT_USE_QTWEBKIT TRUE )

include(${QT_USE_FILE})

set( SC_HEADERS_DIR "${CMAKE_SOURCE_DIR}/include" )
include_directories(
  .
  "${SC_HEADERS_DIR}/QtCollider"
  "${SC_HEADERS_DIR}/common"
  "${SC_HEADERS_DIR}/lang"
  "${SC_HEADERS_DIR}/server"
  "${SC_HEADERS_DIR}/plugin_interface"
  ${SNDFILE_INCLUDE_DIR}
)

qt4_wrap_cpp( QT_COLLIDER_MOC_SRCS ${QT_COLLIDER_HDRS} )

add_library( QtCollider SHARED ${QT_COLLIDER_SRCS} ${QT_COLLIDER_MOC_SRCS} )

add_definitions( -DQT_NO_KEYWORDS )

if(NOT SCLANG_SERVER)
  add_definitions( -DNO_INTERNAL_SERVER )
endif(NOT SCLANG_SERVER)

if(APPLE)
  set_property(
    TARGET QtCollider
    APPEND PROPERTY LINK_FLAGS "-undefined dynamic_lookup"
  )
endif(APPLE)

set_property ( TARGET QtCollider
               APPEND PROPERTY COMPILE_DEFINITIONS QT_COLLIDER_EXPORTING )

target_link_libraries( QtCollider ${QT_LIBRARIES} ${MATH_LIBRARY})


if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	find_package(X11)

	if(X11_FOUND)
		include_directories(X11_INCLUDE_DIR)
		target_link_libraries( QtCollider ${X11_X11_LIB})
	elseif()
		message(SEND_ERROR "Cannot find libx11")
	endif()
endif()

if(APPLE)
	# determines the app name and app install location (scappbundlename, scappdir):
  include (${CMAKE_SOURCE_DIR}/cmake_modules/MacAppFolder.cmake)

  install(TARGETS QtCollider
          DESTINATION ${scappauxresourcesdir}
          PERMISSIONS OWNER_READ OWNER_EXECUTE
                      GROUP_READ GROUP_EXECUTE
                      WORLD_READ WORLD_EXECUTE
                      OWNER_WRITE
          COMPONENT app)
else()
  install(TARGETS QtCollider
          DESTINATION "lib"
          PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE
                      GROUP_READ GROUP_EXECUTE
                      WORLD_READ WORLD_EXECUTE )
endif()
