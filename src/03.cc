#include "common.h"
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <vector>

struct Rucksack : Line_Reader<Rucksack> {
	using Reader_Type = Line_Reader<Rucksack>;
	using Reader_Type::Reader_Type;

	[[nodiscard]] static int priority(char item) noexcept {
		return 'a' <= item && item <= 'z' ? item - 'a' + 1 : item - 'A' + 27;
	}

	void read_line(const std::string &line) final {
		items_ = line;
	}

	[[nodiscard]] const std::string &items() const noexcept {
		return items_;
	}

	[[nodiscard]] int mismatched_priority() const {	
		return priority(mismatched_item());
	}

	[[nodiscard]] char mismatched_item() const {
		const auto half_point = items_.begin() + std::distance(items_.begin(), items_.end()) / 2;
		std::unordered_set<char> compartment1{items_.begin(), half_point};
		return *std::find_if(half_point, items_.end(),
				[&compartment1](auto item) { return compartment1.find(item) != compartment1.end(); });
	}
private:	
	std::string items_;
};

[[nodiscard]] static int mismatched_priority(const Rucksack &rucksack1, const Rucksack &rucksack2, const Rucksack &rucksack3) {
	std::unordered_set<char> rucksack1_set{rucksack1.items().begin(), rucksack1.items().end()};

	std::unordered_set<char> rucksack12_set{};
	std::copy_if(rucksack2.items().begin(), rucksack2.items().end(), std::inserter(rucksack12_set, rucksack12_set.end()),
			[&rucksack1_set](auto item) { return rucksack1_set.find(item) != rucksack1_set.end(); });

	return Rucksack::priority(*std::find_if(rucksack3.items().begin(), rucksack3.items().end(),
			[&rucksack12_set](auto item) { return rucksack12_set.find(item) != rucksack12_set.end(); }));
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Rucksack> rucksacks;
	while (has_input(std::cin))
		rucksacks.push_back(Rucksack::create_from_stream(std::cin));
	switch (part) {
	case 1:
		std::cout << std::accumulate(rucksacks.begin(), rucksacks.end(), 0,
				[](auto sum, const auto &rucksack) { return sum + rucksack.mismatched_priority(); }) << std::endl;
		break;
	case 2:
		{
			int sum{0};
			for (auto rucksack_it = rucksacks.begin(); rucksack_it != rucksacks.end(); rucksack_it += 3)
				sum += mismatched_priority(*rucksack_it, *std::next(rucksack_it, 1), *std::next(rucksack_it, 2));
			std::cout << sum << std::endl;
			break;
		}
	}
	return 0;
}
