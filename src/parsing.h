#ifndef BALAKAI_PARSING_H_
#define BALAKAI_PARSING_H_

#include <vector>
#include <memory>
#include <iostream>
#include <optional>

#include <unicode/regex.h>

#include "tokens.h"

/**
 * Components related to syntax parsing of the language.
 */
namespace balakai::parsing {

/**
 * A syntax parser
 */
class Parser {
	/**
	 * Registered groups of tokens.
	 */
	std::vector<TokenGroup> tokenGroups;

	/**
	 * Registered tokens.
	 */
	TokenGroup tokens;

public:
	Parser();

	/**
	 * Registers a groups of tokens in the parser.
	 * 
	 * @param group A new group of tokens
	 */
	void register_token_group(TokenGroup const& group);

	/**
	 * Registers multiple groups of tokens in the parser.
	 * 
	 * @param groups New groups of tokens
	 */
	void register_token_groups(std::initializer_list<TokenGroup> groups);

	/**
	 * Registers a new syntax token in the parser.
	 * 
	 * @param token A new token
	 */
	void register_token(Token const& token);

	/**
	 * Registers multiple syntax tokens in the parser.
	 * 
	 * @param token A new token
	 */
	void register_tokens(std::initializer_list<Token> tokens);

	TokenGroup const& group(TokenGroup::Name const& name) const;

	TokenGroup const& token(Token::Name const& name) const;

	/**
	 * Parses an input stream into a list of syntax tokens.
	 * May throw a `ParsingException`.
	 * 
	 * @param in Input stream to parse
	 * @param sourceName The name of the code source
	 * @return Parsed tokens 
	 */
	std::vector<Token::Parsed> parse(std::istream& in, icu::UnicodeString const& sourceName) const;
};

/**
 * A position inside the code.
 */
struct CodePosition {
	/**
	 * The name of the code source.
	 */
	const icu::UnicodeString source;
	/**
	 * A number of the line.
	 */
	std::size_t ln = 1;
	/**
	 * A number of the character in the line.
	 */
	std::size_t ch = 1;

	/**
	 * Changes position to the beginning of the next line.
	 */
	void nextLine();
};

std::ostream& operator<<(std::ostream&, CodePosition const&);

/**
 * An exception related to errors occurred during parsing.
 */
struct ParsingException: public std::exception {
	virtual const char* what() const throw() override;
};

/**
 * An exception related to finding an unregistered
 * in the parser syntax token while parsing.
 */
struct UnexpectedTokenException: public ParsingException {
	const CodePosition position;
	const icu::UnicodeString line;

	UnexpectedTokenException(CodePosition const& position, icu::UnicodeString const& line);

	virtual const char* what() const throw() override;
};

} // namespace balakai::parsing

#endif