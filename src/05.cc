#include "common.h"
#include <array>
#include <numeric>
#include <vector>

using Crates = std::array<std::vector<char>, 9>;

struct Instruction : Token_Reader<Instruction> {
	using Reader_Type = Token_Reader<Instruction>;
	using Reader_Type::Reader_Type;

	void read_token(const std::string &token) final {
		if (last_token_ == "move") {
			count_ = std::stoi(token);
		} else if (last_token_ == "from") {
			from_ = std::stoi(token) - 1;
		} else if (last_token_ == "to") {
			to_ = std::stoi(token) - 1;
		}
		last_token_ = token;
	}

	void apply_single_move(Crates &crates) {
		std::move(crates[from_].rbegin(), crates[from_].rbegin() + count_, std::back_inserter(crates[to_]));
		crates[from_].resize(crates[from_].size() - count_);
	}

	void apply_multi_move(Crates &crates) {
		std::move(crates[from_].end() - count_, crates[from_].end(), std::back_inserter(crates[to_]));
		crates[from_].resize(crates[from_].size() - count_);
	}
private:
	std::string last_token_{};
	int count_;
	std::size_t from_, to_;
};

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	Crates crates{
		std::vector<char>{'R', 'G', 'J', 'B', 'T', 'V', 'Z'},
		std::vector<char>{'J', 'R', 'V', 'L'},
		std::vector<char>{'S', 'Q', 'F'},
		std::vector<char>{'Z', 'H', 'N', 'L', 'F', 'V', 'Q', 'G'},
		std::vector<char>{'R', 'Q', 'T', 'J', 'C', 'S', 'M', 'W'},
		std::vector<char>{'S', 'W', 'T', 'C', 'H', 'F'},
		std::vector<char>{'D', 'Z', 'C', 'V', 'F', 'N', 'J'},
		std::vector<char>{'L', 'G', 'Z', 'D', 'W', 'R', 'F', 'Q'},
		std::vector<char>{'J', 'B', 'W', 'V', 'P'}
	};

	std::vector<Instruction> instructions;
	while (has_input(std::cin))
		instructions.push_back(Instruction::create_from_stream(std::cin));
	switch (part) {
	case 1:
		for (auto &instruction : instructions)
			instruction.apply_single_move(crates);
		break;
	case 2:
		for (auto &instruction : instructions)
			instruction.apply_multi_move(crates);
		break;
	}
	for (const auto &stack : crates)
		std::cout << stack.back();
	std::cout << std::endl;
	return 0;
}
