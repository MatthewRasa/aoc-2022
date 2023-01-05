#include "common.h"
#include <optional>
#include <variant>
#include <ranges>

enum class Direction {
	RIGHT = 0, DOWN = 1, LEFT = 2, UP = 3
};

using Grid = std::vector<std::vector<char>>;
using Instruction = std::variant<int, Direction>;

[[nodiscard]] static std::vector<Instruction> parse_instructions(const std::string &line) {
	std::vector<Instruction> instructions;
	std::string num_str;
	for (auto c : line) {
		if (c == 'R' || c == 'L') {
			if (!num_str.empty()) {
				instructions.emplace_back(std::stoi(num_str));
				num_str.clear();
			}
			instructions.emplace_back(c == 'R' ? Direction::RIGHT : Direction::LEFT);
		} else {
			num_str += c;
		}
	}
	if (!num_str.empty())
		instructions.emplace_back(std::stoi(num_str));
	return instructions;
}

[[nodiscard]] static Grid flush_grid(Grid grid) {
	const auto max_ncols = std::ranges::max_element(grid, [](const auto &lhs, const auto &rhs) { return lhs.size() < rhs.size(); })->size();
	for (auto &row : grid)
		row.resize(max_ncols, ' ');
	return grid;
}

struct Wrapper_2D {

	explicit Wrapper_2D(const Grid &grid) noexcept
			: grid_ptr_{&grid} { }

	[[nodiscard]] std::pair<Grid_Position, Direction> operator()(const Grid_Position &position, Direction direction) const noexcept {
		switch (direction) {
		using enum Direction;
		case RIGHT:
			return std::make_pair(find_first_tile(Grid_Position{position.r, 0}, position), direction);
		case DOWN:
			return std::make_pair(find_first_tile(Grid_Position{0, position.c}, position), direction);
		case LEFT:
			return std::make_pair(find_first_tile(Grid_Position{position.r, (*grid_ptr_)[position.r].size() - 1}, position), direction);
		case UP:
		default:
			return std::make_pair(find_first_tile(Grid_Position{grid_ptr_->size() - 1, position.c}, position), direction);
		}
	}

private:
	const Grid *grid_ptr_;

	[[nodiscard]] Grid_Position find_first_tile(const Grid_Position &start, const Grid_Position &end) const {
		int r_inc, c_inc;
		std::function<bool(std::size_t)> r_eval, c_eval;
		if (start.r <= end.r) {
			r_inc = 1;
			r_eval = [&end](auto r) { return r <= end.r; };
		} else {
			r_inc = -1;
			r_eval = [&end](auto r) { return r >= end.r; };
		}
		if (start.c <= end.c) {
			c_inc = 1;
			c_eval = [&end](auto c) { return c <= end.c; };
		} else {
			c_inc = -1;
			c_eval = [&end](auto c) { return c >= end.c; };
		}
		for (std::size_t r = start.r; r_eval(r); r += r_inc) {
			for (std::size_t c = start.c; c_eval(c); c += c_inc) {
				if ((*grid_ptr_)[r][c] != ' ')
					return Grid_Position{r, c};
			}
		}
		throw std::runtime_error{"Cannot find tile"};
	}
};

struct Wrapper_3D {

	explicit Wrapper_3D(const Grid &grid) noexcept
			: grid_ptr_{&grid} { }

	[[nodiscard]] std::pair<Grid_Position, Direction> operator()(const Grid_Position &position, Direction direction) const {
		const auto &face = determine_face(position);
		switch (direction) {
		using enum Direction;
		case RIGHT:
			switch (face.id) {
			case 2:
				return std::make_pair(Grid_Position{FACES[4].start.r + (face.end.r - 1 - position.r), FACES[4].end.c - 1}, Direction::LEFT);
			case 3:
				return std::make_pair(Grid_Position{FACES[1].end.r - 1, FACES[1].start.c + (position.r - face.start.r)}, Direction::UP);
			case 5:
				return std::make_pair(Grid_Position{FACES[1].end.r - 1 - (position.r - face.start.r), FACES[1].end.c - 1}, Direction::LEFT);
			case 6:
				return std::make_pair(Grid_Position{FACES[4].end.r - 1, FACES[4].start.c + (position.r - face.start.r)}, Direction::UP);
			default:
				throw std::logic_error{"Shouldn't get here"};
			}
		case DOWN:
			switch (face.id) {
			case 2:
				return std::make_pair(Grid_Position{FACES[2].start.r + (position.c - face.start.c), FACES[2].end.c - 1}, Direction::LEFT);
			case 5:
				return std::make_pair(Grid_Position{FACES[5].start.r + (position.c - face.start.c), FACES[5].end.c - 1}, Direction::LEFT);
			case 6:
				return std::make_pair(Grid_Position{FACES[1].start.r, FACES[1].start.c + (position.c - face.start.c)}, Direction::DOWN);
			default:
				throw std::logic_error{"Shouldn't get here"};
			}
		case LEFT:
			switch (face.id) {
			case 1:
				return std::make_pair(Grid_Position{FACES[3].start.r + (face.end.r - 1 - position.r), FACES[3].start.c}, Direction::RIGHT);
			case 3:
				return std::make_pair(Grid_Position{FACES[3].start.r, FACES[3].start.c + (position.r - face.start.r)}, Direction::DOWN);
			case 4:
				return std::make_pair(Grid_Position{FACES[0].end.r - 1 - (position.r - face.start.r), FACES[0].start.c}, Direction::RIGHT);
			case 6:
				return std::make_pair(Grid_Position{FACES[0].start.r, FACES[0].start.c + (position.r - face.start.r)}, Direction::DOWN);
			default:
				throw std::logic_error{"Shouldn't get here"};
			}
		case UP:
		default:
			switch (face.id) {
			case 1:
				return std::make_pair(Grid_Position{FACES[5].start.r + (position.c - face.start.c), FACES[5].start.c}, Direction::RIGHT);
			case 2:
				return std::make_pair(Grid_Position{FACES[5].end.r - 1, FACES[5].start.c + (position.c - face.start.c)}, Direction::UP);
			case 4:
				return std::make_pair(Grid_Position{FACES[2].start.r + (position.c - face.start.c), FACES[2].start.c}, Direction::RIGHT);
			default:
				throw std::logic_error{"Shouldn't get here"};
			}
			break;
		}
		return std::make_pair(position, direction);
	}

private:

	struct Face {
		int id;
		Grid_Position start, end;
	};

	static constexpr std::array<Face, 6> FACES{
		Face{1, Grid_Position{0, 50}, Grid_Position{50, 100}},
		Face{2, Grid_Position{0, 100}, Grid_Position{50, 150}},
		Face{3, Grid_Position{50, 50}, Grid_Position{100, 100}},
		Face{4, Grid_Position{100, 0}, Grid_Position{150, 50}},
		Face{5, Grid_Position{100, 50}, Grid_Position{150, 100}},
		Face{6, Grid_Position{150, 0}, Grid_Position{200, 50}}
	};

	[[nodiscard]] static const Face &determine_face(const Grid_Position &position) {
		auto face_it = std::ranges::find_if(FACES, [&position](const auto &face) {
			return face.start.r <= position.r && position.r < face.end.r
					&& face.start.c <= position.c && position.c < face.end.c;
		});
		if (face_it == FACES.end())
			throw std::logic_error{"Grid position is not on any face"};
		return *face_it;
	}

	const Grid *grid_ptr_;
};

[[nodiscard]] static Grid_Position find_first_open(const Grid &grid, const Grid_Position &start, const Grid_Position &end) {
	for (std::size_t r = start.r; r <= end.r; ++r) {
		for (std::size_t c = start.c; c <= end.c; ++c) {
			if (grid[r][c] == '.')
				return Grid_Position{r, c};
			if (grid[r][c] == '#')
				break;
		}
	}
	throw std::runtime_error{"Cannot find starting point"};
}

template<class WrapperT>
[[nodiscard]] static std::pair<Grid_Position, Direction> travel(const Grid &grid, const WrapperT &wrapper, Grid_Position position, Direction direction, int ntiles) {
	for (int tile = 0; tile < ntiles; ++tile) {
		Grid_Position next_position{position};
		Direction next_direction{direction};
		bool out_of_bounds{false};
		switch (direction) {
		using enum Direction;
		case RIGHT:
			if (next_position.c == grid[next_position.r].size() - 1)
				out_of_bounds = true;
			else
				++next_position.c;
			break;
		case DOWN:
			if (next_position.r == grid.size() - 1)
				out_of_bounds = true;
			else
				++next_position.r;
			break;
		case LEFT:
			if (next_position.c == 0)
				out_of_bounds = true;
			else
				--next_position.c;
			break;
		case UP:
			if (next_position.r == 0)
				out_of_bounds = true;
			else
				--next_position.r;
			break;
		}

		if (out_of_bounds || grid[next_position.r][next_position.c] == ' ')
			std::tie(next_position, next_direction) = wrapper(position, direction);
		if (grid[next_position.r][next_position.c] == '#')
			return std::make_pair(position, direction);
		position = next_position;
		direction = next_direction;
	}
	return std::make_pair(position, direction);
}

template<class WrapperT>
[[nodiscard]] static int password(const Grid &grid, const std::vector<Instruction> &instructions, const WrapperT &wrapper) {
	auto current_pos = find_first_open(grid, Grid_Position{0, 0}, Grid_Position{0, grid.size() - 1});
	auto current_dir = Direction::RIGHT;
	for (const auto &instruction : instructions) {
		if (std::holds_alternative<Direction>(instruction)) {
			if (std::get<Direction>(instruction) == Direction::RIGHT)
				current_dir = static_cast<Direction>((static_cast<int>(current_dir) + 1) % 4);
			else
				current_dir = static_cast<Direction>((static_cast<int>(current_dir) + 3) % 4);
		} else {
			std::tie(current_pos, current_dir) = travel(grid, wrapper, current_pos, current_dir, std::get<int>(instruction));
		}
	}

	return 1000 * (current_pos.r + 1) + 4 * (current_pos.c + 1) + static_cast<int>(current_dir);
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	const auto grid = flush_grid(read_grid(std::cin));
	switch (part) {
	case 1:
		std::cout << password(grid, parse_instructions(read_line(std::cin)), Wrapper_2D{grid}) << std::endl;
		break;
	case 2:
		std::cout << password(grid, parse_instructions(read_line(std::cin)), Wrapper_3D{grid}) << std::endl;
		break;
	}
	return 0;
}
