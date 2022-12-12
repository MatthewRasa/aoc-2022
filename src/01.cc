#include "common.h"
#include <algorithm>
#include <numeric>
#include <vector>

struct Elf : Paragraph_Reader<Elf> {
	using Reader_Type = Paragraph_Reader<Elf>;
	using Reader_Type::Reader_Type;

	[[nodiscard]] int total() const noexcept {
		return total_;
	}

	void read_line(const std::string &line) final {
		item_calories_.push_back(std::stoi(line));
	}

	void read_end() final {
		total_ = std::reduce(item_calories_.begin(), item_calories_.end());
	}
private:
	std::vector<int> item_calories_;
	int total_;
};

struct Compare_Elves {
	bool operator()(const Elf &elf1, const Elf &elf2) const noexcept {
		return elf1.total() < elf2.total();
	}
};

int sum_top_three(std::vector<Elf> elves) {
	const auto top3_iter = std::prev(elves.end(), 3);
	std::nth_element(elves.begin(), top3_iter, elves.end(), Compare_Elves{});
	return std::accumulate(top3_iter, elves.end(), 0, [](auto sum, const auto &elf) { return sum + elf.total(); });
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Elf> elves;
	while (has_input(std::cin))
		elves.emplace_back(std::cin);

	switch (part) {
	case 1:
		std::cout << std::max_element(elves.begin(), elves.end(), Compare_Elves{})->total() << std::endl;
		break;
	case 2:
		std::cout << sum_top_three(std::move(elves)) << std::endl;
		break;
	}
	return 0;
}
