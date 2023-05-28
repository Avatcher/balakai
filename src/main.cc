#include <iostream>
#include <fstream>

#include <unicode/regex.h>
#include <unicode/ustream.h>
#include <argumentum/argparse.h>

#include "parsing.h"

void execute(const char* fileName);

int main(int argc, char** argv) {
	std::string filePath;

	auto argsParser = argumentum::argument_parser{};
	auto params = argsParser.params();
	argsParser.config().program(argv[0]).description("Balakai Interpreter");
	params.add_parameter(filePath, "FILE").metavar("FILE").help("Source code");

	if (!argsParser.parse_args(argc, argv, 1)) return 1;

	execute(filePath.c_str());
}

void execute(const char* path) {
	using namespace balakai::parsing;

	Parser parser;
	// parser.register_tokens({
	// 	// Keywords
	// 	Token::keyword("IS",       "is"),
	// 	Token::keyword("USE",      "use"),
	// 	Token::keyword("AND",      "and"),
	// 	Token::keyword("VARIABLE", "variable"),
	// 	Token::keyword("EQUAL",    "equal"),
	// 	Token::keyword("OF",       "of"),
	// 	Token::keyword("FUNCTION", "function"),
	// 	Token::keyword("WITH",     "with"),
	// 	Token("SYMBOL", "\\b[a-zA-Z_][a-zA-Z0-9_]*\\b")
	// });
	parser.register_token_groups({
		TokenGroup("KEYWORDS", {
			Token::keyword("IS",       "is"),
			Token::keyword("USE",      "use"),
			Token::keyword("AND",      "and"),
			Token::keyword("VARIABLE", "variable"),
			Token::keyword("EQUAL",    "equal"),
			Token::keyword("OF",       "of"),
			Token::keyword("FUNCTION", "function"),
			Token::keyword("WITH",     "with"),
		}),
		TokenGroup("IGNORABLE", {
			Token("SPACE", " "),
			Token("TAB", "\t"),
			Token("NEWLINE", "\n")
		}),
		TokenGroup("LITERALS", {
			Token("STRING", R"(\".*\")"),
			Token("NUMBER", "-?[0-9]+")
		})
	});
	parser.register_tokens({
		Token("SYMBOL", "\\b[a-zA-Z_][a-zA-Z0-9_]*\\b"),
		Token("DOT", "\\.")
	});

	std::ifstream file;
	file.open(path);
	if (!file) {
		std::cerr << "FATAL: Couldn't open the file '" << path << "'\n";
		std::exit(1);
	}
	try {
		auto tokens = parser.parse(file, path);
		std::cout << "INFO: Parsed " << tokens.size() << " tokens:\n";
		for (auto const& token: tokens) {
			std::cout << " - [" << token.name << "] " << token.groups.at(0) << "\n";
		}
		std::cout << std::flush;
	} catch (balakai::parsing::UnexpectedTokenException const& e) {
		std::cerr << "FATAL: Found an unexpected token\n "
			<< e.position << " | " << e.line << std::endl;
		std::exit(1);
	}
}