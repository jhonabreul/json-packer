set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# find_package(Catch2 REQUIRED)

add_executable(json_packer_test
	main.cpp
	json_key_dictionary_test.cpp
	json_tlv_integral_test.cpp
	json_tlv_string_test.cpp
	json_tlv_null_test.cpp
	json_tlv_float_test.cpp
	json_tlv_record_test.cpp
	json_packer_test.cpp
)

target_link_libraries(json_packer_test
	PRIVATE json_packer::lib Catch2::Catch2
)

enable_testing()

add_test(
	NAME json_packer_test
	COMMAND "${CMAKE_BINARY_DIR}/bin/json_packer_test"
)

# include(CTest)
# include(Catch)
# catch_discover_tests(json_packer_test)
