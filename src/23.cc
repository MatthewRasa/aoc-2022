#include "common.h"
#include <limits>
#include <unordered_set>

static std::vector<Position> read_positions(std::istream &in) {
	std::vector<Position> positions;
	int y{0};
	for (std::string line; std::getline(in, line); ++y) {
		for (int x = 0; x < static_cast<int>(line.size()); ++x) {
			if (line[x] == '#')
				positions.push_back(Position{x, y});
		}
	}
	return positions;
}

struct Move {
	using Check_Func = std::function<bool(const Position &)>;

	explicit Move(const Check_Func &check_blocked, const Position &move_inc)
		: check_blocked_{check_blocked},
		  move_inc_{move_inc} {}

	[[nodiscard]]
	bool can_move(const Position &position) const noexcept {
		return !check_blocked_(position);
	}

	[[nodiscard]]
	Position move(const Position &position) const noexcept {
		return Position{position.x + move_inc_.x, position.y + move_inc_.y};
	}

private:
	Check_Func check_blocked_;
	Position move_inc_;
};

static int count_empty(const std::unordered_set<Position> &position_set) {
	const auto min_y = std::ranges::min_element(position_set, [](const auto &lhs, const auto &rhs) { return lhs.y < rhs.y; })->y;
	const auto max_y = std::ranges::max_element(position_set, [](const auto &lhs, const auto &rhs) { return lhs.y < rhs.y; })->y;
	const auto min_x = std::ranges::min_element(position_set, [](const auto &lhs, const auto &rhs) { return lhs.x < rhs.x; })->x;
	const auto max_x = std::ranges::max_element(position_set, [](const auto &lhs, const auto &rhs) { return lhs.x < rhs.x; })->x;

	int count{0};
	for (int y = min_y; y <= max_y; ++y) {
		for (int x = min_x; x <= max_x; ++x) {
			if (!position_set.contains(Position{x, y}))
				++count;
		}
	}
	return count;
}

static std::pair<std::unordered_set<Position>, int> run_process(const std::vector<Position> &positions,
																std::size_t max_rounds = std::numeric_limits<std::size_t>::max()) {
	std::unordered_set<Position> position_set{positions.begin(), positions.end()};
	Circular_Queue<Move> moves{std::vector<Move>{
		Move{
			[&position_set](const Position &position) {
				return position_set.contains(Position{position.x - 1, position.y - 1})
					|| position_set.contains(Position{position.x, position.y - 1})
					|| position_set.contains(Position{position.x + 1, position.y - 1});
			},
			Position{0, -1}
		},
		Move{
			[&position_set](const Position &position) {
				return position_set.contains(Position{position.x - 1, position.y + 1})
					|| position_set.contains(Position{position.x, position.y + 1})
					|| position_set.contains(Position{position.x + 1, position.y + 1});
			},
			Position{0, 1}
		},
		Move{
			[&position_set](const Position &position) {
				return position_set.contains(Position{position.x - 1, position.y - 1})
					|| position_set.contains(Position{position.x - 1, position.y})
					|| position_set.contains(Position{position.x - 1, position.y + 1});
			},
			Position{-1, 0}
		},
		Move{
			[&position_set](const Position &position) {
				return position_set.contains(Position{position.x + 1, position.y - 1})
					|| position_set.contains(Position{position.x + 1, position.y})
					|| position_set.contains(Position{position.x + 1, position.y + 1});
			},
			Position{1, 0}
		}
	}};

	for (std::size_t round = 1; round <= max_rounds; ++round) {
		std::unordered_map<Position, std::vector<Position>> proposed_moves;
		for (const auto &position : position_set) {
			if (position_set.contains(Position{position.x - 1, position.y - 1})
					|| position_set.contains(Position{position.x, position.y - 1})
					|| position_set.contains(Position{position.x + 1, position.y - 1})
					|| position_set.contains(Position{position.x - 1, position.y})
					|| position_set.contains(Position{position.x + 1, position.y})
					|| position_set.contains(Position{position.x - 1, position.y + 1})
					|| position_set.contains(Position{position.x, position.y + 1})
					|| position_set.contains(Position{position.x + 1, position.y + 1})) {
				auto moves_cpy{moves};
				for (std::size_t i = 0; i < moves_cpy.size(); ++i) {
					const auto &move = moves_cpy.take();
					if (move.can_move(position)) {
						proposed_moves[move.move(position)].push_back(position);
						break;
					}
				}
			}
		}

		int num_moves{0};
		for (const auto &[proposed_position, source_positions] : proposed_moves) {
			if (source_positions.size() == 1) {
				position_set.erase(source_positions.front());
				position_set.insert(proposed_position);
				++num_moves;
			}
		}
		if (num_moves == 0)
			return std::make_pair(position_set, round);

		moves.next();
	}

	return std::make_pair(position_set, max_rounds);
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	const auto positions = read_positions(std::cin);
	switch (part) {
	case 1:
		std::cout << count_empty(run_process(positions, 10).first) << std::endl;
		break;
	case 2:
		std::cout << run_process(positions).second << std::endl;
		break;
	}
	return 0;
}
