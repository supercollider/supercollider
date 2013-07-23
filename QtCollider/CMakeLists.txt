find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)

set(QT_COLLIDER_DIR ${CMAKE_SOURCE_DIR}/QtCollider)

include_directories( ${QT_COLLIDER_DIR} )

set( QT_COLLIDER_HDRS
  ${QT_COLLIDER_DIR}/QcApplication.h
  ${QT_COLLIDER_DIR}/QObjectProxy.h
  ${QT_COLLIDER_DIR}/QWidgetProxy.h
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

if(APPLE)
  set( QT_COLLIDER_OBJCPP_SRCS
      ${QT_COLLIDER_DIR}/hacks/hacks_mac.mm
  )
endif()

if( CMAKE_COMPILER_IS_GNUCXX )
  set_source_files_properties( ${QT_COLLIDER_OBJCPP_SRCS}
      PROPERTIES COMPILE_FLAGS "-x objective-c++ -fobjc-exceptions"
  )
endif()

qt5_wrap_cpp( QT_COLLIDER_MOC_SRCS ${QT_COLLIDER_HDRS} )
qt5_add_resources( QT_COLLIDER_RCC ${QT_COLLIDER_DIR}/resources.qrc )

list( APPEND QT_COLLIDER_SRCS
  ${QT_COLLIDER_OBJCPP_SRCS}
  ${QT_COLLIDER_MOC_SRCS}
  ${QT_COLLIDER_RCC}
)

macro( qtcollider_prepare_target _target )

  set_property( TARGET ${_target}
    APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include/QtCollider )

  set_property( TARGET ${_target}
    APPEND PROPERTY COMPILE_DEFINITIONS SC_QT QT_COLLIDER_EXPORTING QT_NO_KEYWORDS )

  set_property( TARGET ${_target}
    APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG QC_DEBUG )

  target_link_libraries( ${_target} ${MATH_LIBRARY} )

  if(APPLE)
    target_link_libraries( ${_target} "-framework Cocoa" )
  endif()

  if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(X11)
    if(X11_FOUND)
      set_property( TARGET ${_target} APPEND PROPERTY INCLUDE_DIRECTORIES ${X11_INCLUDE_DIR} )
      target_link_libraries( ${_target} ${X11_X11_LIB} )
    else()
      message(SEND_ERROR "Cannot find libx11")
    endif()
  endif()

  qt5_use_modules( ${_target} Widgets WebKitWidgets )

endmacro( qtcollider_prepare_target )
