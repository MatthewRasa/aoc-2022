#include "common.h"
#include <vector>

enum class Direction { LEFT, RIGHT };

static constexpr int CHAMBER_WIDTH{7};

using Chamber = std::vector<std::array<bool, CHAMBER_WIDTH>>;

using Rock_Shape = std::vector<Position>;

static const std::vector<std::vector<Position>> ROCK_SHAPES{
	Rock_Shape{{0, 0}, {1, 0}, {2, 0}, {3, 0}},
	Rock_Shape{{1, 0}, {0, 1}, {1, 1}, {2, 1}, {1, 2}},
	Rock_Shape{{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}},
	Rock_Shape{{0, 0}, {0, 1}, {0, 2}, {0, 3}},
	Rock_Shape{{0, 0}, {1, 0}, {0, 1}, {1, 1}}
};

struct Rock {

	Rock(Rock_Shape blocks, Position position)
			: blocks_{std::move(blocks)},
			  position_{std::move(position)} {
		width_ = std::max_element(blocks_.begin(), blocks_.end(), [](const auto &lhs, const auto &rhs) { return lhs.x < rhs.x; })->x + 1;
	}

	void move_left(const Chamber &chamber) noexcept {
		if (can_move(chamber, -1, 0))
			--position_.x;
	}

	void move_right(const Chamber &chamber) noexcept {
		if (can_move(chamber, 1, 0))
			++position_.x;
	}

	bool try_move_down(const Chamber &chamber) noexcept {
		if (can_move(chamber, 0, -1)) {
			--position_.y;
			return true;
		}
		return false;
	}

	void place(Chamber &chamber) noexcept {
		for (const auto &block : blocks_) {
			const auto row = static_cast<std::size_t>(position_.y + block.y);
			if (row >= chamber.size())
				chamber.resize(row + 1);
			chamber[row][position_.x + block.x] = true;
		}
	}

private:
	Rock_Shape blocks_;
	Position position_;
	int width_;

	bool can_move(const Chamber &chamber, int x, int y) const noexcept {
		for (const auto &block : blocks_) {
			const auto row = position_.y + y + block.y;
			const auto col = position_.x + x + block.x;
			if (row < 0 || col < 0 || col >= CHAMBER_WIDTH || (row < static_cast<int>(chamber.size()) && chamber[row][col]))
				return false;
		}
		return true;
	}
};

static Circular_Queue<Direction> read_input(std::istream &in) {
	std::vector<Direction> directions;
	for (char c; in >> c; ) {
		if (c == '<')
			directions.push_back(Direction::LEFT);
		else if (c == '>')
			directions.push_back(Direction::RIGHT);
		else
			throw std::runtime_error{std::string{"Unexpected input character: "} + c};
	}
	return Circular_Queue<Direction>(std::move(directions));
}

static std::size_t simulate(Circular_Queue<Direction> directions, std::size_t num_rocks) {
	Circular_Queue<Rock_Shape> rock_shapes{ROCK_SHAPES};
	Chamber chamber;
	std::size_t height{0};
	std::size_t prev_rock_position{static_cast<std::size_t>(-1)};
	std::size_t prev_chamber_size{0};
	std::size_t prev_count{0};
	for (std::size_t count = 0; count < num_rocks; ++count) {
		Rock rock{rock_shapes.take(), Position{2, static_cast<int>(chamber.size()) + 3}};
		do {
			if (directions.take() == Direction::LEFT)
				rock.move_left(chamber);
			else
				rock.move_right(chamber);
			if (directions.at_start()) {
				if (rock_shapes.position() == prev_rock_position) {
					const auto pattern_height = chamber.size() - prev_chamber_size;
					const auto pattern_count = count - prev_count;
					const auto remaining_rocks = num_rocks - count;
					height += (remaining_rocks / pattern_count) * pattern_height;
					num_rocks = count + remaining_rocks % pattern_count;
				}
				prev_rock_position = rock_shapes.position();
				prev_chamber_size = chamber.size();
				prev_count = count;
			}
		} while (rock.try_move_down(chamber));
		rock.place(chamber);
	}
	return height + chamber.size();
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	auto directions = read_input(std::cin);
	switch (part) {
	case 1:
		std::cout << simulate(std::move(directions), 2022) << std::endl;
		break;
	case 2:
		std::cout << simulate(std::move(directions), 1000000000000) << std::endl;
		break;
	}
	return 0;
}
