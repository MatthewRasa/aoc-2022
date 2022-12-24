#include "common.h"

struct Sensor_Info : Line_Reader<Sensor_Info> {

	void read_line(const std::string &line) final {
		const auto sensor_start = line.find("x=");
		const auto sensor_end = line.find(':', sensor_start);
		position_ = read_position(line, sensor_start, sensor_end);
		beacon_position_ = read_position(line, line.find("x=", sensor_end), std::string::npos);
		range_ = manhattan_distance(position_, beacon_position_);
	}

	[[nodiscard]] const Position &nearest_beacon() const noexcept {
		return beacon_position_;
	}

	[[nodiscard]] int min_x_coord() const noexcept {
		return position_.x - range_;
	}

	[[nodiscard]] int max_x_coord() const noexcept {
		return position_.x + range_;
	}

	[[nodiscard]] bool in_range(const Position &position) const noexcept {
		return manhattan_distance(position_, position) <= range_;
	}

	[[nodiscard]] int move_x_past_range(int y) const noexcept {
		return position_.x + range_ - std::abs(y - position_.y) + 1;
	}

private:
	[[nodiscard]] static int manhattan_distance(const Position &lhs, const Position &rhs) noexcept {
		return std::abs(lhs.x - rhs.x) + std::abs(lhs.y - rhs.y);
	}

	[[nodiscard]] static Position read_position(const std::string &line, std::size_t start, std::size_t end) {
		const auto delim_pos = line.find(',', start);
		return Position{std::stoi(line.substr(start + 2, delim_pos)),
				std::stoi(line.substr(delim_pos + 4, end))};
	}

	Position position_{};
	Position beacon_position_{};
	int range_;
};

static Position beacon_position(const std::vector<Sensor_Info> &sensors, int x_min, int y_min, int x_max, int y_max) {
	for (Position position{x_min, y_min}; position.y <= y_max; ++position.y) {
		for (position.x = x_min; position.x <= x_max; ) {
			auto sensor_it = std::find_if(sensors.begin(), sensors.end(), [&position](const auto &sensor) { return sensor.in_range(position); });
			if (sensor_it == sensors.end())
				return position;
			position.x = sensor_it->move_x_past_range(position.y);
		}
	}
	throw std::logic_error{"Beacon not found"};
}

static int num_blind_spots(const std::vector<Sensor_Info> &sensors, int y) {
	const auto x_min = std::min_element(sensors.begin(), sensors.end(),
					  [](const auto &lhs, const auto &rhs) { return lhs.min_x_coord() < rhs.min_x_coord(); })->min_x_coord();
	const auto x_max = std::max_element(sensors.begin(), sensors.end(),
								[](const auto &lhs, const auto &rhs) { return lhs.max_x_coord() < rhs.max_x_coord(); })->max_x_coord();
	int num_spots{0};
	for (Position position{x_min, y}; position.x != x_max; ++position.x) {
		if (std::any_of(sensors.begin(), sensors.end(),
						[&position](const auto &sensor) { return position != sensor.nearest_beacon() && sensor.in_range(position); }))
			++num_spots;
	}
	return num_spots;
}

static long tuning_frequency(const Position &position) {
	return position.x * 4000000l + position.y;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Sensor_Info> sensors;
	while (has_input(std::cin))
		sensors.push_back(Sensor_Info::create_from_stream(std::cin));
	switch (part) {
	case 1:
		std::cout << num_blind_spots(sensors, 2000000) << std::endl;
		break;
	case 2:
		std::cout << tuning_frequency(beacon_position(sensors, 0, 0, 4000000, 4000000)) << std::endl;
		break;
	}
	return 0;
}
