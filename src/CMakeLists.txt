add_library(json_packer_lib
    "${PROJECT_SOURCE_DIR}/include/json_packer.hpp"
    "${PROJECT_SOURCE_DIR}/include/json_key_dictionary.hpp"
    json_packer.cpp
    json_key_dictionary.cpp
)
add_library(json_packer::lib ALIAS json_packer_lib)
target_include_directories(json_packer_lib
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)
target_link_libraries(json_packer_lib PRIVATE nlohmann_json::nlohmann_json)

add_executable(json_packer main.cpp)
target_link_libraries(json_packer PRIVATE json_packer::lib)
