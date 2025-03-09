/*
 * Copyright (c) 2025, hacktarux-azimer-rsp-hle maintainers, contributors, and original authors (Hacktarux, Azimer).
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stdafx.h"
#include "io_helpers.h"

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


std::wstring string_to_wstring(const std::string& str)
{
#ifdef _WIN32
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    if (size_needed <= 0)
        return L"";

    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    wstr.pop_back();
    return wstr;
#endif
}

std::string wstring_to_string(const std::wstring& wstr)
{
#ifdef _WIN32
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0)
        return "";

    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);
    str.pop_back();
    return str;
#endif
}
