#ifndef BALAKAI_PARSING_H_
#define BALAKAI_PARSING_H_

#include <vector>
#include <memory>
#include <iostream>
#include <optional>

#include <unicode/regex.h>

/**
 * Components related to syntax parsing of the language.
 */
namespace balakai::parsing {

/**
 * A syntax token.
 */
struct Token {
private:
	static std::size_t lastId;
public:
	using Name = icu::UnicodeString;
	/**
	 * A parsed token.
	 */
	class Parsed;
	/**
	 * The name of the token.
	 */
	const Name name;
	/**
	 * The id of the token.
	 */
	const std::size_t id;
	/**
	 * The regular expression of the token.
	 */
	const icu::RegexPattern pattern;

	Token(Name&& name, icu::UnicodeString const& pattern);

	static Token keyword(Name&& name, icu::UnicodeString pattern);

	icu::RegexMatcher* matcher(UErrorCode& status) const;

	struct Parsed {
		const Name name;
		const std::vector<icu::UnicodeString> groups;
		
		std::size_t length() const;
	};
};

/**
 * A syntax parser
 */
class Parser {
	/**
	 * Registred tokens.
	 */
	std::vector< Token> tokens;

public:
	/**
	 * Registers a new syntax token in the parser.
	 * 
	 * @param token A new token.
	 */
	void register_token(Token const& token);

	/**
	 * Registers multiple syntax tokens in the parser.
	 * 
	 * @param token A new token.
	 */
	void register_tokens(std::initializer_list<Token> tokens);

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
 * An exception related to finding an unregistred
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