set(REQUIRED_QT_VERSION 6.2)
option(SC_USE_QTWEBENGINE "Build with Qt WebEngine." ON)

###############################################################################
# Components and libraries

set(
  QT_COMPONENTS
  Core
  Gui
  Network
  PrintSupport
  Svg
  Widgets
)

# find Qt6 or Qt5
find_package(QT NAMES Qt6 ${REQUIRED_QT_VERSION} QUIET COMPONENTS Core)
if (NOT Qt6_FOUND)
    set(REQUIRED_QT_VERSION 5.15)
    find_package(QT NAMES Qt5 ${REQUIRED_QT_VERSION} QUIET COMPONENTS Core)
endif()
set(QT_VERSION_MAJOR ${QT_VERSION_MAJOR} PARENT_SCOPE) # make it available elsewhere

set (QT_COLLIDER_LIBS
  Qt::Core Qt::Gui Qt::Network Qt::PrintSupport Qt::Svg Qt::Widgets
  ${MATH_LIBRARY})

if(SC_USE_QTWEBENGINE)
  list(APPEND QT_COMPONENTS WebEngineCore WebEngineWidgets)
  list(APPEND QT_COLLIDER_LIBS Qt::WebEngineCore Qt::WebEngineWidgets)
endif()

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    list(APPEND QT_COMPONENTS Core5Compat)
    list(APPEND QT_COLLIDER_LIBS Qt::Core5Compat)
    if(UNIX AND SC_USE_QTWEBENGINE)
        list(APPEND QT_COLLIDER_LIBS Qt::DBus)
    endif()
endif()

find_package(Qt${QT_VERSION_MAJOR} ${REQUIRED_QT_VERSION} COMPONENTS ${QT_COMPONENTS} REQUIRED)

if (APPLE AND ${QT_VERSION_MAJOR} LESS 6)
  find_package(Qt5MacExtras)
  mark_as_advanced(Qt5MacExtras_DIR)
  list(APPEND QT_COLLIDER_LIBS Qt5::MacExtras)
endif()

###############################################################################
# Debug flags

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DQC_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -DQC_DEBUG")

###############################################################################
# Source file manifest

set(QT_COLLIDER_DIR ${CMAKE_SOURCE_DIR}/QtCollider)

include_directories( ${QT_COLLIDER_DIR} )

set( QT_COLLIDER_HDRS
  ${QT_COLLIDER_DIR}/QcApplication.h
  ${QT_COLLIDER_DIR}/QObjectProxy.h
  ${QT_COLLIDER_DIR}/QWidgetProxy.h
  ${QT_COLLIDER_DIR}/QtDownload.h
  ${QT_COLLIDER_DIR}/QcCallback.hpp
  ${QT_COLLIDER_DIR}/widgets/BasicWidgets.h
  ${QT_COLLIDER_DIR}/widgets/QcButton.h
  ${QT_COLLIDER_DIR}/widgets/QcCheckBox.h
  ${QT_COLLIDER_DIR}/widgets/QcTextField.h
  ${QT_COLLIDER_DIR}/widgets/QcListWidget.h
  ${QT_COLLIDER_DIR}/widgets/QcPopUpMenu.h
  ${QT_COLLIDER_DIR}/widgets/QcTreeWidget.h
  ${QT_COLLIDER_DIR}/widgets/QcMenu.h
  ${QT_COLLIDER_DIR}/widgets/QcScrollArea.h
  ${QT_COLLIDER_DIR}/widgets/QcNumberBox.h
  ${QT_COLLIDER_DIR}/widgets/QcSlider.h
  ${QT_COLLIDER_DIR}/widgets/QcSlider2D.h
  ${QT_COLLIDER_DIR}/widgets/QcRangeSlider.h
  ${QT_COLLIDER_DIR}/widgets/QcMultiSlider.h
  ${QT_COLLIDER_DIR}/widgets/QcKnob.hpp
  ${QT_COLLIDER_DIR}/widgets/QcTextEdit.h
  ${QT_COLLIDER_DIR}/widgets/QcScope.h
  ${QT_COLLIDER_DIR}/widgets/QcScopeShm.h
  ${QT_COLLIDER_DIR}/widgets/QcGraph.h
  ${QT_COLLIDER_DIR}/widgets/QcLevelIndicator.h
  ${QT_COLLIDER_DIR}/widgets/QcCanvas.h
  ${QT_COLLIDER_DIR}/widgets/soundfileview/view.hpp
  ${QT_COLLIDER_DIR}/widgets/QcFileDialog.h
  ${QT_COLLIDER_DIR}/widgets/QcPenPrinter.h
  ${QT_COLLIDER_DIR}/widgets/QcWindow.h
  ${QT_COLLIDER_DIR}/layouts/layouts.hpp
  ${QT_COLLIDER_DIR}/layouts/stack_layout.hpp
  ${QT_COLLIDER_DIR}/style/style.hpp
)

set( QT_COLLIDER_SRCS
  ${QT_COLLIDER_DIR}/type_codec.cpp
  ${QT_COLLIDER_DIR}/metatype.cpp
  ${QT_COLLIDER_DIR}/debug.cpp
  ${QT_COLLIDER_DIR}/interface.cpp
  ${QT_COLLIDER_DIR}/factories.cpp
  ${QT_COLLIDER_DIR}/QcApplication.cpp
  ${QT_COLLIDER_DIR}/Common.cpp
  ${QT_COLLIDER_DIR}/QObjectProxy.cpp
  ${QT_COLLIDER_DIR}/QWidgetProxy.cpp
  ${QT_COLLIDER_DIR}/QcObjectFactory.cpp
  ${QT_COLLIDER_DIR}/QtDownload.cpp
  ${QT_COLLIDER_DIR}/QcCallback.cpp
  ${QT_COLLIDER_DIR}/hacks/hacks_x11.cpp
  ${QT_COLLIDER_DIR}/primitives/primitives.cpp
  ${QT_COLLIDER_DIR}/primitives/prim_QObject.cpp
  ${QT_COLLIDER_DIR}/primitives/prim_QPen.cpp
  ${QT_COLLIDER_DIR}/primitives/prim_misc.cpp
  ${QT_COLLIDER_DIR}/primitives/prim_QWidget.cpp
  ${QT_COLLIDER_DIR}/primitives/prim_QPalette.cpp
  ${QT_COLLIDER_DIR}/primitives/prim_QImage.cpp
  ${QT_COLLIDER_DIR}/widgets/BasicWidgets.cpp
  ${QT_COLLIDER_DIR}/widgets/QcButton.cpp
  ${QT_COLLIDER_DIR}/widgets/QcCheckBox.h
  ${QT_COLLIDER_DIR}/widgets/QcTextField.h
  ${QT_COLLIDER_DIR}/widgets/QcListWidget.cpp
  ${QT_COLLIDER_DIR}/widgets/QcPopUpMenu.cpp
  ${QT_COLLIDER_DIR}/widgets/QcTreeWidget.cpp
  ${QT_COLLIDER_DIR}/widgets/QcMenu.cpp
  ${QT_COLLIDER_DIR}/widgets/QcScrollArea.cpp
  ${QT_COLLIDER_DIR}/widgets/QcAbstractStepValue.cpp
  ${QT_COLLIDER_DIR}/widgets/QcNumberBox.cpp
  ${QT_COLLIDER_DIR}/widgets/QcSlider.cpp
  ${QT_COLLIDER_DIR}/widgets/QcSlider2D.cpp
  ${QT_COLLIDER_DIR}/widgets/QcRangeSlider.cpp
  ${QT_COLLIDER_DIR}/widgets/QcMultiSlider.cpp
  ${QT_COLLIDER_DIR}/widgets/QcKnob.cpp
  ${QT_COLLIDER_DIR}/widgets/QcTextEdit.cpp
  ${QT_COLLIDER_DIR}/widgets/QcScope.cpp
  ${QT_COLLIDER_DIR}/widgets/QcScopeShm.cpp
  ${QT_COLLIDER_DIR}/widgets/QcLevelIndicator.cpp
  ${QT_COLLIDER_DIR}/widgets/QcWebView.cpp
  ${QT_COLLIDER_DIR}/widgets/QcWindow.cpp
  ${QT_COLLIDER_DIR}/widgets/QcGraph.cpp
  ${QT_COLLIDER_DIR}/widgets/QcCanvas.cpp
  ${QT_COLLIDER_DIR}/widgets/soundfileview/view.cpp
  ${QT_COLLIDER_DIR}/widgets/soundfileview/filestream.cpp
  ${QT_COLLIDER_DIR}/widgets/soundfileview/cachestream.cpp
  ${QT_COLLIDER_DIR}/widgets/QcFileDialog.cpp
  ${QT_COLLIDER_DIR}/widgets/QcPenPrinter.cpp
  ${QT_COLLIDER_DIR}/widgets/web_page.cpp
  ${QT_COLLIDER_DIR}/layouts/layouts.cpp
  ${QT_COLLIDER_DIR}/layouts/stack_layout.cpp
  ${QT_COLLIDER_DIR}/style/ProxyStyle.cpp
  ${QT_COLLIDER_DIR}/style/style.cpp
)

if(QT_COLLIDER_LANG_CLIENT)
    list(APPEND QT_COLLIDER_HDRS ${QT_COLLIDER_DIR}/LanguageClient.h)
    list(APPEND QT_COLLIDER_SRCS ${QT_COLLIDER_DIR}/LanguageClient.cpp)
endif()

set(QT_COLLIDER_WEBENGINE_HDRS
  ${QT_COLLIDER_DIR}/widgets/QcWebView.h
  ${QT_COLLIDER_DIR}/widgets/web_page.hpp
)

if(SC_USE_QTWEBENGINE)
  list(APPEND QT_COLLIDER_HDRS ${QT_COLLIDER_WEBENGINE_HDRS})
endif()

###############################################################################

qt_wrap_cpp( QT_COLLIDER_MOC_SRCS ${QT_COLLIDER_HDRS} )
qt_add_resources( QT_COLLIDER_RCC ${QT_COLLIDER_DIR}/resources.qrc )

if(APPLE)
  set( QT_COLLIDER_OBJCPP_SRCS ${QT_COLLIDER_DIR}/hacks/hacks_mac.mm )
  if(${QT_VERSION_MAJOR} LESS 6)
    list(APPEND QT_COLLIDER_HDRS ${QT_COLLIDER_DIR}/widgets/QcQuartzComposerView.h )
    qt_wrap_cpp( QT_COLLIDER_OBJ_MOC_SRCS ${QT_COLLIDER_DIR}/widgets/QcQuartzComposerView.h )
    list(APPEND QT_COLLIDER_OBJCPP_SRCS ${QT_COLLIDER_OBJ_MOC_SRCS} ${QT_COLLIDER_DIR}/widgets/QcQuartzComposerView.mm  ${QT_COLLIDER_DIR}/primitives/prim_QQuartzComposer.mm )
  endif()
endif()

set_source_files_properties( ${QT_COLLIDER_OBJCPP_SRCS}
    PROPERTIES COMPILE_FLAGS "-x objective-c++ -fobjc-exceptions"
)

list( APPEND QT_COLLIDER_SRCS
  ${QT_COLLIDER_OBJCPP_SRCS}
  ${QT_COLLIDER_MOC_SRCS}
  ${QT_COLLIDER_RCC}
  ${QT_COLLIDER_WEBENGINE_HDRS}
)

add_definitions(-DSC_QT -DQT_COLLIDER_EXPORTING -DQT_NO_KEYWORDS)

if(APPLE)
  list(APPEND QT_COLLIDER_LIBS "-framework Cocoa" )
  list(APPEND QT_COLLIDER_LIBS "-framework Quartz" )
  list(APPEND QT_COLLIDER_LIBS "-framework AppKit" )
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -F/Library/Frameworks")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(X11 REQUIRED)
    include_directories(X11_INCLUDE_DIR)
    list(APPEND QT_COLLIDER_LIBS ${X11_X11_LIB})
endif()
