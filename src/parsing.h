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

struct Token;

/**
 * A group of tokens sharing some similar properties.
 */
struct TokenGroup {
	std::vector<Token> _tokens;
public:
	using Name = icu::UnicodeString;
	/**
	 * The name of the group.
	 */
	const Name name;

	TokenGroup(Name const& name);

	TokenGroup(Name const& name, std::initializer_list<Token> const& tokens);

	/**
	 * Adds a token to the group.
	 * 
	 * @param token A new token
	 */
	void add_token(Token const& token);

	/**
	 * Returns all tokens of the group.
	 * 
	 * @return Tokens of the group
	 */
	std::vector<Token> const& tokens() const;
};

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

	/**
	 * Creates a new token for a keyword.
	 * 
	 * @param name A name of the keyword
	 * @param pattern A word
	 * @return Keyword token
	 */
	static Token keyword(Name&& name, icu::UnicodeString pattern);

	/**
	 * Adds token to a specific group
	 * 
	 * @param group 
	 * @return Token&& 
	 */
	Token in_group(TokenGroup& group);

	/**
	 * Returns a regex matcher for this token.
	 * 
	 * @param status UErrorCode
	 * @return A pointer to a matcher
	 */
	icu::RegexMatcher* matcher(UErrorCode& status) const;

	/**
	 * A found token in the code with
	 * corresponding strings.
	 */
	struct Parsed {
		/**
		 * The name of the original token.
		 */
		const Name name;
		/**
		 * Matched string groups.
		 */
		const std::vector<icu::UnicodeString> groups;
		
		/**
		 * Returns the length of the full matched string.
		 * 
		 * @return The length of the string
		 */
		std::size_t length() const;
	};
};


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
	std::vector<Token> tokens;

public:
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