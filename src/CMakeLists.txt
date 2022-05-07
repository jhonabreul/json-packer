add_library(json_packer_lib
    "${PROJECT_SOURCE_DIR}/include/dummy.hpp"
    dummy.cpp
)
add_library(json_packer::lib ALIAS json_packer_lib)
# target_link_libraries(cmake_cpp_boilerplate_lib
#   PUBLIC cmake_cpp_boilerplate::compiler_options fmt::fmt)
target_include_directories(json_packer_lib
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)

add_executable(json_packer main.cpp)
target_link_libraries(json_packer
    PRIVATE json_packer::lib nlohmann_json::nlohmann_json
)
