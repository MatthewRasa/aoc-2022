#include "common.h"
#include <unordered_set>

enum class Direction {
	UP, DOWN, LEFT, RIGHT
};

struct Move : Token_Reader<Move> {
	void read_token(const std::string &token) final {
		switch (token_num()) {
		case 0:
			if (token == "U")
				direction_ = Direction::UP;
			if (token == "D")
				direction_ = Direction::DOWN;
			if (token == "L")
				direction_ = Direction::LEFT;
			if (token == "R")
				direction_ = Direction::RIGHT;
			break;
		case 1:
			count_ = std::stoi(token);
			break;
		}
	}
	
	[[nodiscard]] Direction direction() const noexcept {
		return direction_;
	}

	[[nodiscard]] uint count() const noexcept {
		return count_;
	}

private:
	Direction direction_;
	uint count_;
};

static void follow(int &coord, int target) {
	if (coord < target)
		++coord;
	else if (coord > target)
		--coord;
}

static std::size_t model_movement(const std::vector<Direction> &move_directions, std::size_t num_knots) {
	std::vector<Position> knots(num_knots, Position{0, 0});
	std::unordered_set<Position> tail_positions{knots.back()};
	for (auto direction : move_directions) {
		switch (direction) {
			case Direction::UP: --knots.front().y; break;
			case Direction::DOWN: ++knots.front().y; break;
			case Direction::LEFT: --knots.front().x; break;
			case Direction::RIGHT: ++knots.front().x; break;
		}
		for (std::size_t knot_idx = 1; knot_idx < knots.size(); ++knot_idx) {
			if (knots[knot_idx - 1].x - knots[knot_idx].x >= 2) {
				++knots[knot_idx].x;
				follow(knots[knot_idx].y, knots[knot_idx - 1].y);
			} else if (knots[knot_idx].x - knots[knot_idx - 1].x >= 2) {
				--knots[knot_idx].x;
				follow(knots[knot_idx].y, knots[knot_idx - 1].y);
			} else if (knots[knot_idx - 1].y - knots[knot_idx].y >= 2) {
				++knots[knot_idx].y;
				follow(knots[knot_idx].x, knots[knot_idx - 1].x);
			} else if (knots[knot_idx].y - knots[knot_idx - 1].y >= 2) {
				--knots[knot_idx].y;
				follow(knots[knot_idx].x, knots[knot_idx - 1].x);
			}
		}
		tail_positions.insert(knots.back());
	}
	return tail_positions.size();
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Direction> move_directions;
	while (has_input(std::cin)) {
		const auto move = Move::create_from_stream(std::cin);
		for (std::size_t i = 0; i < move.count(); ++i)
			move_directions.push_back(move.direction());
	}

	switch (part) {
	case 1:
		std::cout << model_movement(move_directions, 2) << std::endl;
		break;
	case 2:
		std::cout << model_movement(move_directions, 10) << std::endl;
		break;
	}
	return 0;
}
