#include "common.h"
#include <deque>
#include <memory>
#include <unordered_map>
#include <unordered_set>

struct Valve_Info : Line_Reader<Valve_Info> {

	void read_line(const std::string &line) final {
		const auto name_start = line.find(' ') + 1;
		const auto name_end = line.find(' ', name_start);
		name_ = line.substr(name_start, name_end - name_start);

		const auto rate_start = line.find('=') + 1;
		const auto rate_end = line.find(';', rate_start);
		rate_ = std::stoi(line.substr(rate_start, rate_end - rate_start));

		const auto nbrs_start = line.find("valve");
		for (auto nbr_start = line.find(' ', nbrs_start) + 1;;) {
			auto delim_pos = line.find(',', nbr_start);
			if (delim_pos == std::string::npos) {
				neighbors_.push_back(line.substr(nbr_start));
				break;
			}
			neighbors_.push_back(line.substr(nbr_start, delim_pos - nbr_start));
			nbr_start = delim_pos + 2;
		}
	}

	[[nodiscard]] const std::string &name() const noexcept {
		return name_;
	}

	[[nodiscard]] int rate() const noexcept {
		return rate_;
	}

	[[nodiscard]] const std::vector<std::string> &neighbors() const noexcept {
		return neighbors_;
	}

private:
	std::string name_;
	int rate_;
	std::vector<std::string> neighbors_;
};

struct Valve {

	Valve(std::string name, int rate)
			: name_{std::move(name)}, rate_{rate} {}

	[[nodiscard]] const std::string &name() const noexcept {
		return name_;
	}

	[[nodiscard]] int rate() const noexcept {
		return rate_;
	}

	[[nodiscard]] const std::vector<Valve *> &neighbors() const noexcept {
		return neighbors_;
	}

	void add_neighbor(Valve &neighbor) {
		neighbors_.push_back(&neighbor);
	}

private:
	std::string name_;
	int rate_;
	std::vector<Valve *> neighbors_;
};

using Valve_Graph = std::unordered_map<std::string, std::unique_ptr<Valve>>;

static Valve_Graph create_valve_graph(std::istream &in) {
	std::vector<Valve_Info> valve_infos;
	while (has_input(in))
		valve_infos.push_back(Valve_Info::create_from_stream(in));

	Valve_Graph valves;
	valves.reserve(valve_infos.size());
	for (const auto &valve_info : valve_infos)
		valves.emplace(valve_info.name(), std::make_unique<Valve>(valve_info.name(), valve_info.rate()));
	for (const auto &valve_info : valve_infos) {
		for (const auto &neighbor : valve_info.neighbors())
			valves[valve_info.name()]->add_neighbor(*valves[neighbor]);
	}
	return valves;
}

struct Node {
	struct Mover {
		Mover(const Valve &from, const Valve &to) noexcept
			: from_{&from}, to_{&to} { }

		[[nodiscard]] const Valve &from() const noexcept {
			return *from_;
		}

		[[nodiscard]] const Valve &to() const noexcept {
			return *to_;
		}

		[[nodiscard]] bool operator==(const Mover &other) const noexcept {
			return from_ == other.from_ && to_ == other.to_;
		}
	private:
		const Valve *from_, *to_;
	};

	Node(std::vector<Mover> movers, int time_remaining, int pressure, std::unordered_set<const Valve *> opened_valves = {})
			: movers_{std::move(movers)},
			  time_remaining_{time_remaining},
			  pressure_{pressure},
			  opened_valves_{std::move(opened_valves)} {
		std::sort(movers_.begin(), movers_.end(), [](const auto &lhs, const auto &rhs) {
			return lhs.from().name() < rhs.from().name() || (lhs.from().name() == rhs.from().name() && lhs.to().name() < rhs.to().name());
		});
	}

	[[nodiscard]] const std::vector<Mover> &movers() const noexcept {
		return movers_;
	}

	[[nodiscard]] const Valve &from() const noexcept {
		return movers_[0].from();
	}

	[[nodiscard]] const Valve &to() const noexcept {
		return movers_[0].to();
	}

	[[nodiscard]] int time_remaining() const noexcept {
		return time_remaining_;
	}

	[[nodiscard]] int pressure() const noexcept {
		return pressure_;
	}

	[[nodiscard]] std::size_t num_opened() const noexcept {
		return opened_valves_.size();
	}

	[[nodiscard]] const std::unordered_set<const Valve *> &opened_valves() const noexcept {
		return opened_valves_;
	}

	[[nodiscard]] bool in_open_set(const Valve &valve) const {
		return opened_valves_.contains(&valve);
	}	

	[[nodiscard]] bool operator==(const Node &other) const noexcept {
		return movers_ == other.movers_ && num_opened() == other.num_opened();
	}

private:	
	std::vector<Mover> movers_;
	int time_remaining_;
	int pressure_;
	std::unordered_set<const Valve *> opened_valves_;
};

struct Node_Hasher {
	[[nodiscard]] std::size_t operator()(const Node &node) const noexcept {
		std::string hash_str;
		for (const auto &mover : node.movers())
			hash_str += mover.from().name() + "->" + mover.to().name() + ",";
		return std::hash<std::string>{}(hash_str + ":" + std::to_string(node.num_opened()));
	}
};

struct Mover_Option {
	const Valve *from, *to;
	int pressure_inc;
	std::unordered_set<const Valve *> opened_valves;
};

static int max_pressure(const Valve_Graph &valve_graph, const std::string &start_valve, int total_time, std::size_t num_movers) {
	Node start_node{std::vector<Node::Mover>(num_movers, Node::Mover{*valve_graph.at(start_valve), *valve_graph.at(start_valve)}), total_time, 0};
	std::unordered_set<Node, Node_Hasher> max_nodes{start_node};
	std::deque<Node> to_visit{std::move(start_node)};
	while (!to_visit.empty()) {
		auto node = to_visit.front();
		to_visit.pop_front();

		const auto next_time = node.time_remaining() - 1;
		if (next_time <= 0)
			continue;

		std::vector<std::vector<Mover_Option>> mover_options(node.movers().size());
		for (std::size_t mi = 0; mi < node.movers().size(); ++mi) {
			const auto &mover = node.movers()[mi];
			if (!node.in_open_set(mover.to()))
				mover_options[mi].push_back(Mover_Option{&mover.to(), &mover.to(), mover.to().rate() * next_time, {&mover.to()}});
			for (auto nbr_ptr : mover.to().neighbors())
				mover_options[mi].push_back(Mover_Option{&mover.to(), nbr_ptr, 0, {}});
		}

		for (std::vector<std::size_t> mover_indices(mover_options.size(), 0); mover_indices.front() < mover_options.front().size(); ) {
			std::vector<Node::Mover> movers;
			int pressure{node.pressure()};
			auto opened_valves = node.opened_valves();
			for (std::size_t i = 0; i < mover_indices.size(); ++i) {
				const auto &options = mover_options[i][mover_indices[i]];
				movers.emplace_back(*options.from, *options.to);
				for (auto valve_ptr : options.opened_valves) {
					if (opened_valves.emplace(valve_ptr).second)
						pressure += valve_ptr->rate() * next_time;
				}
			}
			Node next_node{std::move(movers), next_time, pressure, std::move(opened_valves)};

			auto max_it = max_nodes.find(next_node);
			if (max_it == max_nodes.end() || next_node.pressure() > max_it->pressure()) {
				if (max_it != max_nodes.end())
					max_nodes.erase(max_it);
				max_nodes.insert(next_node);
				to_visit.push_back(std::move(next_node));
			}

			for (std::size_t i = mover_indices.size() - 1;;) {
				if (++mover_indices[i] != mover_options[i].size())
					break;
				if (i == 0)
					break;
				mover_indices[i] = 0;
				--i;
			}
		}
	}

	int max_val{0};
	for (const auto &node : max_nodes)
		max_val = std::max(max_val, node.pressure());
	return max_val;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	auto valves = create_valve_graph(std::cin);
	switch (part) {
	case 1:
		std::cout << max_pressure(valves, "AA", 30, 1) << std::endl;
		break;
	case 2:
		std::cout << max_pressure(valves, "AA", 26, 2) << std::endl;
		break;
	}
	return 0;
}
