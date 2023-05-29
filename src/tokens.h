#ifndef BALAKAI_TOKENS_H_
#define BALAKAI_TOKENS_H_

#include <vector>

#include <unicode/regex.h>

namespace balakai::parsing {

struct Token;

/**
 * A group of tokens sharing some similar properties.
 */
class TokenGroup {
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
class Token {
	static std::size_t lastId;
public:
	using Name = icu::UnicodeString;

	class Parsed;
	class Builder;

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

	static Builder& builder();

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

} // namespace balakai::parsing

#endif