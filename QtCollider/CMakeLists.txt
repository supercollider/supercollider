cmake_minimum_required(VERSION 2.8.11)

set(required_qt_version 5.2)

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(Qt5WebKit)
find_package(Qt5WebKitWidgets)
find_package(Qt5PrintSupport)
find_package(Qt5OpenGL)
find_package(Qt5Sensors)
find_package(Qt5Quick)
find_package(Qt5Qml)
find_package(Qt5Sql)
find_package(Qt5Positioning)
mark_as_advanced(Qt5Concurrent_DIR Qt5Core_DIR Qt5Gui_DIR Qt5LinguistTools_DIR Qt5Location_DIR Qt5Network_DIR Qt5OpenGL_DIR Qt5Positioning_DIR Qt5PrintSupport_DIR Qt5Qml_DIR Qt5Quick_DIR Qt5Sensors_DIR Qt5Sql_DIR Qt5WebKitWidgets_DIR Qt5WebKit_DIR Qt5Widgets_DIR Qt5X11Extras_DIR)

set (QT_COLLIDER_LIBS
  Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::WebKit Qt5::WebKitWidgets Qt5::PrintSupport
  Qt5::OpenGL Qt5::Sensors Qt5::Quick Qt5::Qml Qt5::Sql Qt5::Positioning
  ${MATH_LIBRARY})

if (APPLE)
  find_package(Qt5MacExtras)
  mark_as_advanced(Qt5MacExtras_DIR)
  list(APPEND QT_COLLIDER_LIBS Qt5::MacExtras)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -DQC_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -DQC_DEBUG")

set(QT_COLLIDER_DIR ${CMAKE_SOURCE_DIR}/QtCollider)

include_directories( ${QT_COLLIDER_DIR} )

set( QT_COLLIDER_HDRS
  ${QT_COLLIDER_DIR}/QcApplication.h
  ${QT_COLLIDER_DIR}/QObjectProxy.h
  ${QT_COLLIDER_DIR}/QWidgetProxy.h
  ${QT_COLLIDER_DIR}/QtDownload.h
  ${QT_COLLIDER_DIR}/widgets/BasicWidgets.h
  ${QT_COLLIDER_DIR}/widgets/QcButton.h
  ${QT_COLLIDER_DIR}/widgets/QcCheckBox.h
  ${QT_COLLIDER_DIR}/widgets/QcTextField.h
  ${QT_COLLIDER_DIR}/widgets/QcListWidget.h
  ${QT_COLLIDER_DIR}/widgets/QcPopUpMenu.h
  ${QT_COLLIDER_DIR}/widgets/QcTreeWidget.h
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
  ${QT_COLLIDER_DIR}/widgets/QcWebView.h
  ${QT_COLLIDER_DIR}/widgets/web_page.hpp
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
  ${QT_COLLIDER_DIR}/widgets/QcWindow.cpp
  ${QT_COLLIDER_DIR}/widgets/QcGraph.cpp
  ${QT_COLLIDER_DIR}/widgets/QcCanvas.cpp
  ${QT_COLLIDER_DIR}/widgets/soundfileview/view.cpp
  ${QT_COLLIDER_DIR}/widgets/soundfileview/filestream.cpp
  ${QT_COLLIDER_DIR}/widgets/soundfileview/cachestream.cpp
  ${QT_COLLIDER_DIR}/widgets/QcFileDialog.cpp
  ${QT_COLLIDER_DIR}/widgets/QcPenPrinter.cpp
  ${QT_COLLIDER_DIR}/widgets/QcWebView.cpp
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


set( QT_USE_QTWEBKIT TRUE )

qt5_wrap_cpp( QT_COLLIDER_MOC_SRCS ${QT_COLLIDER_HDRS} )
qt5_add_resources( QT_COLLIDER_RCC ${QT_COLLIDER_DIR}/resources.qrc )

if(APPLE)
	qt5_wrap_cpp( QT_COLLIDER_OBJ_MOC_SRCS ${QT_COLLIDER_DIR}/widgets/QcQuartzComposerView.h )
	list(APPEND QT_COLLIDER_HDRS ${QT_COLLIDER_DIR}/widgets/QcQuartzComposerView.h )
  	set( QT_COLLIDER_OBJCPP_SRCS
  		${QT_COLLIDER_OBJ_MOC_SRCS}
		${QT_COLLIDER_DIR}/hacks/hacks_mac.mm
		${QT_COLLIDER_DIR}/widgets/QcQuartzComposerView.mm
		${QT_COLLIDER_DIR}/primitives/prim_QQuartzComposer.mm
  	)
endif()

set_source_files_properties( ${QT_COLLIDER_OBJCPP_SRCS}
    PROPERTIES COMPILE_FLAGS "-x objective-c++ -fobjc-exceptions"
)

list( APPEND QT_COLLIDER_SRCS
  ${QT_COLLIDER_OBJCPP_SRCS}
  ${QT_COLLIDER_MOC_SRCS}
  ${QT_COLLIDER_RCC}
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
