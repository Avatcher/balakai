#include <string.h>

#include <cstddef>

#include <unicode/ustream.h>

#include "parsing.h"

namespace {
	icu::RegexPattern build_pattern(balakai::parsing::Token::Name const& name, icu::UnicodeString const& pattern) {
		UErrorCode status = U_ZERO_ERROR;
		auto regex = icu::RegexPattern::compile(pattern, 0, status);
		if (U_FAILURE(status)) {
			std::cerr << "FATAL: Couldn't initialise regex pattern for token '" << name << "'\n";
			throw;
		} else {
			return *regex;
		}
	}
}

namespace balakai::parsing {

std::size_t Token::lastId = 0;

Token::Token(Token::Name&& name, icu::UnicodeString const& pattern):
		name(name), regex(build_pattern(name, pattern)), id(Token::lastId++) { }

Token Token::keyword(Token::Name&& name, icu::UnicodeString pattern) {
	return Token(
		std::move(name.insert(0, "KEYWORD_")),
		pattern.insert(0, "\\b").append("\\b")
	);
}

std::size_t Token::Parsed::length() const {
	return groups.at(0).length();
}

void Parser::register_token(Token const& token) {
	tokens.push_back(token);
}

void Parser::register_tokens(std::initializer_list<Token> tokens) {
	for (auto token: tokens) {
		register_token(token);
	}
}

std::vector<Token::Parsed> Parser::parse(std::istream& in, icu::UnicodeString const& sourceName) const {
	UErrorCode status;
	std::vector<icu::RegexMatcher*> matchers;
	for (auto& token: tokens) {
		matchers.push_back(token.regex.matcher(status));
	}
	std::vector<Token::Parsed> parsed;
	CodePosition position { sourceName };
	std::string utf8_line;
	while (std::getline(in, utf8_line)) {
		icu::UnicodeString line = icu::UnicodeString::fromUTF8(icu::StringPiece(utf8_line.c_str()));

		while (position.ch < line.length()) {
			icu::UnicodeString slice = line.tempSubString(position.ch - 1);
			bool tokenRecognised = false;
			for (std::size_t i = 0; i < tokens.size(); i++) {
				auto& token = tokens.at(i);
				auto matcher = matchers.at(i);
				matcher->reset(slice);  // SIGSEGV
				if (matcher->lookingAt(status)) {
					std::vector<icu::UnicodeString> groups { matcher->group(status) };
					for (std::size_t i = 0; i < matcher->groupCount(); i++) {
						groups.push_back(matcher->group(i, status));
					}
					auto result = Token::Parsed {
						token.name,
						std::move(groups)
					};
					parsed.push_back(result);
					position.ch += result.length();
					tokenRecognised = true;
					break;
				}
			}
			if (!tokenRecognised) {
				throw UnexpectedTokenException(position, line);
			}
		}
		position.nextLine();
	}
	return parsed;
}

void CodePosition::nextLine() {
	ln++, ch = 1;
}

std::ostream& operator<<(std::ostream& out, CodePosition const& position) {
	out << position.source << ":" << position.ln << ":" << position.ch;
	return out;
}

const char* ParsingException::what() const throw() {
	return "ParsingException";
}

UnexpectedTokenException::UnexpectedTokenException(CodePosition const& position, icu::UnicodeString const& line):
	position(position), line(line) { }

const char* UnexpectedTokenException::what() const throw() {
	return "UnexpectedTokenException";
}

} // namespace balakai::parsing