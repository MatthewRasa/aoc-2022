#include "common.h"
#include <deque>
#include <numeric>
#include <vector>

using Item_Type = unsigned long;

struct Monkey : Paragraph_Reader<Monkey> {
	enum class Operator { ADD, MULTIPLY };

	void read_line(const std::string &line) final {
		const auto delim_pos = line.find(':');
		switch (line_num()) {
		case 0:
			id_ = std::stoi(line.substr(delim_pos - 1, delim_pos));
			break;
		case 1:
			for (std::size_t delim_pos_old = delim_pos + 2, delim_pos_new = 0; delim_pos_new != std::string::npos; delim_pos_old = delim_pos_new + 2) {
				delim_pos_new = line.find(',', delim_pos_old);
				items_.push_back(std::stoi(line.substr(delim_pos_old, delim_pos_new - delim_pos_old)));
			}
			break;
		case 2:
		{
			std::size_t op_delim;
			if (op_delim = line.find('+', delim_pos + 2); op_delim != std::string::npos)
				operator_ = Operator::ADD;
			else if (op_delim = line.find('*', delim_pos + 2); op_delim != std::string::npos)
				operator_ = Operator::MULTIPLY;
			else
				throw std::logic_error{"No operator found"};

			const auto value_str = line.substr(op_delim + 2);
			operation_value_ = value_str == "old" ? 0 : std::stoi(value_str);
			break;
		}
		case 3:
			test_value_ = std::stoi(line.substr(line.find_last_of(' ') + 1));
			break;
		case 4:
			monkey_if_true_ = std::stoi(line.substr(line.find_last_of(' ') + 1));
			break;
		case 5:
			monkey_if_false_ = std::stoi(line.substr(line.find_last_of(' ') + 1));
			break;
		default:
			throw std::logic_error{std::string{"Unexpected input line: "} + line};
		}
	}

	[[nodiscard]] std::size_t id() const noexcept {
		return id_;
	}

	[[nodiscard]] Item_Type test_value() const noexcept {
		return test_value_;
	}

	[[nodiscard]] bool has_items() const noexcept {
		return !items_.empty();
	}

	[[nodiscard]] std::size_t select_next_monkey(Item_Type item) const noexcept {
		return item % test_value_ == 0 ? monkey_if_true_ : monkey_if_false_;
	}

	void give_item(Item_Type item) noexcept {
		items_.push_back(item);
	}

	[[nodiscard]] Item_Type take_item() noexcept {
		const auto item = items_.front();
		items_.pop_front();
		return adjust_worry_level(item);
	}

private:
	std::size_t id_;
	std::deque<Item_Type> items_;
	Operator operator_;
	Item_Type operation_value_;
	Item_Type test_value_;
	std::size_t monkey_if_true_;
	std::size_t monkey_if_false_;

	[[nodiscard]] Item_Type adjust_worry_level(Item_Type item) const noexcept {
		const auto value = operation_value_ == 0 ? item : operation_value_;
		switch (operator_) {
		case Operator::ADD: return item + value;
		case Operator::MULTIPLY: default: return item * value;
		}
	}
};

template<typename Worry_Manage_FuncT>
Item_Type count_monkey_business(std::vector<Monkey> &monkeys, int rounds, const Worry_Manage_FuncT &manage_worry_level) {
	std::vector<Item_Type> items_inspected(monkeys.size(), 0);
	for (; rounds > 0; --rounds) {
		for (auto &monkey : monkeys) {
			while (monkey.has_items()) {
				++items_inspected[monkey.id()];
				const auto item = manage_worry_level(monkey.take_item());
				monkeys[monkey.select_next_monkey(item)].give_item(item);
			}
		}
	}

	std::nth_element(items_inspected.begin(), std::prev(items_inspected.end(), 2), items_inspected.end());
	return *std::prev(items_inspected.end(), 2) * *std::prev(items_inspected.end(), 1);
}

static Item_Type calc_largest_modulus(const std::vector<Monkey> &monkeys) {
	return std::accumulate(monkeys.begin(), monkeys.end(), Item_Type{1},
						   [](auto sum, const auto &monkey) { return sum * monkey.test_value(); });
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Monkey> monkeys;
	while (has_input(std::cin))
		monkeys.push_back(Monkey::create_from_stream(std::cin));
	switch (part) {
	case 1:
		std::cout << count_monkey_business(monkeys, 20, [](auto item) { return item / 3; }) << std::endl;
		break;
	case 2:
		std::cout << count_monkey_business(monkeys, 10000, [mod = calc_largest_modulus(monkeys)](auto item) { return item % mod; }) << std::endl;
		break;
	}
	return 0;
}
