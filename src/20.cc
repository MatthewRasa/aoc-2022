#include "common.h"
#include <cmath>
#include <list>

[[nodiscard]] static std::vector<int> read_input(std::istream &in) {
	std::vector<int> numbers;
	for (int num; in >> num; )
		numbers.push_back(num);
	return numbers;
}

[[nodiscard]] static long bound_number(long num, std::size_t range) noexcept {
	return std::copysign(std::abs(num) % (range - 1), num);
}

[[nodiscard]] static std::vector<long> mix_numbers(const std::vector<int> &numbers, long key, std::size_t times) {
	std::list<long> number_list;
	std::transform(numbers.begin(), numbers.end(), std::back_inserter(number_list), [key](auto num) { return num * key; });

	std::vector<decltype(number_list)::const_iterator> iterators;
	iterators.reserve(number_list.size());
	for (auto it = number_list.cbegin(); it != number_list.cend(); ++it)
		iterators.push_back(it);

	for (std::size_t time = 0; time < times; ++time) {
		for (auto it : iterators) {
			const auto pos = static_cast<std::size_t>(std::distance(number_list.cbegin(), it));
			const auto inc = bound_number(*it, number_list.size());
			const auto mixed_pos = (inc + pos + (inc < 0 ? number_list.size() : 0)) % number_list.size();
			const auto mixed_it = std::next(number_list.begin(), mixed_pos + (*it >= 0 ? 1 : 0));
			number_list.splice(mixed_it, number_list, it);
		}
	}
	return std::vector<long>{number_list.begin(), number_list.end()};
}

[[nodiscard]] static long grove_sum(const std::vector<int> &numbers, long key, std::size_t times) {
	const auto mixed = mix_numbers(numbers, key, times);
	const auto zero_it = std::find(mixed.begin(), mixed.end(), 0);
	if (zero_it == mixed.end())
		throw std::runtime_error{"No 0 value found"};
	const auto zero_pos = std::distance(mixed.begin(), zero_it);

	long sum{0};
	for (auto grove : {1000, 2000, 3000})
		sum += mixed[(zero_pos + grove) % mixed.size()];
	return sum;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	const auto numbers = read_input(std::cin);	
	switch (part) {
	case 1:
		std::cout << grove_sum(numbers, 1, 1) << std::endl;
		break;
	case 2:
		std::cout << grove_sum(numbers, 811589153, 10) << std::endl;
		break;
	}
	return 0;
}
