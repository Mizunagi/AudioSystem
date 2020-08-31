#include "Utilities.h"

std::wstring Convert_Str_to_Wstr(std::string const& src) {
	std::size_t converted{};
	std::vector<wchar_t> dest(src.size(), L'\0');
	if (::_mbstowcs_s_l(&converted, dest.data(), dest.size(), src.data(), _TRUNCATE, ::_create_locale(LC_ALL, "jpn")) != 0) {
		throw std::system_error{ errno, std::system_category() };
	}
	dest.resize(std::char_traits<wchar_t>::length(dest.data()));
	dest.shrink_to_fit();
	return std::wstring(dest.begin(), dest.end());
}

std::string Convert_Wstr_to_Str(std::wstring const& src) {
	std::size_t converted{};
	std::vector<char> dest(src.size() * sizeof(wchar_t) + 1, '\0');
	if (::_wcstombs_s_l(&converted, dest.data(), dest.size(), src.data(), _TRUNCATE, ::_create_locale(LC_ALL, "jpn")) != 0) {
		throw std::system_error{ errno, std::system_category() };
	}
	dest.resize(std::char_traits<char>::length(dest.data()));
	dest.shrink_to_fit();
	return std::string(dest.begin(), dest.end());
}