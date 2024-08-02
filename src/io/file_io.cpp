
#include <string>
#include "io\file_io.h"


std::string read_file_contents(const char* path) {
	std::string s;
	auto fp = std::fopen(path, "rb");

	std::fseek(fp, 0u, SEEK_END);
	auto size = std::ftell(fp);
	std::fseek(fp, 0u, SEEK_SET);
	s.resize(size);

	std::fread(&s[0], 1u, size, fp);
	std::fclose(fp);

	return s;
}
