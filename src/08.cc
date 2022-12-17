#include "common.h"
#include <numeric>

static uint count_visible(const std::vector<std::vector<int>> &grid) {
	const auto num_cols = grid[0].size();
	std::vector<std::vector<std::uint8_t>> is_visible(grid.size(), std::vector<std::uint8_t>(num_cols, false));
	auto update_largest = [&grid, &is_visible](auto &largest, std::size_t row, std::size_t col) {
		if (grid[row][col] > largest) {
			is_visible[row][col] = true;
			largest = grid[row][col];
		}
	};

	for (std::size_t row = 0; row < grid.size(); ++row) {
		is_visible[row][0] = is_visible[row][num_cols - 1] = true;
		auto largest = grid[row][0];
		for (std::size_t col = 1; col < num_cols; ++col)
			update_largest(largest, row, col);

		largest = grid[row][num_cols - 1];
		for (ssize_t col = num_cols - 1; col >= 0; --col)
			update_largest(largest, row, col);
	}
	for (std::size_t col = 0; col < num_cols; ++col) {
		is_visible[0][col] = is_visible[grid.size() - 1][col] = true;
		auto largest = grid[0][col];
		for (std::size_t row = 1; row < grid.size(); ++row)
			update_largest(largest, row, col);

		largest = grid[grid.size() - 1][col];
		for (ssize_t row = grid.size() - 1; row >= 0; --row)
			update_largest(largest, row, col);
	}

	return std::accumulate(is_visible.begin(), is_visible.end(), 0u,
			[](auto sum, const auto &row) { return sum + std::count(row.begin(), row.end(), true); });
}

static uint highest_scenic_score(const std::vector<std::vector<int>> &grid) {
	const auto num_rows = grid.size(), num_cols = grid[0].size();
	uint highest_score{0};
	for (std::size_t row = 1; row < num_rows - 1; ++row) {
		for (std::size_t col = 1; col < num_cols - 1; ++col) {
			uint up_view{1}, down_view{1}, left_view{1}, right_view{1};
			while (up_view < row && grid[row - up_view][col] < grid[row][col])
				++up_view;
			while (row + down_view < num_rows - 1 && grid[row + down_view][col] < grid[row][col])
				++down_view;
			while (left_view < col && grid[row][col- left_view] < grid[row][col])
				++left_view;
			while (col + right_view < num_cols - 1 && grid[row][col + right_view] < grid[row][col])
				++right_view;
			highest_score = std::max(highest_score, up_view * down_view * left_view * right_view);
		}
	}
	return highest_score;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	const auto grid = read_integer_grid(std::cin);
	switch (part) {
	case 1:
		std::cout << count_visible(grid) << std::endl;
		break;
	case 2:
		std::cout << highest_scenic_score(grid) << std::endl;
		break;
	}
	return 0;
}
