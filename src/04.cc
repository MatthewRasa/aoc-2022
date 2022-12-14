#include "common.h"
#include <numeric>
#include <vector>

struct Assignment_Pair : Token_Reader<Assignment_Pair, ','> {
	using Reader_Type = Token_Reader<Assignment_Pair, ','>;
	using Reader_Type::Reader_Type;

	void read_token(const std::string &token) final {
		const auto delim_pos = token.find('-');
		(token_num() == 0 ? range1_ : range2_) = std::make_pair(std::stoi(token.substr(0, delim_pos)),
																std::stoi(token.substr(delim_pos + 1)));
	}

	[[nodiscard]] bool fully_overlapping() const noexcept {
		return (range1_.first <= range2_.first && range2_.second <= range1_.second)
			|| (range2_.first <= range1_.first && range1_.second <= range2_.second);
	}

	[[nodiscard]] bool partially_overlapping() const noexcept {
		return (range2_.first <= range1_.first && range1_.first <= range2_.second)
			|| (range1_.first <= range2_.first && range2_.first <= range1_.second);
	}

private:
	std::pair<int, int> range1_, range2_;
};

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Assignment_Pair> assignment_pairs;
	while (has_input(std::cin))
		assignment_pairs.push_back(Assignment_Pair::create_from_stream(std::cin));
	switch (part) {
	case 1:
		std::cout << std::accumulate(assignment_pairs.begin(), assignment_pairs.end(), 0, [](auto sum, const auto &pair) { return sum + pair.fully_overlapping(); }) << std::endl;
		break;
	case 2:
		std::cout << std::accumulate(assignment_pairs.begin(), assignment_pairs.end(), 0, [](auto sum, const auto &pair) { return sum + pair.partially_overlapping(); }) << std::endl;
		break;
	}
	return 0;
}
