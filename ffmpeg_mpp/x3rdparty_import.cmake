# === FFMPEG_MPP 导入脚本 ===
# 用于导入 FFMPEG_MPP 库，支持细粒度子模块按需导入
# 依赖平台变量 X3RDPARTY_TARGET_PLATFORM
# 导出变量: XXX_INCLUDE_DIRS, XXX_LIBS, XXX_STATIC_LIBS, XXX_INSTALL_FILES

set(_name "FFMPEG_MPP")
set(_root "${CMAKE_CURRENT_LIST_DIR}/${X3RDPARTY_TARGET_PLATFORM}")
set(_inc "${_root}/include")
set(_lib "${_root}/lib")

set("${_name}_INCLUDE_DIRS" "")
set("${_name}_LIBS" "")
set("${_name}_STATIC_LIBS" "")
set("${_name}_INSTALL_FILES" "")

if (EXISTS "${_inc}")
    list(APPEND "${_name}_INCLUDE_DIRS" "${_inc}")
    list(APPEND X3RDPARTY_INCLUDE_DIRS "${_inc}")
else()
    message(WARNING "[${_name}] Include directory not found: ${_inc}")
endif()

if (EXISTS "${_lib}")
    file(GLOB _shared_libs
            "${_lib}/*.so*"
            "${_lib}/*.dylib"
            "${_lib}/*.dll"
    )
    file(GLOB _static_libs
            "${_lib}/*.a"
            "${_lib}/*.lib"
    )

    if (DEFINED X3RDPARTY_${_name}_LINKS)
        foreach(_link IN LISTS X3RDPARTY_${_name}_LINKS)
            foreach(_lib IN LISTS _shared_libs)
                if (_lib MATCHES ".*${_link}.*")
                    list(APPEND "${_name}_LIBS" "${_lib}")
                    list(APPEND X3RDPARTY_SHARED_LIBS "${_lib}")
                    list(APPEND X3RDPARTY_INSTALL_FILES "${_lib}")
                endif()
            endforeach()
            foreach(_lib IN LISTS _static_libs)
                if (_lib MATCHES ".*${_link}.*")
                    list(APPEND "${_name}_STATIC_LIBS" "${_lib}")
                    list(APPEND X3RDPARTY_STATIC_LIBS "${_lib}")
                    list(APPEND X3RDPARTY_INSTALL_FILES "${_lib}")
                endif()
            endforeach()
        endforeach()
    else()
        list(APPEND "${_name}_LIBS" ${_shared_libs})
        list(APPEND "${_name}_STATIC_LIBS" ${_static_libs})
        list(APPEND X3RDPARTY_SHARED_LIBS ${_shared_libs})
        list(APPEND X3RDPARTY_STATIC_LIBS ${_static_libs})
        list(APPEND X3RDPARTY_INSTALL_FILES ${_shared_libs} ${_static_libs})
    endif()

    # 合并某个组件的动态库和静态库并导出
    list(APPEND "${_name}_INSTALL_FILES" ${_name}_LIBS ${_name}_STATIC_LIBS)

else()
    message(WARNING "[${_name}] Library directory not found: ${_lib}")
endif()

unset(_name)
unset(_root)
unset(_inc)
unset(_lib)
unset(_shared_libs)
unset(_static_libs)
unset(_link)
unset(_lib)
