#pragma once
#include <string>
#include <vector>
#include <string_view>

#define GemUnused(x, ...) (void)(x, __VA_ARGS__)
#define GemArrayCount(a) (sizeof(a) / sizeof((a)[0]))

namespace Gem
{
	// TODO: return option
	std::string LoadFileAsString(const std::string& path);
	// TODO: return option
	std::vector<unsigned char> LoadFileAsBytes(const std::string& path);
	std::string ConvertWideStringToString(const std::wstring& wstr);
	std::string GetBaseFilename(std::string_view path);
	std::string RemoveExtension(std::string_view str);
}