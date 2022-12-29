#include "common.h"
#include <deque>
#include <unordered_set>

static std::vector<Position3D> read_input(std::istream &in) {
	std::vector<Position3D> positions;
	for (std::string line; std::getline(in, line); ) {
		const auto tokens = read_tokens(line, ',');
		positions.push_back(Position3D{std::stoi(tokens[0]) + 1, std::stoi(tokens[1]) + 1, std::stoi(tokens[2]) + 1});
	}
	return positions;
}

static std::tuple<int, int, int> max_coords(const std::vector<Position3D> &positions) {
	const auto max_x = std::max_element(positions.begin(), positions.end(), [](const auto &lhs, const auto &rhs) { return lhs.x < rhs.x; })->x;
	const auto max_y = std::max_element(positions.begin(), positions.end(), [](const auto &lhs, const auto &rhs) { return lhs.y < rhs.y; })->y;
	const auto max_z = std::max_element(positions.begin(), positions.end(), [](const auto &lhs, const auto &rhs) { return lhs.z < rhs.z; })->z;
	return std::make_tuple(max_x, max_y, max_z);
}

static auto create_grid(const std::vector<Position3D> &positions) {
	const auto [max_x, max_y, max_z] = max_coords(positions);
	std::vector<std::vector<std::vector<std::uint8_t>>> grid(max_x + 2,
															 std::vector<std::vector<std::uint8_t>>(max_y + 2,
																 									std::vector<std::uint8_t>(max_z + 2, 0)));
	for (const auto &position : positions)
		grid[position.x][position.y][position.z] = 1;
	return grid;
}

static int surface_area(const std::vector<Position3D> &positions) {
	auto grid = create_grid(positions);
	int count{0};
	for (const auto &position : positions) {
		if (grid[position.x - 1][position.y][position.z] == 0)
			++count;
		if (grid[position.x + 1][position.y][position.z] == 0)
			++count;
		if (grid[position.x][position.y - 1][position.z] == 0)
			++count;
		if (grid[position.x][position.y + 1][position.z] == 0)
			++count;
		if (grid[position.x][position.y][position.z - 1] == 0)
			++count;
		if (grid[position.x][position.y][position.z + 1] == 0)
			++count;
	}
	return count;
}

static int exterior_surface_area(const std::vector<Position3D> &positions) {
	static constexpr std::uint8_t VISITED{255};

	auto grid = create_grid(positions);
	grid[0][0][0] = VISITED;
	std::deque<Position3D> to_visit{Position3D{0, 0, 0}};
	int count{0};
	const auto check_position = [&grid, &to_visit, &count](const Position3D &position) {
		if (0 <= position.x && position.x < static_cast<int>(grid.size())
				&& 0 <= position.y && position.y < static_cast<int>(grid[0].size())
				&& 0 <= position.z && position.z < static_cast<int>(grid[0][0].size())) {
			if (grid[position.x][position.y][position.z] == 0) {
				to_visit.push_back(position);
				grid[position.x][position.y][position.z] = VISITED;
			} else if (grid[position.x][position.y][position.z] == 1) {
				++count;
			}
		}
	};

	while (!to_visit.empty()) {
		const auto position = to_visit.front();
		to_visit.pop_front();

		check_position(Position3D{position.x - 1, position.y, position.z});
		check_position(Position3D{position.x + 1, position.y, position.z});
		check_position(Position3D{position.x, position.y - 1, position.z});
		check_position(Position3D{position.x, position.y + 1, position.z});
		check_position(Position3D{position.x, position.y, position.z - 1});
		check_position(Position3D{position.x, position.y, position.z + 1});
	}
	return count;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	auto positions = read_input(std::cin);
	switch (part) {
	case 1:
		std::cout << surface_area(positions) << std::endl;
		break;
	case 2:
		std::cout << exterior_surface_area(positions) << std::endl;
		break;
	}
	return 0;
}
