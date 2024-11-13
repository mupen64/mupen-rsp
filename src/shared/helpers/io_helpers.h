#pragma once

#include <filesystem>
#include <string>
#include <vector>

/**
 * \brief Gets all files with a specific file extension directly under a directory
 * \param directory The path joiner-terminated directory
 * \param extension The file extension with no period
 */
std::vector<std::string> get_files_with_extension_in_directory(
	const std::string &directory, const std::string &extension);
/**
 * \brief Gets all files under all subdirectory of a specific directory, including the directory's shallow files
 * \param directory The path joiner-terminated directory
 */
std::vector<std::string> get_files_in_subdirectories(
	const std::string& directory);

/**
 * \brief Reads a file into a buffer
 * \param path The file's path
 * \return The file's contents, or an empty vector if the operation failed
 */
std::vector<uint8_t> read_file_buffer(const std::filesystem::path& path);

/**
 * \brief Reads source data into the destination, advancing the source pointer by <c>len</c>
 * \param src A pointer to the source data
 * \param dest The destination buffer
 * \param len The destination buffer's length
 */
void memread(uint8_t** src, void* dest, unsigned int len);
