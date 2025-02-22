#include "helpers/io_helpers.h"
#include <cstdio>
#include <Windows.h>
#include <shlobj.h>
#include <string>
#include <vector>

std::vector<std::string> get_files_with_extension_in_directory(
	const std::string& directory, const std::string& extension)
{
	WIN32_FIND_DATA find_file_data;
	const HANDLE h_find = FindFirstFile((directory + "*." + extension).c_str(),
	                                    &find_file_data);
	if (h_find == INVALID_HANDLE_VALUE)
	{
		return {};
	}

	std::vector<std::string> paths;

	do
	{
		if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			paths.emplace_back(directory + find_file_data.cFileName);
		}
	}
	while (FindNextFile(h_find, &find_file_data) != 0);

	FindClose(h_find);

	return paths;
}

std::vector<std::string> get_files_in_subdirectories(
	const std::string& directory)
{
	std::string newdir = directory;
	if (directory.back() != '\0')
	{
		newdir.push_back('\0');
	}

	WIN32_FIND_DATA find_file_data;
	const HANDLE h_find = FindFirstFile((directory + "*").c_str(),
	                                    &find_file_data);
	if (h_find == INVALID_HANDLE_VALUE)
	{
		return {};
	}

	std::vector<std::string> paths;
	std::string fixed_path = directory;
	do
	{
		if (strcmp(find_file_data.cFileName, ".") != 0 && strcmp(
			find_file_data.cFileName, "..") != 0)
		{
			std::string full_path = directory + find_file_data.cFileName;
			if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (directory[directory.size() - 2] == '\0')
				{
					if (directory.back() == '\\')
					{
						fixed_path.pop_back();
						fixed_path.pop_back();
					}
				}
				if (directory.back() != '\\')
				{
					fixed_path.push_back('\\');
				}
				full_path = fixed_path + find_file_data.cFileName;
				for (const auto& path : get_files_in_subdirectories(
					     full_path + "\\"))
				{
					paths.push_back(path);
				}
			} else
			{
				paths.push_back(full_path);
			}
		}
	}
	while (FindNextFile(h_find, &find_file_data) != 0);

	FindClose(h_find);

	return paths;
}

void copy_to_clipboard(HWND owner, const std::string& str)
{
	OpenClipboard(owner);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, str.size() + 1);
	if (hg)
	{
		memcpy(GlobalLock(hg), str.c_str(), str.size() + 1);
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT, hg);
		CloseClipboard();
		GlobalFree(hg);
	} else
	{
		printf("Failed to copy\n");
		CloseClipboard();
	}
}

std::vector<uint8_t> read_file_buffer(const std::filesystem::path& path)
{
	FILE* f = fopen(path.string().c_str(), "rb");

	if (!f)
	{
		return {};
	}

	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	fseek(f, 0, SEEK_SET);

	std::vector<uint8_t> b;
	b.resize(len);

	fread(b.data(), sizeof(uint8_t), len, f);

	fclose(f);
	return b;
}

void memread(uint8_t** src, void* dest, unsigned int len)
{
	memcpy(dest, *src, len);
	*src += len;
}