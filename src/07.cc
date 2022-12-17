#include "common.h"
#include <limits>
#include <memory>
#include <unordered_map>
#include <vector>

enum class Type { COMMAND, FILE, DIR };
enum class Command_Type { CD, LS };

static constexpr uint TOTAL_SPACE{70000000};
static constexpr uint FREE_SPACE_TARGET{30000000};

struct Terminal_Line : Token_Reader<Terminal_Line> {	

	void read_token(const std::string &token) final {
		switch (token_num()) {
		case 0:
			if (token == "$") {
				type_ = Type::COMMAND;
			} else if (token == "dir") {
				type_ = Type::DIR;
				file_size_ = 0;
			} else {
				type_ = Type::FILE;
				file_size_ = std::stoi(token);
			}
			break;
		case 1:
			switch (type_) {
			case Type::COMMAND:
				command_type_ = token == "cd" ? Command_Type::CD : Command_Type::LS;
				break;
			case Type::FILE:
			case Type::DIR:
				file_name_ = token;
				break;
			}
			break;
		case 2:
			if (type_ == Type::COMMAND && command_type_ == Command_Type::CD)
				file_name_ = token;
			break;
		}
	}

	[[nodiscard]] Type type() const noexcept {
		return type_;
	}

	[[nodiscard]] Command_Type command_type() const noexcept {
		return command_type_;
	}

	[[nodiscard]] const std::string &file_name() const noexcept {
		return file_name_;
	}

	[[nodiscard]] uint file_size() const noexcept {
		return file_size_;
	}

private:
	Type type_;
	Command_Type command_type_;
	std::string file_name_;
	uint file_size_;
};

struct Node {

	Node() : Node{"/", *this, true} { }

	Node(std::string name, Node &parent, bool is_dir, uint size = 0)
			: name_{std::move(name)},
			  parent_{&parent},
			  is_dir_{is_dir},
			  size_{size} {
	}

	[[nodiscard]] bool is_dir() const noexcept {
		return is_dir_;
	}

	[[nodiscard]] const Node &parent() const noexcept {
		return *parent_;
	}

	[[nodiscard]] Node &parent() noexcept {
		return *parent_;
	}

	Node &emplace_child_dir(const std::string &name) {
		return emplace_child(name, true, 0);
	}

	Node &emplace_child_file(const std::string &name, uint size) {
		return emplace_child(name, false, size);
	}	

	uint size() const {
		if (is_dir() && size_ == 0) {
			for (const auto &[name, node_ptr] : children_)
				size_ += node_ptr->size();
		}
		return size_;
	}

	template<class Func>
	void visit(const Func &func) const {
		func(*this);
		for (auto &[name, node_ptr] : children_)
			node_ptr->visit(func);
	}

private:
	std::string name_;
	Node *parent_;
	bool is_dir_;
	mutable uint size_;
	std::unordered_map<std::string, std::unique_ptr<Node>> children_;

	Node &emplace_child(const std::string &name, bool is_dir, uint size) {
		auto it = children_.find(name);
		if (it == children_.end())
			it = children_.emplace(name, std::make_unique<Node>(name, *this, is_dir, size)).first;
		return *it->second;
	}
};

std::unique_ptr<Node> create_tree(const std::vector<Terminal_Line> &terminal_lines) {
	auto root = std::make_unique<Node>();
	auto currentPtr = root.get();
	for (const auto &line : terminal_lines) {
		switch (line.type()) {
		case Type::COMMAND:
			switch (line.command_type()) {
			case Command_Type::CD:
				if (line.file_name() == "/")
					currentPtr = root.get();
				else if (line.file_name() == "..")
					currentPtr = &currentPtr->parent();
				else
					currentPtr = &currentPtr->emplace_child_dir(line.file_name());
				break;
			case Command_Type::LS:
				break;
			}
			break;
		case Type::FILE:
			currentPtr->emplace_child_file(line.file_name(), line.file_size());
			break;
		case Type::DIR:
			currentPtr->emplace_child_dir(line.file_name());
			break;
		}
	}
	return root;
}

static uint sum_large_dirs(const Node &root) {
	uint sum{0};
	root.visit([&sum](const Node &node) {
		if (node.is_dir() && node.size() <= 100000)
			sum += node.size();
	});
	return sum;
}

static uint deletion_dir_size(const Node &root) {
	const auto free_space = TOTAL_SPACE - root.size();
	auto smallest_size{std::numeric_limits<uint>::max()};
	root.visit([free_space, &smallest_size](const Node &node) {
		if (free_space + node.size() >= FREE_SPACE_TARGET && node.size() < smallest_size)
			smallest_size = node.size();
	});
	return smallest_size;
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	std::vector<Terminal_Line> terminal_lines;
	while (has_input(std::cin))
		terminal_lines.push_back(Terminal_Line::create_from_stream(std::cin));
	const auto root_ptr = create_tree(terminal_lines);
	switch (part) {
	case 1:
		std::cout << sum_large_dirs(*root_ptr) << std::endl;
		break;
	case 2:
		std::cout << deletion_dir_size(*root_ptr) << std::endl;
		break;
	}
	return 0;
}
