#include "common.h"
#include <memory>
#include <optional>
#include <unordered_map>

struct Node;

using Node_Tree = std::unordered_map<std::string, std::unique_ptr<Node>>;

struct Node : Token_Reader<Node> {
	enum class Operation { PLUS, MINUS, MULT, DIV };

	void read_token(const std::string &token) final {
		switch (token_num()) {
		case 0:
			name_ = token.substr(0, token.size() - 1);
			break;
		case 1:
			try {
				val_ = std::stoi(token);
			} catch (const std::invalid_argument &) {
				left_name_ = token;
			}
			break;
		case 2:
			op_ = read_op(token);
			break;
		case 3:
			right_name_ = token;
			break;
		}
	}

	[[nodiscard]] const std::string &name() const noexcept {
		return name_;
	}

	[[nodiscard]] long value() const noexcept {
		if (!val_) {
			val_ = perform_op(left_->value(), right_->value());
		}
		return *val_;
	};
	
	[[nodiscard]] long make_equal(const std::string &name) const {
		if (!left_ || !right_)
			throw std::logic_error{"Can only call make_equal() on non-leaf node"};
		return left_->contains(name) ? left_->make_value(name, right_->value()) : right_->make_value(name, left_->value());
	}

	void link_children(const Node_Tree &nodes) {
		if (!left_name_.empty()) {
			left_ = nodes.at(left_name_).get();
			right_ = nodes.at(right_name_).get();
		}
	}

private:

	[[nodiscard]] static Operation read_op(const std::string &token) {
		switch (token.front()) {
		case '+': return Operation::PLUS;
		case '-': return Operation::MINUS;
		case '*': return Operation::MULT;
		case '/': return Operation::DIV;
		default:
			throw std::invalid_argument{token};
		}
	}
	
	std::string name_, left_name_, right_name_;
	mutable std::optional<long> val_;
	Node *left_{nullptr}, *right_{nullptr};
	Operation op_;

	[[nodiscard]] long perform_op(long lhs, long rhs) const noexcept {
		switch (op_) {
		case Operation::PLUS: return lhs + rhs;
		case Operation::MINUS: return lhs - rhs;
		case Operation::MULT: return lhs * rhs;
		case Operation::DIV: default: return lhs / rhs;
		}
	}

	[[nodiscard]] long reverse_lhs_op(long result, long lhs) const noexcept {
		switch (op_) {
		case Operation::PLUS: return result - lhs;
		case Operation::MINUS: return lhs - result;
		case Operation::MULT: return result / lhs;
		case Operation::DIV: default: return lhs / result;
		}
	}

	[[nodiscard]] long reverse_rhs_op(long result, long rhs) const noexcept {
		switch (op_) {
		case Operation::PLUS: return result - rhs;
		case Operation::MINUS: return result + rhs;
		case Operation::MULT: return result / rhs;
		case Operation::DIV: default: return result * rhs;
		}
	}

	[[nodiscard]] long make_value(const std::string &name, long value) const noexcept {
		return name_ == name ? value
			 : left_->contains(name) ? left_->make_value(name, reverse_rhs_op(value, right_->value()))
			 : right_->make_value(name, reverse_lhs_op(value, left_->value()));
	}

	[[nodiscard]] bool contains(const std::string &name) const noexcept {
		return name_ == name || (left_ && left_->contains(name)) || (right_ && right_->contains(name));
	}
};

[[nodiscard]] static Node_Tree read_tree(std::istream &in) {
	Node_Tree nodes;
	while (has_input(in)) {
		auto node_ptr = std::make_unique<Node>(Node::create_from_stream(std::cin));
		auto name = node_ptr->name();
		nodes.emplace(std::move(name), std::move(node_ptr));
	}
	for (auto &[name, node_ptr] : nodes)
		node_ptr->link_children(nodes);
	return nodes;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	auto nodes = read_tree(std::cin);
	switch (part) {
	case 1:
		std::cout << nodes["root"]->value() << std::endl;
		break;
	case 2:
		std::cout << nodes["root"]->make_equal("humn") << std::endl;
		break;
	}
	return 0;
}
