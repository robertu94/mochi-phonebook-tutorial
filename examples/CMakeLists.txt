add_executable (example-server ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp)
target_link_libraries (example-server yp-server)

add_executable (example-admin ${CMAKE_CURRENT_SOURCE_DIR}/admin.cpp)
target_link_libraries (example-admin yp-admin)

add_executable (example-client ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp)
target_link_libraries (example-client yp-client)
