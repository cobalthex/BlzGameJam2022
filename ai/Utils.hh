#pragma once

#include <vector>

template <typename TVal>
void SwapAndDrop(std::vector<TVal>& vector, size_t index)
{
	vector[index] = std::move(vector.back());
	vector.erase(vector.end() - 1);
}
