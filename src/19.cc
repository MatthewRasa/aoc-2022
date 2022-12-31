#include "common.h"
#include <numeric>

struct Counts {
	int ore{0}, clay{0}, obsidian{0}, geode{0};

	Counts &operator+=(const Counts &other) noexcept {
		ore += other.ore;
		clay += other.clay;
		obsidian += other.obsidian;
		geode += other.geode;
		return *this;
	}

	Counts &operator-=(const Counts &other) noexcept {
		return *this += Counts{-other.ore, -other.clay, -other.obsidian, -other.geode};
	}

	[[nodiscard]] Counts operator+(const Counts &other) const noexcept {
		return Counts{*this} += other;
	}

	[[nodiscard]] Counts operator-(const Counts &other) const noexcept {
		return Counts{*this} -= other;
	}
};

struct Blueprint : Line_Reader<Blueprint> {

	void read_line(const std::string &line) final {
		std::size_t pos{0};

		id_ = read_id(line, pos);
		ore_robot_ = Costs{read_single_cost(line, pos), 0, 0};
		clay_robot_ = Costs{read_single_cost(line, pos), 0, 0};
		
		const auto [obsidian_robot_ore, obsidian_robot_clay] = read_two_costs(line, pos);
		obsidian_robot_ = Costs{obsidian_robot_ore, obsidian_robot_clay, 0};

		const auto [geode_robot_ore, geode_robot_obsidian] = read_two_costs(line, pos);
		geode_robot_ = Costs{geode_robot_ore, 0, geode_robot_obsidian};
	}

	[[nodiscard]] int id() const noexcept {
		return id_;
	}

	[[nodiscard]] int highest_ore_cost() const noexcept {
		return std::max(ore_robot_.ore, std::max(clay_robot_.ore, std::max(obsidian_robot_.ore, geode_robot_.ore)));
	}

	[[nodiscard]] int highest_clay_cost() const noexcept {
		return std::max(ore_robot_.clay, std::max(clay_robot_.clay, std::max(obsidian_robot_.clay, geode_robot_.clay)));
	}

	[[nodiscard]] int highest_obsidian_cost() const noexcept {
		return std::max(ore_robot_.obsidian, std::max(clay_robot_.obsidian, std::max(obsidian_robot_.obsidian, geode_robot_.obsidian)));
	}

	[[nodiscard]] bool can_build_any_robot(const Counts &resources, int time) const noexcept {
		if (time >= 2 && !can_build_geode_robot(resources))
			return false;
		if (time >= 4 && (!can_build_ore_robot(resources) || !can_build_obsidian_robot(resources)))
			return false;
		if (time >= 6 && !can_build_clay_robot(resources))
			return false;
		return true;
	}

	[[nodiscard]] bool can_build_ore_robot(const Counts &resources) const noexcept {
		auto new_resources = build_ore_robot(resources);
		return new_resources.ore >= 0 && new_resources.clay >= 0 && new_resources.obsidian >= 0;
	}

	[[nodiscard]] bool can_build_clay_robot(const Counts &resources) const noexcept {
		auto new_resources = build_clay_robot(resources);
		return new_resources.ore >= 0 && new_resources.clay >= 0 && new_resources.obsidian >= 0;
	}

	[[nodiscard]] bool can_build_obsidian_robot(const Counts &resources) const noexcept {
		auto new_resources = build_obsidian_robot(resources);
		return new_resources.ore >= 0 && new_resources.clay >= 0 && new_resources.obsidian >= 0;
	}

	[[nodiscard]] bool can_build_geode_robot(const Counts &resources) const noexcept {
		auto new_resources = build_geode_robot(resources);
		return new_resources.ore >= 0 && new_resources.clay >= 0 && new_resources.obsidian >= 0;
	}

	[[nodiscard]] Counts build_ore_robot(Counts resources) const noexcept {
		resources.ore -= ore_robot_.ore;
		resources.clay -= ore_robot_.clay;
		resources.obsidian -= ore_robot_.obsidian;
		return resources;
	}

	[[nodiscard]] Counts build_clay_robot(Counts resources) const noexcept {
		resources.ore -= clay_robot_.ore;
		resources.clay -= clay_robot_.clay;
		resources.obsidian -= clay_robot_.obsidian;
		return resources;
	}

	[[nodiscard]] Counts build_obsidian_robot(Counts resources) const noexcept {
		resources.ore -= obsidian_robot_.ore;
		resources.clay -= obsidian_robot_.clay;
		resources.obsidian -= obsidian_robot_.obsidian;
		return resources;
	}

	[[nodiscard]] Counts build_geode_robot(Counts resources) const noexcept {
		resources.ore -= geode_robot_.ore;
		resources.clay -= geode_robot_.clay;
		resources.obsidian -= geode_robot_.obsidian;
		return resources;
	}

private:
	struct Costs {
		int ore, clay, obsidian;
	};

	[[nodiscard]] static int read_id(const std::string &line, std::size_t &pos) {
		const auto id_start = line.find(' ') + 1;
		const auto id_end = line.find(':', id_start);
		pos = id_end;
		return std::stoi(line.substr(id_start, id_end));
	}

	[[nodiscard]] static int read_single_cost(const std::string &line, std::size_t &pos) {
		const auto cost_start = line.find("costs ", pos) + std::string{"costs "}.size();
		const auto cost_end = line.find(' ', cost_start);
		pos = cost_end;
		return std::stoi(line.substr(cost_start, cost_end));
	}

	[[nodiscard]] static std::pair<int, int> read_two_costs(const std::string &line, std::size_t &pos) {
		const auto cost1_start = line.find("costs ", pos) + std::string{"costs "}.size();
		const auto cost1_end = line.find(' ', cost1_start);
		const auto cost2_start = line.find("and ", cost1_end) + std::string{"and "}.size();
		const auto cost2_end = line.find(' ', cost2_start);
		pos = cost2_end;
		return std::make_pair(std::stoi(line.substr(cost1_start, cost1_end)), std::stoi(line.substr(cost2_start, cost2_end)));
	}

	int id_;
	Costs ore_robot_, clay_robot_, obsidian_robot_, geode_robot_;
};

static std::string hash_key(const Counts &resources, const Counts &robots, int time) {
	return std::to_string(time)
			+ "," + std::to_string(resources.ore)
			+ "," + std::to_string(resources.clay)
			+ "," + std::to_string(resources.obsidian)
			+ "," + std::to_string(resources.geode)
			+ "," + std::to_string(robots.ore)
			+ "," + std::to_string(robots.clay)
			+ "," + std::to_string(robots.obsidian)
			+ "," + std::to_string(robots.geode);
}

static int max_geode(const Blueprint &blueprint, const Counts &resources, const Counts &robots, int time, std::unordered_map<std::string, int> &cache) {
	if (time == 1)
		return resources.geode + robots.geode;

	const auto key = hash_key(resources, robots, time);
	auto it = cache.find(key);
	if (it == cache.end()) {
		const auto new_resources = resources + robots;
		int max_val{-1};
		if (blueprint.can_build_geode_robot(resources))
			max_val = std::max(max_val, max_geode(blueprint, blueprint.build_geode_robot(new_resources), robots + Counts{0, 0, 0, 1}, time - 1, cache));
		if (time >= 4) {
			if (robots.obsidian < blueprint.highest_obsidian_cost() && blueprint.can_build_obsidian_robot(resources))
				max_val = std::max(max_val, max_geode(blueprint, blueprint.build_obsidian_robot(new_resources), robots + Counts{0, 0, 1, 0}, time - 1, cache));
			if (robots.ore < blueprint.highest_ore_cost() && blueprint.can_build_ore_robot(resources))
				max_val = std::max(max_val, max_geode(blueprint, blueprint.build_ore_robot(new_resources), robots + Counts{1, 0, 0, 0}, time - 1, cache));
		}
		if (time >= 6 && robots.ore < blueprint.highest_clay_cost() && blueprint.can_build_clay_robot(resources))
			max_val = std::max(max_val, max_geode(blueprint, blueprint.build_clay_robot(new_resources), robots + Counts{0, 1, 0, 0}, time - 1, cache));
		if (max_val == -1 || !blueprint.can_build_any_robot(resources, time))
			max_val = std::max(max_val, max_geode(blueprint, new_resources, robots, time - 1, cache));
		it = cache.emplace(key, max_val).first;
	}
	return it->second;
}

static int max_geode(const Blueprint &blueprint, int time) {
	std::unordered_map<std::string, int> cache;
	return max_geode(blueprint, Counts{}, Counts{1, 0, 0, 0}, time, cache);
}

static int quality_sum(const std::vector<Blueprint> &blueprints) {
	return std::accumulate(blueprints.begin(), blueprints.end(), 0, [](auto sum, const auto &blueprint) {
		return sum + blueprint.id() * max_geode(blueprint, 24);
	});
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Blueprint> blueprints;
	while (has_input(std::cin))
		blueprints.push_back(Blueprint::create_from_stream(std::cin));
	switch (part) {
	case 1:
		std::cout << quality_sum(blueprints) << std::endl;
		break;
	case 2:
		std::cout << (max_geode(blueprints[0], 32) * max_geode(blueprints[1], 32)  * max_geode(blueprints[2], 32)) << std::endl;
		break;
	}
	return 0;
}
