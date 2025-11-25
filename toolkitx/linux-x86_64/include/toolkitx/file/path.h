#pragma once

#include <string>
#include <vector>

namespace toolkitx::file::path {
    std::string path_join(std::initializer_list<std::string> paths);

    std::string get_executable_file_path();

    std::string get_executable_root();
}
