#include "common.h"
#include <algorithm>
#include <numeric>
#include <ranges>

static long snafu_value(char snafu_char) noexcept {
	switch (snafu_char) {
	case '=': return -2;
	case '-': return -1;
	default: return snafu_char - '0';
	}
}

static char snafu_char(long snafu_value) noexcept {
	switch (snafu_value) {
	case -2: return '=';
	case -1: return '-';
	default: return '0' + snafu_value;
	}
}

static long to_decimal(const std::string &snafu) noexcept {
	long decimal{0};
	long base = 1;
	for (auto it = snafu.rbegin(); it != snafu.rend(); ++it) {
		decimal += snafu_value(*it) * base;
		base *= 5;
	}
	return decimal;
}

static long highest_base(long num) noexcept {
	long base, new_base = 1;
	do {
		base = new_base;
		new_base = base * 5;
	} while (new_base <= num);
	return base;
}

static std::string to_snafu(long decimal) {
	std::vector<int> snafu_values{};
	for (auto base = highest_base(decimal); base > 0; base /= 5) {
		snafu_values.push_back(decimal / base);
		int carry{0};
		for (auto it = snafu_values.rbegin(); it != snafu_values.rend(); ++it) {
			*it += carry;
			if (*it <= 2) {
				carry = 0;
				break;
			}
			*it -= 5;
			carry = 1;
		}
		if (carry)
			snafu_values.insert(snafu_values.begin(), 1);
		decimal %= base;
	}

	std::string snafu;
	snafu.reserve(snafu_values.size());
	std::ranges::transform(snafu_values, std::back_inserter(snafu), snafu_char);
	return snafu;
}

static std::string snafu_sum(const std::vector<std::string> &snafu_nums) {
	std::vector<long> decimal_nums(snafu_nums.size());
	std::ranges::transform(snafu_nums, decimal_nums.begin(), [](const auto &snafu) { return to_decimal(snafu); });
	return to_snafu(std::reduce(decimal_nums.begin(), decimal_nums.end()));
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	const auto snafu_nums = read_lines(std::cin);
	switch (part) {
	case 1:
		std::cout << snafu_sum(snafu_nums) << std::endl;
		break;
	case 2:
		break;
	}
	return 0;
}
