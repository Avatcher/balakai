#include <iostream>

#include <unicode/ustream.h>

#include "tokens.h"

namespace {
	icu::RegexPattern build_pattern(balakai::parsing::Token::Name const& name, icu::UnicodeString const& pattern) {
		UErrorCode status = U_ZERO_ERROR;
		auto regex = icu::RegexPattern::compile(pattern, 0, status);
		if (U_FAILURE(status)) {
			std::cerr << "FATAL: Failed to initialise regex pattern for token '" << name << "'\n";
			throw;
		} else {
			return *regex;
		}
	}
}

namespace balakai::parsing {

TokenGroup::TokenGroup(TokenGroup::Name const& name): name(name) { }

TokenGroup::TokenGroup(TokenGroup::Name const& name, std::initializer_list<Token> const& tokens):
		name(name) {
	for (Token const& token: tokens) {
		this->_tokens.push_back(token);
	}
}

void TokenGroup::add_token(Token const& token) {
	_tokens.push_back(token);
}

std::vector<Token> const& TokenGroup::tokens() const {
	return _tokens;
}

std::size_t Token::lastId = 0;

Token::Token(Token::Name&& name, icu::UnicodeString const& pattern):
		name(name), pattern(build_pattern(name, pattern)), id(Token::lastId++) { }

Token Token::keyword(Token::Name&& name, icu::UnicodeString pattern) {
	return Token(
		std::move(name.insert(0, "KEYWORD_")),
		pattern.insert(0, "\\b").append("\\b")
	);
}

Token Token::in_group(TokenGroup& group) {
	group.add_token(*this);
	return *this;
}

icu::RegexMatcher* Token::matcher(UErrorCode& status) const {
	status = U_ZERO_ERROR;
	icu::RegexMatcher* matcher = pattern.matcher(status);
	if (U_FAILURE(status)) {
		std::cerr << "FATAL: Failed to create a regex matcher for token '" << name << "'\n"
			<< "pattern: " << pattern.pattern() << "\n"
			<< u_errorName(status) << std::endl;
		throw;
	} else {
		return matcher;
	}
}

std::size_t Token::Parsed::length() const {
	return groups.at(0).length();
}

} // namespace balakai::parsing