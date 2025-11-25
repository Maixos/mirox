# === OpenCV 导入脚本 ===
# 用于导入 OpenCV 库，支持细粒度子模块按需导入
# 依赖平台变量 X3RDPARTY_TARGET_PLATFORM
# 导出变量: OCV480_INCLUDE_DIRS, OCV480_LIBS, OCV480_STATIC_LIBS, OCV480_INSTALL_FILES

set(_ocv_name "OCV480")
set(_ocv_root "${CMAKE_CURRENT_LIST_DIR}/${X3RDPARTY_TARGET_PLATFORM}")
set(_ocv_inc "${_ocv_root}/include/opencv4")
set(_ocv_lib "${_ocv_root}/lib")

set("${_ocv_name}_INCLUDE_DIRS" "")
set("${_ocv_name}_LIBS" "")
set("${_ocv_name}_STATIC_LIBS" "")
set("${_ocv_name}_INSTALL_FILES" "")

if (EXISTS "${_ocv_inc}")
    list(APPEND "${_ocv_name}_INCLUDE_DIRS" "${_ocv_inc}")
    list(APPEND X3RDPARTY_INCLUDE_DIRS "${_ocv_inc}")
else()
    message(WARNING "[${_ocv_name}] Include directory not found: ${_ocv_inc}")
endif()

if (EXISTS "${_ocv_lib}")
    file(GLOB _ocv_shared_libs
            "${_ocv_lib}/*.so*"
            "${_ocv_lib}/*.dylib"
            "${_ocv_lib}/*.dll"
    )
    file(GLOB _ocv_static_libs
            "${_ocv_lib}/*.a"
            "${_ocv_lib}/*.lib"
    )

    if (DEFINED X3RDPARTY_${_ocv_name}_LINKS)
        foreach(_link IN LISTS X3RDPARTY_${_ocv_name}_LINKS)
            foreach(_lib IN LISTS _ocv_shared_libs)
                if (_lib MATCHES ".*${_link}.*")
                    list(APPEND "${_ocv_name}_LIBS" "${_lib}")
                    list(APPEND X3RDPARTY_SHARED_LIBS "${_lib}")
                    list(APPEND X3RDPARTY_INSTALL_FILES "${_lib}")
                endif()
            endforeach()
            foreach(_lib IN LISTS _ocv_static_libs)
                if (_lib MATCHES ".*${_link}.*")
                    list(APPEND "${_ocv_name}_STATIC_LIBS" "${_lib}")
                    list(APPEND X3RDPARTY_STATIC_LIBS "${_lib}")
                    list(APPEND X3RDPARTY_INSTALL_FILES "${_lib}")
                endif()
            endforeach()
        endforeach()
    else()
        list(APPEND "${_ocv_name}_LIBS" ${_ocv_shared_libs})
        list(APPEND "${_ocv_name}_STATIC_LIBS" ${_ocv_static_libs})
        list(APPEND X3RDPARTY_SHARED_LIBS ${_ocv_shared_libs})
        list(APPEND X3RDPARTY_STATIC_LIBS ${_ocv_static_libs})
        list(APPEND X3RDPARTY_INSTALL_FILES ${_ocv_shared_libs} ${_ocv_static_libs})
    endif()

    # 合并某个组件的动态库和静态库并导出
    list(APPEND "${_ocv_name}_INSTALL_FILES" ${_ocv_name}_LIBS ${_ocv_name}_STATIC_LIBS)

else()
    message(WARNING "[${_ocv_name}] Library directory not found: ${_ocv_lib}")
endif()

unset(_ocv_name)
unset(_ocv_root)
unset(_ocv_inc)
unset(_ocv_lib)
unset(_ocv_shared_libs)
unset(_ocv_static_libs)
unset(_link)
unset(_lib)
