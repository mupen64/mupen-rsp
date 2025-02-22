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