#include "common.h"
#include <array>
#include <deque>
#include <vector>

static constexpr std::size_t SCREEN_WIDTH{40};

using Screen = std::array<std::array<char, SCREEN_WIDTH>, 6>;

struct Instruction : Token_Reader<Instruction> {
	enum class Type { NOOP, ADDX };

	void read_token(const std::string &token) final {
		switch (token_num()) {
		case 0:
			type_ = token == "noop" ? Type::NOOP : Type::ADDX;
			break;
		case 1:
			value_ = std::stoi(token);
			break;
		}
	}

	[[nodiscard]] Type type() const noexcept {
		return type_;
	}

	[[nodiscard]] int value() const noexcept {
		return value_;
	}

private:
	Type type_;
	int value_;
};

static std::pair<int, Screen> run_cycles(const std::vector<Instruction> &instructions) {
	Screen screen;
	std::size_t cycle{1}, busy_duration{0};
	int reg_x{1}, sum{0}, current_val{0};
	for (auto inst_it = instructions.begin(); inst_it != instructions.end(); ) {
		if (cycle >= 20 && (cycle - 20) % 40 == 0)
			sum += cycle * reg_x;
		const auto x_pos = (cycle - 1) % SCREEN_WIDTH;
		screen[(cycle - 1) / SCREEN_WIDTH][x_pos] = (reg_x - 1 <= static_cast<int>(x_pos) && static_cast<int>(x_pos) <= reg_x + 1) ? '#' : '.';
		if (busy_duration == 0) {
			switch (inst_it->type()) {
				case Instruction::Type::NOOP:
					busy_duration = 0;
					break;
				case Instruction::Type::ADDX:
					busy_duration = 1;
					current_val = inst_it->value();
					break;
			}
			++inst_it;
		} else if (--busy_duration == 0) {
			reg_x += current_val;
		}
		++cycle;
	}
	return std::make_pair(sum, std::move(screen));
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Instruction> instructions;
	while (has_input(std::cin))
		instructions.push_back(Instruction::create_from_stream(std::cin));
	const auto [signal_sum, screen] = run_cycles(instructions);
	switch (part) {
	case 1:
		std::cout << signal_sum << std::endl;
		break;
	case 2:
		for (const auto &row : screen) {
			for (auto c : row)
				std::cout << c;
			std::cout << std::endl;
		}
		break;
	}
	return 0;
}
