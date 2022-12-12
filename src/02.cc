#include "common.h"
#include <numeric>
#include <vector>

struct Round : Token_Reader<Round> {
	using Reader_Type = Token_Reader<Round>;
	using Reader_Type::Reader_Type;

	enum class Selection { ROCK = 0, PAPER = 1, SCISSORS = 2 };
	enum class Code { X = 0, Y = 1, Z = 2 };

	void read_token(const std::string &token) final {
		switch (token_num()) {
		case 0:
			switch (token[0]) {
			case 'A': opponent_ = Selection::ROCK; break;
			case 'B': opponent_ = Selection::PAPER; break;
			case 'C': opponent_ = Selection::SCISSORS; break;
			}
			break;
		case 1:
			switch (token[0]) {
			case 'X': code_ = Code::X; break;
			case 'Y': code_ = Code::Y; break;
			case 'Z': code_ = Code::Z; break;
			}
			break;
		}
	}

	[[nodiscard]] int score1() const noexcept {
		const auto move = static_cast<Selection>(code_);
		if (static_cast<Selection>((static_cast<int>(move) + 1) % 3) == opponent_)
			return 1 + static_cast<int>(move);
		else if (move == opponent_)
			return 4 + static_cast<int>(move);
		else
			return 7 + static_cast<int>(move);
	}

	[[nodiscard]] int score2() const noexcept {
		Selection move;
		int score;
		switch (code_) {
		case Code::X:
			move = static_cast<Selection>((static_cast<int>(opponent_) + 2) % 3);
			score = 0;
			break;
		case Code::Y:
			move = opponent_;
			score = 3;
			break;
		case Code::Z:
		default:
			move = static_cast<Selection>((static_cast<int>(opponent_) + 1) % 3);
			score = 6;
			break;
		}
		return score + static_cast<int>(move) + 1;
	}
private:
	Selection opponent_;
	Code code_;
};

[[nodiscard]] static int total_score1(const std::vector<Round> &rounds) {
	return std::accumulate(rounds.begin(), rounds.end(), 0, [](auto sum, const auto &round) { return sum + round.score1(); });
}

[[nodiscard]] static int total_score2(const std::vector<Round> &rounds) {
	return std::accumulate(rounds.begin(), rounds.end(), 0, [](auto sum, const auto &round) { return sum + round.score2(); });
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Round> rounds;
	while (has_input(std::cin))
		rounds.emplace_back(std::cin);
	switch (part) {
	case 1:
		std::cout << total_score1(rounds) << std::endl;
		break;
	case 2:
		std::cout << total_score2(rounds) << std::endl;
		break;
	}
	return 0;
}
