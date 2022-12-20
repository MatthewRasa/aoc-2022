#include "common.h"
#include <deque>
#include <limits>

static constexpr int INF{std::numeric_limits<int>::max()};

[[nodiscard]] static Grid_Position find_location(const std::vector<std::vector<char>> &grid, char value) {
	for (std::size_t r = 0; r < grid.size(); ++r) {
		for (std::size_t c = 0; c < grid[r].size(); ++c) {
			if (grid[r][c] == value)
				return Grid_Position{r, c};
		}
	}
	throw std::logic_error{"Location not found"};
}

[[nodiscard]] static bool climbable(char from, char to) noexcept {
	return to - from <= 1;
}

template<typename IsClimbableT>
[[nodiscard]] static std::vector<std::vector<int>> shortest_path_costs(const std::vector<std::vector<char>> &grid, const Grid_Position &start,
																	   const IsClimbableT &is_climbable) {
	auto costs = std::vector<std::vector<int>>(grid.size(), std::vector<int>(grid[0].size(), INF));
	costs[start.r][start.c] = 0;
	for (std::deque<Grid_Position> to_visit{start}; !to_visit.empty(); ) {
		const auto [row, col] = to_visit.front();
		to_visit.pop_front();

		std::vector<Grid_Position> nbrs;
		nbrs.reserve(4);
		if (row > 0 && is_climbable(grid[row][col], grid[row - 1][col]))
			nbrs.push_back(Grid_Position{row - 1, col});
		if (row < grid.size() - 1 && is_climbable(grid[row][col], grid[row + 1][col]))
			nbrs.push_back(Grid_Position{row + 1, col});
		if (col > 0 && is_climbable(grid[row][col], grid[row][col - 1]))
			nbrs.push_back(Grid_Position{row, col - 1});
		if (col < grid[0].size() - 1 && is_climbable(grid[row][col], grid[row][col + 1]))
			nbrs.push_back(Grid_Position{row, col + 1});
		for (const auto &nbr : nbrs) {
			if (costs[row][col] + 1 < costs[nbr.r][nbr.c]) {
				costs[nbr.r][nbr.c] = costs[row][col] + 1;
				to_visit.push_back(nbr);
			}
		}
	}
	return costs;
}

[[nodiscard]] static int shortest_path_from_S(std::vector<std::vector<char>> grid) {
	const auto start_pos = find_location(grid, 'S');
	const auto end_pos = find_location(grid, 'E');
	grid[start_pos.r][start_pos.c] = 'a';
	grid[end_pos.r][end_pos.c] = 'z';
	auto costs = shortest_path_costs(grid, start_pos, [](auto from, auto to) { return climbable(from, to); });
	return costs[end_pos.r][end_pos.c];
}

[[nodiscard]] static int shortest_path_from_any_a(std::vector<std::vector<char>> grid) {
	const auto pos_S = find_location(grid, 'S');
	const auto pos_E = find_location(grid, 'E');
	grid[pos_S.r][pos_S.c] = 'a';
	grid[pos_E.r][pos_E.c] = 'z';
	auto costs = shortest_path_costs(grid, pos_E, [](auto from, auto to) { return climbable(to, from); });

	auto min_cost{INF};
	for (std::size_t r = 0; r < grid.size(); ++r) {
		for (std::size_t c = 0; c < grid[r].size(); ++c) {
			if (grid[r][c] == 'a' && costs[r][c] < min_cost)
				min_cost = costs[r][c];
		}
	}
	return min_cost;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	auto grid = read_grid(std::cin);
	switch (part) {
	case 1:
		std::cout << shortest_path_from_S(std::move(grid)) << std::endl;
		break;
	case 2:
		std::cout << shortest_path_from_any_a(std::move(grid)) << std::endl;
		break;
	}
	return 0;
}
