add_executable (AdminTest AdminTest.cpp)
target_link_libraries (AdminTest PRIVATE Catch2::Catch2WithMain yp-server yp-admin)
add_test (NAME AdminTest COMMAND ./AdminTest)

add_executable (ClientTest ClientTest.cpp)
target_link_libraries (ClientTest PRIVATE Catch2::Catch2WithMain yp-server yp-client yp-admin)
add_test (NAME ClientTest COMMAND ./ClientTest)

add_executable (PhonebookTest PhonebookTest.cpp)
target_link_libraries (PhonebookTest PRIVATE Catch2::Catch2WithMain yp-server yp-client yp-admin)
add_test (NAME PhonebookTest COMMAND ./PhonebookTest)
