/*
MIT License

Copyright (c) 2024 omalinov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "CommonTypes.h"
#include "CodePoints.h"

namespace css_parser
{
enum class TokenType
{
	Ident,
	Function,
	AtKeyword,
	Hash,
	String,
	BadString,
	URL,
	BadURL,
	Delim,
	Number,
	Percentage,
	Dimension,
	Whitespace,
	CDO,
	CDC,
	Colon,
	SemiColon,
	Comma,
	LeftSquareBracket,
	RightSquareBracket,
	LeftParenthesis,
	RightParenthesis,
	LeftCurlyBracket,
	RightCurlyBracket
};

class HashTokenValue
{
public:
	// Additionally, hash tokens have a type flag set to either "id" or "unrestricted".
	// The type flag defaults to "unrestricted" if not otherwise set.
	HashTokenValue(Vector<CodePoint>&& value, bool isID = false);
public:
	Vector<CodePoint> m_Value;
	bool m_IsID;
};

class NumberTokenValue
{
public:
	constexpr static unsigned BYTES_FOR_VALUE = 64;
	// <number-token> and <dimension-token> additionally have a type flag set to either "integer" or "number".
	// The type flag defaults to "integer" if not otherwise set. 
	NumberTokenValue(FixedArray<Byte, BYTES_FOR_VALUE>&& value, bool isInteger = true);
private:
	FixedArray<Byte, BYTES_FOR_VALUE> m_Value;
	bool m_IsInteger = true;
};

class DimensionTokenValue
{
public:
	DimensionTokenValue(NumberTokenValue&& number, Vector<CodePoint>&& unit);
private:
	NumberTokenValue m_Number;
	// <dimension-token> additionally have a unit composed of one or more code points.
	Vector<CodePoint> m_Unit;
};

class Token
{
public:
	static Token CreateWhitespace();
	static Token CreateBadString();
	static Token CreateBadURL();
	static Token CreateLeftParenthesis();
	static Token CreateRightParenthesis();
	static Token CreateLeftSquareBracket();
	static Token CreateRightSquareBracket();
	static Token CreateLeftCurlyBracket();
	static Token CreateRightCurlyBracket();
	static Token CreateComma();
	static Token CreateColon();
	static Token CreateSemiColon();
	static Token CreateCDC();
	static Token CreateCDO();
	static Token CreateDelim(const CodePoint& value);
	static Token CreateHash(HashTokenValue&& value);
	static Token CreateString(Vector<CodePoint>&& value);
	static Token CreateNumber(NumberTokenValue&& value);
	static Token CreatePercentage(NumberTokenValue&& value);
	static Token CreateDimension(DimensionTokenValue&& value);
	static Token CreateFunction(Vector<CodePoint>&& value);
	static Token CreateIdent(Vector<CodePoint>&& value);
	static Token CreateURL(Vector<CodePoint>&& value);
	static Token CreateAtKeyword(Vector<CodePoint>&& value);
private:
	Token(TokenType type);

	TokenType m_Type;
	Variant<Vector<CodePoint>, HashTokenValue, CodePoint, NumberTokenValue, DimensionTokenValue> m_Value;
};

// To tokenize a stream of code points into a stream of CSS tokens input,
// repeatedly consume a token from input until an <EOF-token> is reached,
// pushing each of the returned tokens into a stream.
bool TokenizeCodePoints(const Vector<CodePoint>& inputStream, Vector<Token>& output);
}