# ========================
# x3rdparty.cmake
# ========================
# 用于按需引入多个第三方组件（支持静态/动态库/头文件）
#
# 需要外部定义的变量：
#   - X3RDPARTY_TARGET_PLATFORM     : 目标平台（如 linux-x86_64、windows-x86_64）
#   - X3RDPARTY_SHARED_COMPONENTS   : 要链接的动态库组件名（如 ocv480、rkinfer）
#   - X3RDPARTY_STATIC_COMPONENTS   : 要链接的静态库组件名（如 eigen）
#   - 可选：X3RDPARTY_<COMPONENT>_LINKS : 指定组件所需的子模块库名（不含 lib 前缀）
#
# 最终导出变量包括：
#   - ${PACKAGE_NAME}_LIBS：每个依赖组件的动态库
#   - ${PACKAGE_NAME}_STATIC_LIBS：每个依赖组件的静态库
#   - ${PACKAGE_NAME}_INCLUDE_DIRS：每个依赖组件的头文件
#   - ${PACKAGE_NAME}_INSTALL_FILES：每个依赖组件的所有动态库和静态库的文件
#   - X3RDPARTY_INCLUDE_DIRS：所有库的头文件
#   - X3RDPARTY_SHARED_LIBS：所有共享库
#   - X3RDPARTY_STATIC_LIBS：所有静态库
#   - X3RDPARTY_LINK_LIBS：所有动态库和静态库
#   - X3RDPARTY_INSTALL_FILES：所有动态库和静态库的文件

if (NOT DEFINED X3RDPARTY_ROOT_DIR)
    get_filename_component(X3RDPARTY_ROOT_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
endif ()

if (NOT DEFINED X3RDPARTY_TARGET_PLATFORM)
    message(FATAL_ERROR "[X3RDPARTY] Missing 'X3RDPARTY_TARGET_PLATFORM'. Please define it, e.g. 'linux-x86_64'")
else ()
    message(STATUS "[X3RDPARTY] Target platform: ${X3RDPARTY_TARGET_PLATFORM}")
endif ()

# 收集全部组件名
set(X3RDPARTY_ALL_COMPONENTS ${X3RDPARTY_SHARED_COMPONENTS} ${X3RDPARTY_STATIC_COMPONENTS})

# 初始化导出变量（全局）
set(X3RDPARTY_INCLUDE_DIRS "")
set(X3RDPARTY_SHARED_LIBS "")
set(X3RDPARTY_STATIC_LIBS "")
set(X3RDPARTY_INSTALL_FILES "")

foreach(component IN LISTS X3RDPARTY_ALL_COMPONENTS)
    message(STATUS "[X3RDPARTY] Importing component: ${component}")

    string(TOUPPER "${component}" COMPONENT_UPPER)
    set(COMPONENT_DIR "${X3RDPARTY_ROOT_DIR}/${component}/${X3RDPARTY_TARGET_PLATFORM}")
    set(COMPONENT_LIB_DIR "${COMPONENT_DIR}/lib")
    set(COMPONENT_INC_DIR "${COMPONENT_DIR}/include")

    if (NOT EXISTS "${COMPONENT_DIR}")
        message(FATAL_ERROR "[X3RDPARTY] Component directory not found: ${COMPONENT_DIR}")
    endif ()

    # 优先加载定制导入脚本
    set(CUSTOM_IMPORT_SCRIPT "${X3RDPARTY_ROOT_DIR}/${component}/x3rdparty_import.cmake")
    if (EXISTS "${CUSTOM_IMPORT_SCRIPT}")
        message(STATUS "[X3RDPARTY] Using custom import script: ${CUSTOM_IMPORT_SCRIPT}")
        include("${CUSTOM_IMPORT_SCRIPT}")
        continue()
    endif ()

    # 包含头文件目录
    if (EXISTS "${COMPONENT_INC_DIR}")
        list(APPEND X3RDPARTY_INCLUDE_DIRS "${COMPONENT_INC_DIR}")
        set("${COMPONENT_UPPER}_INCLUDE_DIRS" "${COMPONENT_INC_DIR}")
    endif ()

    # 解析子模块过滤
    set(SUBMODULES "")
    set(SUBMOD_VAR "X3RDPARTY_${COMPONENT_UPPER}_LINKS")
    if (DEFINED ${SUBMOD_VAR})
        set(SUBMODULES ${${SUBMOD_VAR}})
    endif ()

    # 加载动态库
    if (${component} IN_LIST X3RDPARTY_SHARED_COMPONENTS)
        file(GLOB SHARED_LIBS "${COMPONENT_LIB_DIR}/*.so" "${COMPONENT_LIB_DIR}/*.dylib")
        set(SELECTED_SHARED "")
        foreach(lib IN LISTS SHARED_LIBS)
            get_filename_component(BASENAME "${lib}" NAME_WE)
            string(REGEX REPLACE "^lib" "" STRIPPED "${BASENAME}")
            if (SUBMODULES STREQUAL "" OR STRIPPED IN_LIST SUBMODULES)
                list(APPEND SELECTED_SHARED "${lib}")
                list(APPEND X3RDPARTY_SHARED_LIBS "${lib}")
                list(APPEND X3RDPARTY_INSTALL_FILES "${lib}")
            endif ()
        endforeach ()
        set("${COMPONENT_UPPER}_LIBS" "${SELECTED_SHARED}")
    endif ()

    # 加载静态库
    if (${component} IN_LIST X3RDPARTY_STATIC_COMPONENTS)
        file(GLOB STATIC_LIBS "${COMPONENT_LIB_DIR}/*.a")
        set(SELECTED_STATIC "")
        foreach(lib IN LISTS STATIC_LIBS)
            get_filename_component(BASENAME "${lib}" NAME_WE)
            string(REGEX REPLACE "^lib" "" STRIPPED "${BASENAME}")
            if (SUBMODULES STREQUAL "" OR STRIPPED IN_LIST SUBMODULES)
                list(APPEND SELECTED_STATIC "${lib}")
                list(APPEND X3RDPARTY_STATIC_LIBS "${lib}")
                list(APPEND X3RDPARTY_INSTALL_FILES "${lib}")
            endif ()
        endforeach ()
        set("${COMPONENT_UPPER}_STATIC_LIBS" "${SELECTED_STATIC}")
    endif ()

    # 合并某个组件的动态库和静态库并导出
    set("${COMPONENT_UPPER}_INSTALL_FILES" ${SELECTED_SHARED} ${SELECTED_STATIC})

endforeach ()

# 合并链接库
set(X3RDPARTY_LINK_LIBS ${X3RDPARTY_SHARED_LIBS} ${X3RDPARTY_STATIC_LIBS})
