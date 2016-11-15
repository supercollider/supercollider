# - Functions to help assemble a standalone Qt5 executable.
# A collection of CMake utility functions useful for deploying
# Qt5 executables.
#
# The following functions are provided by this module:
#   write_qt5_conf
#   resolve_qt5_paths
#   fixup_qt5_executable
#   install_qt5_plugin_path
#   install_qt5_plugin
#   install_qt5_executable
# Requires CMake 2.6 or greater because it uses function and
# PARENT_SCOPE. Also depends on BundleUtilities.cmake.
#
#  WRITE_QT5_CONF(<qt_conf_dir> <qt_conf_contents>)
# Writes a qt.conf file with the <qt_conf_contents> into <qt_conf_dir>.
#
#  RESOLVE_QT5_PATHS(<paths_var> [<executable_path>])
# Loop through <paths_var> list and if any don't exist resolve them
# relative to the <executable_path> (if supplied) or the CMAKE_INSTALL_PREFIX.
#
#  FIXUP_QT5_EXECUTABLE(<executable> [<qtplugins> <libs> <dirs> <plugins_dir> <request_qt_conf>])
# Copies Qt plugins, writes a Qt configuration file (if needed) and fixes up a
# Qt5 executable using BundleUtilities so it is standalone and can be
# drag-and-drop copied to another machine as long as all of the system
# libraries are compatible.
#
# <executable> should point to the executable to be fixed-up.
#
# <qtplugins> should contain a list of the names or paths of any Qt plugins
# to be installed.
#
# <libs> will be passed to BundleUtilities and should be a list of any already
# installed plugins, libraries or executables to also be fixed-up.
#
# <dirs> will be passed to BundleUtilities and should contain and directories
# to be searched to find library dependencies.
#
# <plugins_dir> allows an custom plugins directory to be used.
#
# <request_qt_conf> will force a qt.conf file to be written even if not needed.
#
#  INSTALL_QT5_PLUGIN_PATH(plugin executable copy installed_plugin_path_var <plugins_dir> <component> <configurations>)
# Install (or copy) a resolved <plugin> to the default plugins directory
# (or <plugins_dir>) relative to <executable> and store the result in
# <installed_plugin_path_var>.
#
# If <copy> is set to TRUE then the plugins will be copied rather than
# installed. This is to allow this module to be used at CMake time rather than
# install time.
#
# If <component> is set then anything installed will use this COMPONENT.
#
#  INSTALL_QT5_PLUGIN(plugin executable copy installed_plugin_path_var <plugins_dir> <component>)
# Install (or copy) an unresolved <plugin> to the default plugins directory
# (or <plugins_dir>) relative to <executable> and store the result in
# <installed_plugin_path_var>. See documentation of INSTALL_QT5_PLUGIN_PATH.
#
#  INSTALL_QT5_EXECUTABLE(<executable> [<qtplugins> <libs> <dirs> <plugins_dir> <request_qt_conf> <component>])
# Installs Qt plugins, writes a Qt configuration file (if needed) and fixes up
# a Qt5 executable using BundleUtilities so it is standalone and can be
# drag-and-drop copied to another machine as long as all of the system
# libraries are compatible. The executable will be fixed-up at install time.
# <component> is the COMPONENT used for bundle fixup and plugin installation.
# See documentation of FIXUP_QT5_BUNDLE.

#=============================================================================
# Copyright 2011 Mike McQuaid <mike@mikemcquaid.com>
# Copyright 2013 Mihai Moldovan <ionic@ionic.de>
# CMake - Cross Platform Makefile Generator
# Copyright 2000-2011 Kitware, Inc., Insight Software Consortium
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the names of Kitware, Inc., the Insight Software Consortium,
#   nor the names of their contributors may be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# The functions defined in this file depend on the fixup_bundle function
# (and others) found in BundleUtilities.cmake

include(BundleUtilities)
set(DeployQt5_cmake_dir "${CMAKE_CURRENT_LIST_DIR}")
set(DeployQt5_apple_plugins_dir "Plugins")

function(write_qt5_conf qt_conf_dir qt_conf_contents)
        set(qt_conf_path "${qt_conf_dir}/qt.conf")
        message(STATUS "Writing ${qt_conf_path}")
        file(WRITE "${qt_conf_path}" "${qt_conf_contents}")
endfunction()

function(resolve_qt5_paths paths_var)
        set(executable_path ${ARGV1})

        set(paths_resolved)
        foreach(path ${${paths_var}})
                if(EXISTS "${path}")
                        list(APPEND paths_resolved "${path}")
                else()
                        if(${executable_path})
                                list(APPEND paths_resolved "${executable_path}/${path}")
                        else()
                                list(APPEND paths_resolved "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${path}")
                        endif()
                endif()
        endforeach()
        set(${paths_var} ${paths_resolved} PARENT_SCOPE)
endfunction()

function(fixup_qt5_executable executable)
        set(qtplugins ${ARGV1})
        set(libs ${ARGV2})
        set(dirs ${ARGV3})
        set(plugins_dir ${ARGV4})
        set(request_qt_conf ${ARGV5})

        message(STATUS "fixup_qt5_executable")
        message(STATUS "  executable='${executable}'")
        message(STATUS "  qtplugins='${qtplugins}'")
        message(STATUS "  libs='${libs}'")
        message(STATUS "  dirs='${dirs}'")
        message(STATUS "  plugins_dir='${plugins_dir}'")
        message(STATUS "  request_qt_conf='${request_qt_conf}'")

        if(QT_LIBRARY_DIR)
                list(APPEND dirs "${QT_LIBRARY_DIR}")
        endif()
        if(QT_BINARY_DIR)
                list(APPEND dirs "${QT_BINARY_DIR}")
        endif()

        if(APPLE)
                set(qt_conf_dir "${executable}/Contents/Resources")
                set(executable_path "${executable}")
                set(write_qt_conf TRUE)
                if(NOT plugins_dir)
                        set(plugins_dir "${DeployQt5_apple_plugins_dir}")
                endif()
        else()
                get_filename_component(executable_path "${executable}" PATH)
                if(NOT executable_path)
                        set(executable_path ".")
                endif()
                set(qt_conf_dir "${executable_path}")
                set(write_qt_conf ${request_qt_conf})
        endif()

        foreach(plugin ${qtplugins})
                set(installed_plugin_path "")
                install_qt5_plugin("${plugin}" "${executable}" 1 installed_plugin_path)
                list(APPEND libs ${installed_plugin_path})
        endforeach()

        foreach(lib ${libs})
                if(NOT EXISTS "${lib}")
                        message(FATAL_ERROR "Library does not exist: ${lib}")
                endif()
        endforeach()

        resolve_qt5_paths(libs "${executable_path}")

        if(write_qt_conf)
                set(qt_conf_contents "[Paths]\nPlugins = ${plugins_dir}")
                write_qt5_conf("${qt_conf_dir}" "${qt_conf_contents}")
        endif()

        fixup_bundle("${executable}" "${libs}" "${dirs}")
endfunction()

function(install_qt5_plugin_path plugin executable copy installed_plugin_path_var)
        set(plugins_dir ${ARGV4})
        set(component ${ARGV5})
        set(configurations ${ARGV6})
        if(EXISTS "${plugin}")
                if(APPLE)
                        if(NOT plugins_dir)
                                set(plugins_dir "${DeployQt5_apple_plugins_dir}")
                        endif()
                        set(plugins_path "${executable}/Contents/${plugins_dir}")
                else()
                        get_filename_component(plugins_path "${executable}" PATH)
                        if(NOT plugins_path)
                                set(plugins_path ".")
                        endif()
                        if(plugins_dir)
                                set(plugins_path "${plugins_path}/${plugins_dir}")
                        endif()
                endif()

                set(plugin_group "")

                get_filename_component(plugin_path "${plugin}" PATH)
                get_filename_component(plugin_parent_path "${plugin_path}" PATH)
                get_filename_component(plugin_parent_dir_name "${plugin_parent_path}" NAME)
                get_filename_component(plugin_name "${plugin}" NAME)
                string(TOLOWER "${plugin_parent_dir_name}" plugin_parent_dir_name)

                if("${plugin_parent_dir_name}" STREQUAL "plugins")
                        get_filename_component(plugin_group "${plugin_path}" NAME)
                        set(${plugin_group_var} "${plugin_group}")
                endif()
                set(plugins_path "${plugins_path}/${plugin_group}")

                if(${copy})
                        file(MAKE_DIRECTORY "${plugins_path}")
                        file(COPY "${plugin}" DESTINATION "${plugins_path}")
                else()
                        if(configurations AND (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE))
                                set(configurations CONFIGURATIONS ${configurations})
                        else()
                                unset(configurations)
                        endif()
                        install(FILES "${plugin}" DESTINATION "${plugins_path}" ${configurations} ${component})
                endif()
                set(${installed_plugin_path_var} "${plugins_path}/${plugin_name}" PARENT_SCOPE)
        endif()
endfunction()

function(install_qt5_plugin plugin executable copy installed_plugin_path_var)
        set(plugins_dir ${ARGV4})
        set(component ${ARGV5})
        if(EXISTS "${plugin}")
                install_qt5_plugin_path("${plugin}" "${executable}" "${copy}" "${installed_plugin_path_var}" "${plugins_dir}" "${component}")
        else()
                #string(TOUPPER "QT_${plugin}_PLUGIN" plugin_var)
                set(plugin_release)
                set(plugin_debug)
                set(plugin_tmp_path)
                set(plugin_find_path "${Qt5Core_DIR}/../../../plugins/")
                get_filename_component(plugin_find_path "${plugin_find_path}" REALPATH)
                set(plugin_find_release_filename "lib${plugin}.dylib")
                set(plugin_find_debug_filename "lib${plugin}_debug.dylib")
                file(GLOB_RECURSE pluginlist "${plugin_find_path}" "${plugin_find_path}/*/lib*.dylib")
                foreach(found_plugin ${pluginlist})
                  get_filename_component(curname "${found_plugin}" NAME)
                  if("${curname}" STREQUAL "${plugin_find_release_filename}")
                    set(plugin_tmp_release_path "${found_plugin}")
                  endif()
                  if("${curname}" STREQUAL "${plugin_find_debug_filename}")
                    set(plugin_tmp_debug_path "${found_plugin}")
                  endif()
                endforeach()

                if((NOT DEFINED plugin_tmp_release_path OR NOT EXISTS "${plugin_tmp_release_path}") AND (NOT DEFINED plugin_tmp_debug_PATH OR NOT EXISTS "${plugin_tmp_debug_path}"))
                        message(WARNING "Qt plugin \"${plugin}\" not recognized or found.")
                endif()

                if(EXISTS "${plugin_tmp_release_path}")
                  set(plugin_release "${plugin_tmp_release_path}")
                elseif(EXISTS "${plugin_tmp_debug_path}")
                  set(plugin_release "${plugin_tmp_debug_path}")
                endif()

                if(EXISTS "${plugin_tmp_debug_path}")
                  set(plugin_debug "${plugin_tmp_debug_path}")
                elseif(EXISTS "${plugin_tmp_release_path}")
                  set(plugin_debug "${plugin_tmp_release_path}")
                endif()

                if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
                        install_qt5_plugin_path("${plugin_release}" "${executable}" "${copy}" "${installed_plugin_path_var}_release" "${plugins_dir}" "${component}" "Release|RelWithDebInfo|MinSizeRel")
                        install_qt5_plugin_path("${plugin_debug}" "${executable}" "${copy}" "${installed_plugin_path_var}_debug" "${plugins_dir}" "${component}" "Debug")

                        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
                                set(${installed_plugin_path_var} ${${installed_plugin_path_var}_debug})
                        else()
                                set(${installed_plugin_path_var} ${${installed_plugin_path_var}_release})
                        endif()
                else()
                        install_qt5_plugin_path("${plugin_release}" "${executable}" "${copy}" "${installed_plugin_path_var}" "${plugins_dir}" "${component}")
                endif()
        endif()
        set(${installed_plugin_path_var} ${${installed_plugin_path_var}} PARENT_SCOPE)
endfunction()

function(install_qt5_executable executable)
        set(qtplugins ${ARGV1})
        set(libs ${ARGV2})
        set(dirs ${ARGV3})
        set(plugins_dir ${ARGV4})
        set(request_qt_conf ${ARGV5})
        set(component ${ARGV6})
        if(QT_LIBRARY_DIR)
                list(APPEND dirs "${QT_LIBRARY_DIR}")
        endif()
        if(QT_BINARY_DIR)
                list(APPEND dirs "${QT_BINARY_DIR}")
        endif()
        if(component)
                set(component COMPONENT ${component})
        else()
                unset(component)
        endif()

        get_filename_component(executable_absolute "${executable}" ABSOLUTE)
        if(EXISTS "${QT_QTCORE_LIBRARY_RELEASE}")
            gp_file_type("${executable_absolute}" "${QT_QTCORE_LIBRARY_RELEASE}" qtcore_type)
        elseif(EXISTS "${QT_QTCORE_LIBRARY_DEBUG}")
            gp_file_type("${executable_absolute}" "${QT_QTCORE_LIBRARY_DEBUG}" qtcore_type)
        endif()
        if(qtcore_type STREQUAL "system")
                set(qt_plugins_dir "")
        endif()

        if(QT_IS_STATIC)
                message(WARNING "Qt built statically: not installing plugins.")
        else()
                foreach(plugin ${qtplugins})
                        message(STATUS "trying to install plugin ${plugin}")
                        set(installed_plugin_paths "")
                        install_qt5_plugin("${plugin}" "${executable}" 0 installed_plugin_paths "${plugins_dir}" "${component}")
                        list(APPEND libs ${installed_plugin_paths})
                endforeach()
        endif()

        resolve_qt5_paths(libs "")

        install(CODE
  "include(\"${DeployQt5_cmake_dir}/DeployQt5.cmake\")
  set(BU_CHMOD_BUNDLE_ITEMS TRUE)
  FIXUP_QT5_EXECUTABLE(\"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${executable}\" \"\" \"${libs}\" \"${dirs}\" \"${plugins_dir}\" \"${request_qt_conf}\")"
                ${component}
        )
endfunction()
