#include <string.h>

#include <cstddef>

#include <unicode/ustream.h>

#include "parsing.h"

namespace balakai::parsing {

Parser::Parser(): tokens("ANY") { }

void Parser::register_token(Token const& token) {
	tokens.add_token(token);
	std::cout << "Registered token " << token.name << std::endl;
}

void Parser::register_tokens(std::initializer_list<Token> tokens) {
	for (auto token: tokens) {
		register_token(token);
	}
}

void Parser::register_token_group(TokenGroup const& group) {
	tokenGroups.push_back(group);
	for (Token const& token: group.tokens()) {
		register_token(token);
	}
}

void Parser::register_token_groups(std::initializer_list<TokenGroup> groups) {
	for (TokenGroup const& group: groups) {
		register_token_group(group);
	}
}

std::vector<Token::Parsed> Parser::parse(std::istream& in, icu::UnicodeString const& sourceName) const {
	UErrorCode status;
	std::vector<std::unique_ptr<icu::RegexMatcher>> matchers;
	for (auto& token: tokens.tokens()) {
		matchers.push_back(std::unique_ptr<icu::RegexMatcher>(token.matcher(status)));
	}
	std::vector<Token::Parsed> parsed;
	CodePosition position { sourceName };
	std::string utf8_line;
	while (std::getline(in, utf8_line)) {
		icu::UnicodeString line = icu::UnicodeString::fromUTF8(icu::StringPiece(utf8_line.c_str()));

		while (position.ch < line.length()) {
			icu::UnicodeString slice = line.tempSubString(position.ch - 1);
			bool tokenRecognised = false;
			for (std::size_t i = 0; i < tokens.tokens().size(); i++) {
				auto& token = tokens.tokens().at(i);
				auto& matcher = matchers.at(i);
				matcher->reset(slice);
				if (matcher->lookingAt(status)) {
					std::vector<icu::UnicodeString> groups;
					if (matcher->groupCount() == 0) {
						groups.push_back(matcher->group(status));
					} else {
						for (std::size_t i = 0; i <= matcher->groupCount(); i++) {
							auto group = matcher->group(i, status);
							groups.push_back(group);
						}
					}
					auto result = Token::Parsed {
						token.name,
						groups
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