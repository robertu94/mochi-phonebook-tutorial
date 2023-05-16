# set source files
set (server-src-files
     Provider.cpp
     Backend.cpp)

set (client-src-files
     Client.cpp
     PhonebookHandle.cpp
     AsyncRequest.cpp)

set (admin-src-files
     Admin.cpp)

set (dummy-src-files
     dummy/DummyBackend.cpp)

set (module-src-files
     BedrockModule.cpp)

# load package helper for generating cmake CONFIG packages
include (CMakePackageConfigHelpers)

# where to install files for "find_package"
set (yp-pkg "share/cmake/yp")

# set version fo shared library
set (yp-vers "${YP_VERSION_MAJOR}.${YP_VERSION_MINOR}")

# server library
add_library (yp-server ${server-src-files} ${dummy-src-files})
target_link_libraries (yp-server
    PUBLIC thallium PkgConfig::uuid nlohmann_json::nlohmann_json yokan-client
    PRIVATE spdlog::spdlog coverage_config)
target_include_directories (yp-server PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (yp-server BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (yp-server BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (yp-server
    PROPERTIES VERSION ${YP_VERSION}
    SOVERSION ${YP_VERSION_MAJOR})

# client library
add_library (yp-client ${client-src-files})
target_link_libraries (yp-client
    PUBLIC thallium PkgConfig::uuid nlohmann_json::nlohmann_json
    PRIVATE spdlog::spdlog coverage_config)
target_include_directories (yp-client PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (yp-client BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (yp-client BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (yp-client
    PROPERTIES VERSION ${YP_VERSION}
    SOVERSION ${YP_VERSION_MAJOR})

# admin library
add_library (yp-admin ${admin-src-files})
target_link_libraries (yp-admin PUBLIC thallium PkgConfig::uuid nlohmann_json::nlohmann_json
    PRIVATE spdlog::spdlog coverage_config)
target_include_directories (yp-admin PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (yp-admin BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (yp-admin BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (yp-admin
    PROPERTIES VERSION ${YP_VERSION}
    SOVERSION ${YP_VERSION_MAJOR})

if (${ENABLE_BEDROCK})
# bedrock module library
add_library (yp-bedrock-module ${module-src-files})
target_link_libraries (yp-bedrock-module yp-server yp-admin yp-client bedrock-server coverage_config)
target_include_directories (yp-bedrock-module PUBLIC $<INSTALL_INTERFACE:include>)
target_include_directories (yp-bedrock-module BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_include_directories (yp-bedrock-module BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties (yp-bedrock-module
    PROPERTIES VERSION ${YP_VERSION}
    SOVERSION ${YP_VERSION_MAJOR})
endif ()

# installation stuff (packaging and install commands)
write_basic_package_version_file (
    "yp-config-version.cmake"
    VERSION ${YP_VERSION}
    COMPATIBILITY AnyNewerVersion)

# generate our config file for find_package()
set (INCLUDE_INSTALL_DIR include/)
set (LIB_INSTALL_DIR lib/)
configure_package_config_file (yp-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/yp-config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/yp/cmake
  PATH_VARS INCLUDE_INSTALL_DIR)

# some bits for the pkg-config file
set (DEST_DIR "${CMAKE_INSTALL_PREFIX}")
set (SERVER_PRIVATE_LIBS "-lyp-server")
set (CLIENT_PRIVATE_LIBS "-lyp-client")
set (ADMIN_PRIVATE_LIBS  "-lyp-admin")
configure_file ("yp-server.pc.in" "yp-server.pc" @ONLY)
configure_file ("yp-client.pc.in" "yp-client.pc" @ONLY)
configure_file ("yp-admin.pc.in"  "yp-admin.pc"  @ONLY)

# configure config.h
configure_file ("config.h.in" "config.h" @ONLY)

# "make install" rules
install (TARGETS yp-admin yp-server yp-client
         EXPORT yp-targets
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
if (${ENABLE_BEDROCK})
    install (TARGETS yp-bedrock-module
             ARCHIVE DESTINATION lib
             LIBRARY DESTINATION lib)
endif ()
install (EXPORT yp-targets
         DESTINATION ${yp-pkg}
         FILE "yp-targets.cmake")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/yp-config.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/yp-config-version.cmake"
         DESTINATION ${yp-pkg} )
install (DIRECTORY ../include/yp
         DESTINATION include
         FILES_MATCHING PATTERN "*.h")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/yp-server.pc"
               "${CMAKE_CURRENT_BINARY_DIR}/yp-client.pc"
               "${CMAKE_CURRENT_BINARY_DIR}/yp-admin.pc"
         DESTINATION "lib/pkgconfig/")
