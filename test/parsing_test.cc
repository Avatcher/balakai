#include <gtest/gtest.h>
#include <unicode/ustream.h>

#include "parsing.h"

using namespace balakai::parsing;

class TokenTest: public ::testing::Test {
protected:
	Token* dummies;
	Token keyword = Token::keyword("TEST", "test");
	Token::Parsed parsed = Token::Parsed("TEST", { "blabla", "bla" });

	void SetUp() override {
		dummies = new Token[] {
			Token("FIRST", ""),  // id: 1
			Token("SECOND", ""), // id: 2
		};
	}

	void TearDown() override {
		delete[] dummies;
	}
};

TEST_F(TokenTest, Ids) {
	EXPECT_EQ(0, keyword.id);
	EXPECT_EQ(1, dummies[0].id);
	EXPECT_EQ(2, dummies[1].id);
}

TEST_F(TokenTest, Names) {
	EXPECT_EQ(dummies[0].name, "FIRST");
	EXPECT_EQ(dummies[1].name, "SECOND");
	EXPECT_EQ(keyword.name, "KEYWORD_TEST");
}

TEST_F(TokenTest, KeywordMatch) {
	UErrorCode status = U_ZERO_ERROR;
	std::unique_ptr<icu::RegexMatcher> matcher(keyword.matcher(status));
	EXPECT_FALSE(U_FAILURE(status));

	matcher->reset("foo test bar");
	EXPECT_TRUE(matcher->find(status));
	matcher->reset("test()");
	EXPECT_TRUE(matcher->find(status));
	matcher->reset("test:");
	EXPECT_TRUE(matcher->find(status));

	matcher->reset("footest bar");
	EXPECT_FALSE(matcher->find(status));
	matcher->reset("foo testbar");
	EXPECT_FALSE(matcher->find(status));
}

TEST_F(TokenTest, Parsed) {
	EXPECT_EQ(6, parsed.length());
	EXPECT_EQ(2, parsed.groups.size());
	EXPECT_EQ(parsed.name, "TEST");
}

class ParserTest: public ::testing::Test {
protected:
	Parser parser;
	std::stringstream stream;
	std::vector<Token::Parsed> parsed;

	void SetUp() override {
		parser.register_tokens({
			Token::keyword("IF", "if"),
			Token::keyword("IS", "is"),
			Token::keyword("THEN", "then"),
			Token("NUMBER", "\\b[0-9]+\\b"),
			Token("SYMBOL", "\\b([a-zA-Z_])([a-zA-Z0-9_]*)\\b"),
			Token("SPACE", " ")
		});
	}
};

TEST_F(ParserTest, Tokens) {
	stream << "if then";
	parsed = parser.parse(stream, "std::stringstream");
	EXPECT_EQ(3, parsed.size());
	EXPECT_TRUE(parsed.at(0).name == "KEYWORD_IF");
	EXPECT_TRUE(parsed.at(1).name == "SPACE");
	EXPECT_TRUE(parsed.at(2).name == "KEYWORD_THEN");
	stream.clear();

	stream << "iffer";
	parsed = parser.parse(stream, "std::stringstream");
	EXPECT_EQ(1, parsed.size());
	EXPECT_EQ(parsed.at(0).name, "SYMBOL");
	stream.clear();

	stream << "if byte is 8 then megabyte is mega8";
	parsed = parser.parse(stream, "std::stringstream");
	EXPECT_EQ(15, parsed.size());
	EXPECT_EQ(parsed.at(2).name, "SYMBOL");
	EXPECT_EQ(parsed.at(2).groups.at(0), "byte");
	EXPECT_EQ(parsed.at(2).groups.at(1), "b");
	EXPECT_EQ(parsed.at(6).name, "NUMBER");
	EXPECT_EQ(parsed.at(6).groups.at(0), "8");
	EXPECT_EQ(parsed.at(14).name, "SYMBOL");
	EXPECT_EQ(parsed.at(14).groups.at(0), "mega8");
	EXPECT_EQ(parsed.at(14).groups.at(2), "ega8");
	stream.clear();
}

TEST_F(ParserTest, Exceptions) {
	const char* line = "hello // commentary";
	stream << line;
	ASSERT_THROW({
		parsed = parser.parse(stream, "std::stringstream");
	}, UnexpectedTokenException);
	try {
		parsed = parser.parse(stream, "std::stringstream");
	} catch (UnexpectedTokenException const& e) {
		EXPECT_EQ(1, e.position.ln);
		EXPECT_EQ(7, e.position.ch);
		EXPECT_EQ(e.position.source, "std::stringstream");
		EXPECT_EQ(e.line, line);
		throw e;
	}
}