#include <Gem\pch.h>
#include <Gem\Utils.h>
#include <Gem\Errors.h>

namespace Gem
{
	std::string LoadFileAsString(const std::string& path)
	{
		std::ifstream file(path);
		if (file.is_open())
		{
			std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			return str;
		}
		else
		{
			GemCrash(std::format("failed to load '{}'", path)); // TODO: use exceptions only in exeptional scenarios
		}
	}
	std::vector<unsigned char> LoadFileAsBytes(const std::string& path)
	{
		std::ifstream file{ path, std::ios::binary };
		if (file.is_open())
		{
			std::vector<unsigned char> bytes{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
			return bytes;
		}
		else
		{
			GemCrash(std::format("failed to load '{}'", path)); // TODO: use exceptions only in exeptional scenarios
		}
	}
	std::string ConvertWideStringToString(const std::wstring& wstr)
	{
		// setup converter
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter{};
		// use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		std::string str{ converter.to_bytes(wstr) };
		return str;
	}
	std::string GetBaseFilename(std::string_view path)
	{
		// TODO: this works on windows, not on linux!
		std::string result{ path.substr(path.find_last_of("/\\") + 1) };
		return result;
	}
	std::string RemoveExtension(std::string_view str)
	{
		auto last{ str.find_last_of('.') };
		std::string result{ str.substr(0, last) };
		return result;
	}
}