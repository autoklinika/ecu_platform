cmake_minimum_required(VERSION 3.16)

project(ecu_platform LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================
# Qt
# ==========================

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

# ==========================
# EXECUTABLE
# ==========================

qt_add_executable(ecu_app
    src/main.cpp

    # ---- BACKEND ----
    src/transport/Transport_CAN_Linux.cpp
    src/core/CAN_Dispatcher.cpp
    src/isotp/ISOTP.cpp
    src/uds/UDS_Client.cpp
    src/uds/UDS_Core.cpp
    src/virtual_cockpit/VirtualCockpit.cpp
    src/ecu/sac/SAC_Module.cpp
    src/ecu/sac/SAC_DTC_Module.cpp
    src/ecu/sac/SAC_Runtime_Module.cpp


    # ---- QT BRIDGE ----
    src/QT_Bridge/CockpitController.cpp
    src/QML/LanguageSettings.cpp

    #-----SYSTEM------
    src/system/SystemController.cpp
)

target_include_directories(ecu_app PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# ==========================
# QML FILES
# ==========================

file(GLOB_RECURSE QML_FILES
    CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_SOURCE_DIR}
    src/QML/*.qml
)

# 🔴 KLUCZOWE — SINGLETONY

set_source_files_properties(
    src/QML/LanguageManager.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

set_source_files_properties(
    src/QML/Navigation.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

# ==========================
# QML MODULE
# ==========================

qt_add_qml_module(ecu_app
    URI ecu_gui
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# ==========================
# LINK
# ==========================

target_link_libraries(ecu_app
    PRIVATE Qt6::Quick
)

qt_finalize_executable(ecu_app)
