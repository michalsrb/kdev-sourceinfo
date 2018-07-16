cmake_minimum_required(VERSION 3.0)

project(sourceinfo)

find_package(ECM "5.14.0" REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)

set(KF5_DEP_VERSION "5.15.0")
find_package(KF5 ${KF5_DEP_VERSION} REQUIRED COMPONENTS
    I18n
    ItemModels # needed because missing in KDevPlatformConfig.cmake, remove once dep on kdevplatform >=5.2.2
)

set(KDEVPLATFORM_VERSION "5.1.0")
find_package(KDevPlatform ${KDEVPLATFORM_VERSION} CONFIG)
set_package_properties(KDevPlatform PROPERTIES
    TYPE REQUIRED
)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_definitions(-DTRANSLATION_DOMAIN=\"kdevsourceinfo\")

set(kdevsourceinfo_PART_UIS
    sourceinfotoolview.ui
)

set(kdevsourceinfo_PART_SRCS
    sourceinfoplugin.cpp
    sourceinfoinlinenoteprovider.cpp
    sourceinfotoolview.cpp
    notes/generictextnote.cpp
    notes/membersizenote.cpp
)
ecm_qt_declare_logging_category(kdevsourceinfo_PART_SRCS
    HEADER debug.h
    IDENTIFIER KDEV_SOURCEINFO
    CATEGORY_NAME "kdevelop.plugins.sourceinfo"
)

ki18n_wrap_ui(kdevsourceinfo_PART_SRCS ${kdevsourceinfo_PART_UIS})

kdevplatform_add_plugin(kdevsourceinfo JSON kdevsourceinfo.json SOURCES ${kdevsourceinfo_PART_SRCS})
target_link_libraries(kdevsourceinfo
    KDev::Interfaces
    KDev::Util
    KDev::Project
    KDev::OutputView
    KDev::Language
    KF5::I18n
)

# kdebugsettings file
install(FILES kdevsourceinfo.categories DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
