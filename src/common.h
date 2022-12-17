#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

/* --- Input readers --- */

bool has_input(std::istream &in) {
	return in.peek() != -1;
}

std::string read_line(std::istream &in) {
	std::string line;
	std::getline(in, line);
	return line;
}

std::vector<std::vector<int>> read_integer_grid(std::istream &in) {
	std::vector<std::vector<int>> grid;
	for (std::string line; std::getline(in, line); ) {
		std::vector<int> grid_line(line.size());
		std::transform(line.begin(), line.end(), grid_line.begin(), [](auto c) { return c - '0'; });
		grid.push_back(std::move(grid_line));
	}
	return grid;
}

template<class CRTP, char DelimV = ' '>
struct Token_Reader {
	static CRTP create_from_stream(std::istream &in) {
		std::string line;
		if (!std::getline(in, line))
			throw std::logic_error{"EOF encountered in Token_Reader"};
		std::istringstream ss{line};

		CRTP instance{};
		for (std::string token; std::getline(ss, token, DelimV); ++instance.token_num_)
			instance.read_token(token);
		instance.read_end();
		return instance;
	}

	virtual ~Token_Reader() = default;

	virtual void read_token(const std::string &token) = 0;

	virtual void read_end() { };

	[[nodiscard]] std::size_t token_num() const noexcept {
		return token_num_;
	}

private:
	std::size_t token_num_{0};
};

template<class CRTP>
struct Line_Reader {
	static CRTP create_from_stream(std::istream &in) {
		std::string line;
		if (!std::getline(in, line))
			throw std::logic_error{"EOF encountered in Line_Reader"};

		CRTP instance{};
		instance.read_line(line);
		return instance;
	}

	virtual ~Line_Reader() = default;

	virtual void read_line(const std::string &line) = 0;
};

template<class CRTP>
struct Paragraph_Reader {
	static CRTP create_from_stream(std::istream &in) {
		CRTP instance{};
		for (std::string line; std::getline(in, line) && !line.empty(); ++instance.line_num_)
			instance.read_line(line);
		instance.read_end();
		return instance;
	}

	virtual ~Paragraph_Reader() = default;

	virtual void read_line(const std::string &line) = 0;

	virtual void read_end() { };

	[[nodiscard]] std::size_t line_num() const noexcept {
		return line_num_;
	}

private:
	std::size_t line_num_{0};
};

/* --- Position --- */

struct Position {
	int x, y;
	[[nodiscard]] bool operator==(const Position &other) const noexcept {
		return x == other.x && y == other.y;
	}
};

namespace std {
	template<>
	struct hash<Position> {
		[[nodiscard]] std::size_t operator()(const Position &position) const noexcept {
			return (static_cast<std::size_t>(position.x) << 32) + static_cast<std::size_t>(position.y);
		}
	};

	std::ostream &operator<<(std::ostream &out, const Position &position) {
		out << "<" << position.x << "," << position.y << ">";
		return out;
	}
}

/* --- Visual debugging */

template<typename PositionsT>
void print_grid_positions(const PositionsT &positions, char display_char = '#') {
	print_grid_positions(positions,
						 std::min_element(positions.begin(), positions.end(),
										  [](const auto &p0, const auto &p1) { return p0.x < p1.x; })->x,
						 std::min_element(positions.begin(), positions.end(),
										  [](const auto &p0, const auto &p1) { return p0.y < p1.y; })->y,
					  	 std::max_element(positions.begin(), positions.end(),
											[](const auto &p0, const auto &p1) { return p0.x < p1.x; })->x,
					     std::max_element(positions.begin(), positions.end(),
										  [](const auto &p0, const auto &p1) { return p0.y < p1.y; })->y,
						 display_char);
}

template<typename PositionsT>
void print_grid_positions(const PositionsT &positions, std::size_t min_x, std::size_t min_y, std::size_t max_x, std::size_t max_y, char display_char = '#') {
	std::vector<std::vector<char>> grid(max_y - min_y + 1, std::vector<char>(max_x - min_x + 1, '.'));
	for (const auto &position : positions)
		grid[position.y - min_y][position.x - min_x] = display_char;
	for (const auto &row : grid) {
		std::copy(row.begin(), row.end(), std::ostream_iterator<char>(std::cout));
		std::cout << std::endl;
	}
}

/* --- Boilerplate --- */

uint select_part(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <1|2>" << std::endl;
		std::exit(1);
	}

	if (strncmp(argv[1], "1", 1) == 0) {
		return 1;
	} else if (strncmp(argv[1], "2", 1) == 0) {
		return 2;
	} else {
		std::cerr << argv[0] << ": invalid argument '" << argv[1] << "'" << std::endl;
		std::exit(1);
	}
}
