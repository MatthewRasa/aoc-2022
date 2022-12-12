#pragma once

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

/* --- Input readers --- */

template<class CRTP>
struct Token_Reader {
	Token_Reader(std::istream &in) {
		std::string line;
		if (!std::getline(in, line))
			throw std::logic_error{"EOF encountered in Token_Reader"};
		std::cout << line << std::endl;
		std::istringstream ss{line};
		for (std::string token; std::getline(ss, token); ++token_num_)
			static_cast<CRTP &>(*this).read_token(token);
		static_cast<CRTP &>(*this).read_end();
	}

	virtual void read_token(const std::string &token) = 0;

	virtual void read_end() { };

	[[nodiscard]] std::size_t token_num() const noexcept {
		return token_num_;
	}

private:
	std::size_t token_num_{0};
};

template<class CRTP>
struct Paragraph_Reader {
	Paragraph_Reader(std::istream &in) {
		for (std::string line; std::getline(in, line) && !line.empty(); ++line_num_)
			static_cast<CRTP &>(*this).read_line(line);
		static_cast<CRTP &>(*this).read_end();
	}

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
