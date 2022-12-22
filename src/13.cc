#include "common.h"
#include <variant>

struct Packet {
	using Item_Variant_Type = std::variant<Packet, int>;

	Packet() = default;

	explicit Packet(Item_Variant_Type item) {
		items_.push_back(std::move(item));
	}

	explicit Packet(const std::string &str)
			: Packet{str, 0} {}	

	[[nodiscard]] bool operator==(const Packet &other) const noexcept {
		return !(*this < other || *this > other);
	}

	[[nodiscard]] bool operator<(const Packet &other) const noexcept {
		for (std::size_t i = 0; i < other.items_.size(); ++i) {
			if (i == items_.size())
				return true;
			if (item_less(items_[i], other.items_[i]))
				return true;
			if (item_greater(items_[i], other.items_[i]))
				return false;
		}
		return false;
	}

	[[nodiscard]] bool operator>(const Packet &other) const noexcept {
		for (std::size_t i = 0; i < items_.size(); ++i) {
			if (i == other.items_.size())
				return true;
			if (item_greater(items_[i], other.items_[i]))
				return true;
			if (item_less(items_[i], other.items_[i]))
				return false;
		}
		return false;
	}

	friend std::ostream &operator<<(std::ostream &out, const Packet &packet);

private:
	[[nodiscard]] static bool item_less(const Item_Variant_Type &lhs, const Item_Variant_Type &rhs) {
		if (lhs.index() == rhs.index())
			return lhs < rhs;
		if (std::holds_alternative<int>(lhs))
			return Packet{lhs} < std::get<Packet>(rhs);
		return std::get<Packet>(lhs) < Packet{rhs};
	}

	[[nodiscard]] static bool item_greater(const Item_Variant_Type &lhs, const Item_Variant_Type &rhs) {
		if (lhs.index() == rhs.index())
			return lhs > rhs;
		if (std::holds_alternative<int>(lhs))
			return Packet{lhs} > std::get<Packet>(rhs);
		return std::get<Packet>(lhs) > Packet{rhs};
	}

	std::vector<Item_Variant_Type> items_;

	Packet(const std::string &str, std::size_t pos)
			: Packet{str, pos, true} {}

	Packet(const std::string &str, std::size_t &pos, bool) {
		auto int_start = std::string::npos;
		for (++pos; pos < str.size() && str[pos] != ']'; ++pos) {
			if (str[pos] == '[') {
				items_.emplace_back(Packet{str, pos, true});
			} else if (str[pos] == ',') {
				if (int_start != std::string::npos) {
					items_.emplace_back(std::stoi(str.substr(int_start, pos - int_start)));
					int_start = std::string::npos;
				}
			} else if (int_start == std::string::npos) {
				int_start = pos;
			}
		}
		if (int_start != std::string::npos)
			items_.emplace_back(std::stoi(str.substr(int_start, pos - int_start)));
	}
};

std::ostream &operator<<(std::ostream &out, const Packet &packet) {
	out << "[";
	for (auto item_it = packet.items_.begin(); item_it != packet.items_.end(); ++item_it) {
		if (std::holds_alternative<int>(*item_it))
			out << std::get<int>(*item_it);
		else
			out << std::get<Packet>(*item_it);
		if (item_it != std::prev(packet.items_.end()))
			out << ",";
	}
	out << "]";
	return out;
}

struct Packet_Pair : Paragraph_Reader<Packet_Pair> {

	void read_line(const std::string &line) final {
		switch (line_num()) {
		case 0: left_ = Packet{line}; break;
		case 1: right_ = Packet{line}; break;
		}
	}

	[[nodiscard]] bool in_order() const noexcept {
		return left_ < right_;
	}

	[[nodiscard]] Packet &left() noexcept {
		return left_;
	}
	
	[[nodiscard]] Packet &right() noexcept {
		return right_;
	}

private:
	Packet left_, right_;
};

static int sum_in_order_indices(const std::vector<Packet_Pair> &packet_pairs) {
	int sum{0};
	for (std::size_t i = 0; i < packet_pairs.size(); ++i) {
		if (packet_pairs[i].in_order())
			sum += i + 1;
	}
	return sum;
}

static std::vector<Packet> flatten_pairs(std::vector<Packet_Pair> packet_pairs) {
	std::vector<Packet> packets;
	packets.reserve(packet_pairs.size() * 2);
	for (auto &packet_pair : packet_pairs) {
		packets.push_back(std::move(packet_pair.left()));
		packets.push_back(std::move(packet_pair.right()));
	}
	return packets;
}

static int find_decoder_key(std::vector<Packet> packets) {
	std::vector<Packet> dividers{
		Packet{Packet::Item_Variant_Type{Packet{2}}},
		Packet{Packet::Item_Variant_Type{Packet{6}}}
	};
	std::copy(dividers.begin(), dividers.end(), std::back_inserter(packets));
	std::sort(packets.begin(), packets.end());

	int key{1};
	for (std::size_t i = 0; i < packets.size(); ++i) {
		if (std::find(dividers.begin(), dividers.end(), packets[i]) != dividers.end())
			key *= i + 1;
	}
	return key;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Packet_Pair> packet_pairs;
	while (has_input(std::cin))
		packet_pairs.push_back(Packet_Pair::create_from_stream(std::cin));
	switch (part) {
	case 1:
		std::cout << sum_in_order_indices(packet_pairs) << std::endl;
		break;
	case 2:
		std::cout << find_decoder_key(flatten_pairs(std::move(packet_pairs))) << std::endl;
		break;
	}
	return 0;
}
