#include "common.h"
#include <unordered_map>

static std::size_t find_marker(const std::string &buffer, std::size_t marker_length) {
	std::unordered_map<char, std::size_t> prev_map;
	for (std::size_t pos = 0; pos < marker_length; ++pos)
		++prev_map[buffer[pos]];
	if (prev_map.size() == marker_length)
		return marker_length;

	for (std::size_t pos = marker_length; pos < buffer.size(); ++pos) {
		const auto prev_char = buffer[pos - marker_length];
		if (--prev_map[prev_char] == 0)
			prev_map.erase(prev_char);
		++prev_map[buffer[pos]];
		if (prev_map.size() == marker_length)
			return pos + 1;
	}
	throw std::logic_error{"No marker found"};
}

int main(int argc, char *argv[]) {
	const auto part = select_part(argc, argv);
	const auto line = read_line(std::cin);
	switch (part) {
	case 1:
		std::cout << find_marker(line, 4) << std::endl;
		break;
	case 2:
		std::cout << find_marker(line, 14) << std::endl;
		break;
	}
	return 0;
}
