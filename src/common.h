#pragma once

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

/* --- Input readers --- */

bool has_input(std::istream &in) {
	return in.peek() != -1;
}

template<class CRTP>
struct Token_Reader {
	static CRTP create_from_stream(std::istream &in) {
		std::string line;
		if (!std::getline(in, line))
			throw std::logic_error{"EOF encountered in Token_Reader"};
		std::istringstream ss{line};

		CRTP instance{};
		for (std::string token; std::getline(ss, token, ' '); ++instance.token_num_)
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
