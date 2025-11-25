#pragma once

#include <string>
#include <vector>

namespace toolkitx::file {
    bool file_exists(const std::string &path);

    std::string get_file_name(const std::string &file_path);

    std::string get_file_extension(const std::string &file_path);

    std::string get_parent_path(const std::string &file_path);

    std::string get_parent_directory_name(const std::string &file_path);

    std::string get_dir_name(const std::string &file_path);

    std::pair<std::string, std::string> get_dir_and_file_name(const std::string &file_path);

    std::string get_filename_without_ext(const std::string &path);

    std::vector<std::string> traversal_images(const std::string &root);

    std::vector<std::string> recursive_files(
            const std::string &root, const std::vector<std::string> &suffixes = {".jpg", ".jpeg", ".png"}
    );

    bool create_dir_if_not_exists(const std::string &dir_path);

    bool write_binary_file(const std::string &path, const std::string &data);

    bool write_binary_file(const std::string &path, const void *data, size_t size);

    bool read_binary_file(const std::string &path, std::string &data);

    bool read_binary_file(const std::string &path, std::vector<uint8_t> &data);

    bool write_text_file(const std::string &path, const std::string &data);

    bool read_text_file(const std::string &path, std::string &data);

    bool write_meta_info(const std::string &file_path, const std::string &attr, const std::string &datas);

    bool remove_meta_info(const std::string &file_path, const std::string &attr);

    std::string read_meta_info(const std::string &file_path, const std::string &attr);

    void list_meta_info(const std::string &file_path);
}
