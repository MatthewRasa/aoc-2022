#include "common.h"
#include <cmath>

static Position read_position(const std::string &str) {
	const auto delim_pos = str.find(',');
	return Position{std::stoi(str.substr(0, delim_pos)), std::stoi(str.substr(delim_pos + 1))};
}

static std::vector<Position> read_positions(std::istream &in) {
	std::vector<Position> positions;
	std::stringstream ss{read_line(in)};
	for (std::string token; std::getline(ss, token, ' '); ) {
		if (token != "->")
			positions.push_back(read_position(token));
	}
	return positions;
}

static void draw_rock(std::vector<std::vector<char>> &grid, const Position &position) {
	if (static_cast<std::size_t>(position.y) >= grid.size())
		grid.resize(position.y + 1);
	auto max_length = std::max(static_cast<std::size_t>(position.x + 1),
							   std::max_element(grid.begin(), grid.end(),
								   				[](const auto &row0, const auto &row1) { return row0.size() < row1.size(); })->size());
	for (auto &row : grid)
		row.resize(max_length, '.');
	grid[position.y][position.x] = '#';
}

static void draw_rocks(std::vector<std::vector<char>> &grid, std::istream &in) {
	auto positions = read_positions(in);
	draw_rock(grid, positions.front());
	for (auto position_it = std::next(positions.begin()); position_it != positions.end(); ++position_it) {
		if (position_it->x == std::prev(position_it)->x) {
			const auto dist = position_it->y - std::prev(position_it)->y;
			for (auto position = *std::prev(position_it); ; position.y += std::copysign(1, dist)) {
				draw_rock(grid, position);
				if (position.y == position_it->y)
					break;
			}
		} else if (position_it->y == std::prev(position_it)->y) {
			const auto dist = position_it->x - std::prev(position_it)->x;
			for (auto position = *std::prev(position_it); ; position.x += std::copysign(1, dist)) {
				draw_rock(grid, position);
				if (position.x == position_it->x)
					break;
			}
		} else {
			throw std::logic_error{"Not a valid line"};
		}
	}
}

static int drop_sand(std::vector<std::vector<char>> &grid) {
	static constexpr int source_x{500}, source_y{0};
	int unit_num;
	for (unit_num = 0; grid[source_y][source_x] == '.'; ++unit_num) {
		Position sand{source_x, source_y};
		for (;; ++sand.y) {
			if (sand.y + 1 == static_cast<int>(grid.size()))
				return unit_num;
			if (grid[sand.y + 1][sand.x] != '.') {
				if (sand.x - 1 < 0)
					return unit_num;
				else if (grid[sand.y + 1][sand.x - 1] == '.')
					--sand.x;
				else if (sand.x + 1 == static_cast<int>(grid[sand.y].size()))
					return unit_num;
				else if (grid[sand.y + 1][sand.x + 1] == '.')
					++sand.x;
				else
					break;
			}
		}
		grid[sand.y][sand.x] = '*';
	}
	return unit_num;
}

static std::size_t first_horizontal_rock_position(const std::vector<std::vector<char>> &grid) {
	std::size_t position{std::numeric_limits<std::size_t>::max()};
	for (const auto &row : grid)
		position = std::min(position, static_cast<std::size_t>(std::distance(row.begin(), std::find_if_not(row.begin(), row.end(),
																										   [](auto c) { return c == '.'; }))));
	return position;
}

[[maybe_unused]] static void draw_grid(const std::vector<std::vector<char>> &grid) {
	const auto first_position = first_horizontal_rock_position(grid);
	for (const auto &row : grid) {
		for (std::size_t i = first_position; i < row.size(); ++i)
			std::cout << row[i];
		std::cout << std::endl;
	}
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<std::vector<char>> grid;
	while (has_input(std::cin))
		draw_rocks(grid, std::cin);
	switch (part) {
	case 1:
		std::cout << drop_sand(grid) << std::endl;
		break;
	case 2:
		for (Position position{0, static_cast<int>(grid.size()) + 1}; position.x < 1000; ++position.x)
			draw_rock(grid, position);
		std::cout << drop_sand(grid) << std::endl;
		break;
	}
	return 0;
}
