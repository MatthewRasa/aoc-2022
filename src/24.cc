#include "common.h"
#include <limits>
#include <queue>
#include <unordered_set>
#include <vector>

enum class Direction {
	RIGHT, LEFT, DOWN, UP
};

struct Cloud {

	Cloud(const Position &init_pos, Direction direction) noexcept
		: init_pos_{init_pos}
		, direction_{direction} {}
	
	[[nodiscard]] Position current_position(int time, int area_width, int area_height) const noexcept {
		Position position{init_pos_};
		switch (direction_) {
		using enum Direction;
		case RIGHT:
			position.x = (position.x + time) % area_width;
			break;
		case LEFT:
			position.x = (position.x + area_width - (time % area_width)) % area_width;
			break;
		case DOWN:
			position.y = (position.y + time) % area_height;
			break;
		case UP:
			position.y = (position.y + area_height - (time % area_height)) % area_height;
			break;
		}
		return position;
	}

private:
	Position init_pos_;
	Direction direction_;
};

struct Blizzard {

	Blizzard(std::vector<Cloud> clouds, int width, int height) noexcept
		: clouds_{std::move(clouds)}
		, width_{width}
		, height_{height} {}
	
	[[nodiscard]] int width() const noexcept {
		return width_;
	}

	[[nodiscard]] int height() const noexcept {
		return height_;
	}

	[[nodiscard]] Position determine_goal() const noexcept {
		return Position{width_ - 1, height_};
	}

	[[nodiscard]] std::unordered_set<Position> cloud_positions(int time) const {
		std::unordered_set<Position> positions;
		std::ranges::transform(clouds_, std::inserter(positions, positions.end()),
				[this, time](const auto &cloud) { return cloud.current_position(time, width_, height_); });
		return positions;
	}

private:
	std::vector<Cloud> clouds_;
	int width_, height_;
};

static Blizzard read_input(std::istream &in) {
	int width{};
	std::vector<Cloud> clouds;
	int y{0};
	for (std::string line; std::getline(in, line); ++y) {
		width = line.size() - 2;
		for (int x = 0; x < static_cast<int>(line.size()); ++x) {
			switch (line[x]) {
			case '>':
				clouds.emplace_back(Position{x - 1, y - 1}, Direction::RIGHT);
				break;
			case '<':
				clouds.emplace_back(Position{x - 1, y - 1}, Direction::LEFT);
				break;
			case 'v':
				clouds.emplace_back(Position{x - 1, y - 1}, Direction::DOWN);
				break;
			case '^':
				clouds.emplace_back(Position{x - 1, y - 1}, Direction::UP);
				break;
			}
		}
	}
	return Blizzard{std::move(clouds), width, y - 2};
}

struct Node {
	Position position;
	int time;

	[[nodiscard]] bool operator==(const Node &other) const noexcept {
		return position == other.position && time == other.time;
	}
};

struct Node_Hasher {
	[[nodiscard]] std::size_t operator()(const Node &node) const noexcept {
		return std::hash<std::string>{}(std::to_string(node.position.x) + "," + std::to_string(node.position.y) + "," + std::to_string(node.time));
	}
};

static bool in_bounds(const Position &position, int width, int height, const Position &start, const Position &goal) {
	return position == start || position == goal || (0 <= position.x && position.x < width && 0 <= position.y && position.y < height);
}

static int min_time(const Blizzard &blizzard, const Position &start, const Position &goal, int init_time = 0) {
	const auto compare_nodes = [&start](const Node &lhs, const Node &rhs) {
		return lhs.time - (std::abs(lhs.position.x - start.x) + std::abs(lhs.position.y - start.y))
					> rhs.time - (std::abs(rhs.position.x - start.x) + std::abs(rhs.position.y - start.y));
	};

	std::priority_queue<Node, std::vector<Node>, decltype(compare_nodes)> to_visit{compare_nodes};
	std::unordered_set<Node, Node_Hasher> visited;
	to_visit.push(Node{start, init_time});
	visited.insert(Node{start, init_time});
	while (!to_visit.empty()) {
		const auto node = to_visit.top();
		to_visit.pop();
		if (node.position == goal)
			return node.time;

		const auto next_time = node.time + 1;
		const auto cloud_positions = blizzard.cloud_positions(next_time);
		for (const auto position : {Position{node.position.x, node.position.y},
									Position{node.position.x + 1, node.position.y},
									Position{node.position.x - 1, node.position.y},
									Position{node.position.x, node.position.y + 1},
									Position{node.position.x, node.position.y - 1}}) {
			const Node next_node{position, next_time};
			if (in_bounds(position, blizzard.width(), blizzard.height(), start, goal)
					&& !cloud_positions.contains(position)
					&& !visited.contains(next_node)) {
				to_visit.push(next_node);
				visited.insert(next_node);
			}
		}
	}
	throw std::runtime_error{"Never reached goal"};
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	const auto blizzard = read_input(std::cin);
	switch (part) {
	case 1:
		std::cout << min_time(blizzard, Position{0, -1}, blizzard.determine_goal()) << std::endl;
		break;
	case 2:
		std::cout << min_time(blizzard, Position{0, -1}, blizzard.determine_goal(),
						min_time(blizzard, blizzard.determine_goal(), Position{0, -1},
							min_time(blizzard, Position{0, -1}, blizzard.determine_goal()))) << std::endl;
		break;
	}
	return 0;
}
