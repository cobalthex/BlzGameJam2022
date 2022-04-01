#pragma once

#include <vector>
#include <string>
#include <sstream>

template <typename TVal>
void SwapAndDrop(std::vector<TVal>& vector, size_t index)
{
	vector[index] = std::move(vector.back());
	vector.erase(vector.end() - 1);
}

template <typename TFirst, typename TSecond>
std::ostream& operator <<(std::ostream& os, const std::pair<TFirst, TSecond>& pair)
{
	os << pair.first << ": " << pair.second;
}

template <typename... TArgs, template <typename...> typename TIterable>
std::string Join(const std::string_view& sep, const TIterable<TArgs...>& iterable)
{
	std::ostringstream str;
	for (const auto& i : iterable)
	{
		str << i << sep;
	}
	str.seekp(sep.length(), std::ios::end);
	return str.str();
}
