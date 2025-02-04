#[[
© 2024 AO Kaspersky Lab

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
]]

cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Required for the linters.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(knp-devices VERSION "${KNP_VERSION}" LANGUAGES C CXX
        DESCRIPTION "Kaspersky Neuromorphic Platform device parameters library"
        HOMEPAGE_URL "https://neuro.kaspersky.ru/neyromorfnye-tekhnologii/")

include(GNUInstallDirs)
include(clang-tidy)
include(knp-functions)

set(${PROJECT_NAME}_PUBLIC_INCLUDE_DIR "knp/devices")

file(GLOB_RECURSE ${PROJECT_NAME}_headers include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/*.h)

source_group(source REGULAR_EXPRESSION "impl/.*")
source_group(headers REGULAR_EXPRESSION "include/.*")

knp_add_library("${PROJECT_NAME}-cpu"
    BOTH
    impl/cpu.cpp impl/cpu_power.cpp impl/cpu_power.h

    ${${PROJECT_NAME}_headers}

    LINK_PRIVATE
        spdlog::spdlog PCM_STATIC
        KNP::Core
    ALIAS KNP::Devices::CPU
)

set_target_properties(
        "${PROJECT_NAME}-cpu"
        PROPERTIES
        VERSION "${CMAKE_PROJECT_VERSION}"
        SOVERSION "${CMAKE_PROJECT_VERSION_MAJOR}")

knp_add_library("${PROJECT_NAME}-altai"
    BOTH
    impl/altai.cpp

    include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}/altai.h

    LINK_PRIVATE
        spdlog::spdlog PCM_STATIC
        KNP::Core
    ALIAS KNP::Devices::AltAI
)

set_target_properties(
    "${PROJECT_NAME}-altai"
    PROPERTIES
    VERSION "${CMAKE_PROJECT_VERSION}"
    SOVERSION "${CMAKE_PROJECT_VERSION_MAJOR}"
)

if (KNP_INSTALL)
    set(COMPONENT_NAME "cpp-framework")

    install(TARGETS "${PROJECT_NAME}-cpu"
            COMPONENT "${COMPONENT_NAME}-dev"
            EXPORT "${PROJECT_NAME}-cpu"
            LIBRARY
    )

    install(TARGETS "${PROJECT_NAME}-cpu_static"
            EXPORT "${PROJECT_NAME}-cpu_static"
            COMPONENT "${COMPONENT_NAME}-dev"
            ARCHIVE)

    # Install for AltAI
    install(TARGETS "${PROJECT_NAME}-altai"
            COMPONENT "${COMPONENT_NAME}-dev"
            EXPORT "${PROJECT_NAME}-altai"
            LIBRARY
    )

    install(TARGETS "${PROJECT_NAME}-altai_static"
            EXPORT "${PROJECT_NAME}-altai_static"
            COMPONENT "${COMPONENT_NAME}-dev"
            ARCHIVE)

    install(DIRECTORY "include/${${PROJECT_NAME}_PUBLIC_INCLUDE_DIR}"
            COMPONENT "${COMPONENT_NAME}-dev"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/knp"
            FILES_MATCHING PATTERN "*.h")
endif()
