add_library(json_packer_lib
    "${PROJECT_SOURCE_DIR}/include/json_packer.hpp"
    "${PROJECT_SOURCE_DIR}/include/json_key_dictionary.hpp"
    "${PROJECT_SOURCE_DIR}/include/json_tlv_object.hpp"
    "${PROJECT_SOURCE_DIR}/include/json_tlv_integral.hpp"
    "${PROJECT_SOURCE_DIR}/include/json_tlv_record.hpp"
    "${PROJECT_SOURCE_DIR}/include/serialization.hpp"
    json_packer.cpp
    json_key_dictionary.cpp
    json_tlv_object.cpp
    json_tlv_record.cpp
    serialization.cpp
)
add_library(json_packer::lib ALIAS json_packer_lib)
target_include_directories(json_packer_lib
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(json_packer_lib PUBLIC nlohmann_json::nlohmann_json)

add_executable(json_packer main.cpp)
target_link_libraries(json_packer PRIVATE json_packer::lib)
