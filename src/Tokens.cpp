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

#include "Tokens.h"
#include "CSSParserAssert.h"
#include <cmath>

namespace css_parser
{
HashTokenValue::HashTokenValue(Vector<CodePoint>&& value, bool isID)
	: m_Value(std::move(value))
	, m_IsID(isID)
{
}

NumberTokenValue::NumberTokenValue(FixedArray<Byte, BYTES_FOR_VALUE>&& value, bool isInteger)
	: m_Value(std::move(value))
	, m_IsInteger(isInteger)
{
}

DimensionTokenValue::DimensionTokenValue(NumberTokenValue&& number, Vector<CodePoint>&& unit)
	: m_Number(std::move(number))
	, m_Unit(std::move(unit))
{
}

Token::Token(TokenType type)
	: m_Type(type)
{
}

Token Token::CreateWhitespace()
{
	return Token(TokenType::Whitespace);
}

Token Token::CreateBadString()
{
	return Token(TokenType::BadString);
}

Token Token::CreateBadURL()
{
	return Token(TokenType::BadURL);
}

Token Token::CreateLeftParenthesis()
{
	return Token(TokenType::LeftParenthesis);
}

Token Token::CreateRightParenthesis()
{
	return Token(TokenType::RightParenthesis);
}

Token Token::CreateLeftSquareBracket()
{
	return Token(TokenType::LeftSquareBracket);
}

Token Token::CreateRightSquareBracket()
{
	return Token(TokenType::RightSquareBracket);
}

Token Token::CreateLeftCurlyBracket()
{
	return Token(TokenType::LeftCurlyBracket);
}

Token Token::CreateRightCurlyBracket()
{
	return Token(TokenType::RightCurlyBracket);
}

Token Token::CreateComma()
{
	return Token(TokenType::Comma);
}

Token Token::CreateColon()
{
	return Token(TokenType::Colon);
}

Token Token::CreateSemiColon()
{
	return Token(TokenType::SemiColon);
}

Token Token::CreateCDC()
{
	return Token(TokenType::CDC);
}

Token Token::CreateCDO()
{
	return Token(TokenType::CDO);
}

Token Token::CreateDelim(const CodePoint& value)
{
	Token result(TokenType::Delim);
	result.m_Value = value;
	return result;
}

Token Token::CreateHash(HashTokenValue&& value)
{
	Token result(TokenType::Hash);
	result.m_Value = std::move(value);
	return result;
}

Token Token::CreateString(Vector<CodePoint>&& value)
{
	Token result(TokenType::String);
	result.m_Value = std::move(value);
	return result;
}

Token Token::CreateNumber(NumberTokenValue&& value)
{
	Token result(TokenType::Number);
	result.m_Value = std::move(value);
	return result;
}

Token Token::CreatePercentage(NumberTokenValue&& value)
{
	Token result(TokenType::Percentage);
	result.m_Value = std::move(value);
	return result;
}

Token Token::CreateDimension(DimensionTokenValue&& value)
{
	Token result(TokenType::Dimension);
	result.m_Value = std::move(value);
	return result;
}

Token Token::CreateFunction(Vector<CodePoint>&& value)
{
	Token result(TokenType::Function);
	result.m_Value = std::move(value);
	return result;
}

Token Token::CreateIdent(Vector<CodePoint>&& value)
{
	Token result(TokenType::Ident);
	result.m_Value = std::move(value);
	return result;
}

Token Token::CreateURL(Vector<CodePoint>&& value)
{
	Token result(TokenType::URL);
	result.m_Value = std::move(value);
	return result;
}

Token Token::CreateAtKeyword(Vector<CodePoint>&& value)
{
	Token result(TokenType::AtKeyword);
	result.m_Value = std::move(value);
	return result;
}

// https://www.w3.org/TR/css-syntax-3/#consume-comment
bool ConsumeComments(const Vector<CodePoint>& inputStream, SizeType& position)
{
	if (position + 1 >= inputStream.size())
	{
		return true;
	}
	// If the next two input code point are U+002F SOLIDUS (/) followed by a U+002A ASTERISK (*)
	if (inputStream[position] != CodePointValue::SOLIDUS || inputStream[position + 1] != CodePointValue::ASTERISK)
	{
		return true;
	}
	bool isPreviousAsterisk = false;
	for (; position < inputStream.size(); ++position)
	{
		const CodePoint& codePoint = inputStream[position];
		if (codePoint == CodePointValue::ASTERISK)
		{
			isPreviousAsterisk = true;
			continue;
		}
		if (codePoint == CodePointValue::SOLIDUS)
		{
			if (isPreviousAsterisk)
			{
				++position;
				return true;
			}
		}
		isPreviousAsterisk = false;
	}
	// If the preceding paragraph ended by consuming an EOF code point, this is a parse error.
	return false;
}


void ConsumeWhitespace(const Vector<CodePoint>& inputStream, SizeType& position, FixedArray<Byte, sizeof(Token)>& output)
{
	for (; position < inputStream.size(); ++position)
	{
		if (!IsWhitespace(inputStream[position]))
		{
			break;
		}
	}
	new (output.data()) Token(Token::CreateWhitespace());
}

unsigned HexDigitToNumber(const CodePoint& codePoint)
{
	if (IsDigit(codePoint))
	{
		return codePoint.GetBytes() - static_cast<unsigned>(CodePointValue::ZERO);
	}
	CSS_PARSER_ASSERT(IsHexDigit(codePoint), "Expects a hex digit");
	if (IsLowercaseLetter(codePoint))
	{
		return 10 + codePoint.GetBytes() - static_cast<unsigned>(CodePointValue::LATIN_SMALL_LETTER_A);
	}
	return 10 + codePoint.GetBytes() - static_cast<unsigned>(CodePointValue::LATIN_CAPITAL_LETTER_A);
}

CodePoint HexSequenceToCodePoint(const Vector<CodePoint>& inputStream, SizeType start, SizeType count)
{
	unsigned resultBytes = 0;
	for (SizeType i = 0; i < count; ++i)
	{
		const CodePoint& codePoint = inputStream[start + i];
		const unsigned asNumber = HexDigitToNumber(codePoint);
		resultBytes = (resultBytes << 4) | asNumber;
	}
	return CodePoint(resultBytes);
}

// https://www.w3.org/TR/css-syntax-3/#consume-an-escaped-code-point
bool ConsumeEscapedCodePoint(const Vector<CodePoint>& inputStream, SizeType& position, FixedArray<Byte, sizeof(CodePoint)>& output)
{
	const SizeType startPosition = position;
	if (position == inputStream.size())
	{
		new (output.data()) CodePoint(CodePointValue::REPLACEMENT);
		return false;
	}
	const CodePoint& codePoint = inputStream[position++];
	if (IsHexDigit(codePoint))
	{
		SizeType consumed = 0;
		for (; consumed < 5; ++consumed)
		{
			if (!IsHexDigit(inputStream[position]))
			{
				break;
			}
			++position;
		}
		if (IsWhitespace(inputStream[position]))
		{
			++position;
		}
		const CodePoint asNumber = HexSequenceToCodePoint(inputStream, startPosition, consumed + 1);
		if (asNumber == CodePointValue::ZERO || IsSurrogate(asNumber) || asNumber > CodePointValue::MAXIMUM_ALLOWED_CODE_POINT)
		{
			new (output.data()) CodePoint(CodePointValue::REPLACEMENT);
		}
		else
		{
			new (output.data()) CodePoint(asNumber);
		}
		return true;
	}
	new (output.data()) CodePoint(codePoint);
	return true;
}

// https://www.w3.org/TR/css-syntax-3/#consume-string-token
bool ConsumeStringToken(const Vector<CodePoint>& inputStream,
	SizeType& position,
	FixedArray<Byte, sizeof(Token)>& output,
	const CodePoint& endingCodePoint)
{
	Vector<CodePoint> value;
	while (position < inputStream.size())
	{
		const CodePoint& nextCodePoint = inputStream[position++];
		if (nextCodePoint == endingCodePoint)
		{
			new (output.data()) Token(Token::CreateString(std::move(value)));
			return true;
		}
		else if (IsNewline(nextCodePoint))
		{
			--position;
			new (output.data()) Token(Token::CreateBadString());
			return false;
		}
		else if (nextCodePoint == CodePointValue::REVERSE_SOLIDUS)
		{
			if (position == inputStream.size())
			{
				// If the next input code point is EOF, do nothing.
				continue;
			}
			const CodePoint& nextCodePoint = inputStream[position];
			if (IsNewline(nextCodePoint))
			{
				// Otherwise, if the next input code point is a newline, consume it.
				++position;
				continue;
			}
			FixedArray<Byte, sizeof(CodePoint)> escapedCodePointRaw;
			if (!ConsumeEscapedCodePoint(inputStream, position, escapedCodePointRaw))
			{
				return false;
			}
			value.push_back(*(reinterpret_cast<CodePoint*>(escapedCodePointRaw.data())));
		}
		else
		{
			value.push_back(nextCodePoint);
		}
	}
	// EOF
	new (output.data()) Token(Token::CreateString(std::move(value)));
	return false;
}

// https://www.w3.org/TR/css-syntax-3/#check-if-two-code-points-are-a-valid-escape
bool AreTwoCodePointsValidEscape(const Vector<CodePoint>& inputStream, SizeType position)
{
	if (position + 1 >= inputStream.size())
	{
		return false;
	}
	if (inputStream[position] != CodePointValue::REVERSE_SOLIDUS)
	{
		return false;
	}
	return !IsNewline(inputStream[position + 1]);
}

// https://www.w3.org/TR/css-syntax-3/#check-if-three-code-points-would-start-an-ident-sequence
bool DoThreeCodePointsStartIndentSequence(const Vector<CodePoint>& inputStream, SizeType position)
{
	if (position + 2 >= inputStream.size())
	{
		return false;
	}
	const CodePoint& firstCodePoint = inputStream[position];
	if (firstCodePoint == CodePointValue::HYPHEN_MINUS)
	{
		return IsIdentStart(inputStream[position + 1])
			|| inputStream[position + 1] == CodePointValue::HYPHEN_MINUS
			|| AreTwoCodePointsValidEscape(inputStream, position + 1);
	}
	else if (firstCodePoint == CodePointValue::REVERSE_SOLIDUS)
	{
		return AreTwoCodePointsValidEscape(inputStream, position);
	}
	return IsIdentStart(firstCodePoint);
}

// https://www.w3.org/TR/css-syntax-3/#consume-an-ident-sequence
bool ConsumeIdentSequence(const Vector<CodePoint>& inputStream, SizeType& position, Vector<CodePoint>& result)
{
	Vector<CodePoint> resultLocal;
	while (position < inputStream.size())
	{
		const CodePoint& nextCodePoint = inputStream[position];
		if (IsIdent(nextCodePoint))
		{
			resultLocal.push_back(nextCodePoint);
			++position;
		}
		else if (AreTwoCodePointsValidEscape(inputStream, position))
		{
			++position;
			FixedArray<Byte, sizeof(CodePoint)> escapedCodePoint;
			if (!ConsumeEscapedCodePoint(inputStream, position, escapedCodePoint))
			{
				return false;
			}
			resultLocal.push_back(*reinterpret_cast<CodePoint*>(escapedCodePoint.data()));

		}
		else
		{
			break;
		}
	}
	result = std::move(resultLocal);
	return true;
}

// https://www.w3.org/TR/css-syntax-3/#starts-with-a-number
bool DoThreeCodePointsStartNumber(const Vector<CodePoint>& inputStream, SizeType position)
{
	if (position >= inputStream.size())
	{
		return false;
	}
	const CodePoint& firstCodePoint = inputStream[position];
	if (firstCodePoint == CodePointValue::PLUS_SIGN || firstCodePoint == CodePointValue::HYPHEN_MINUS)
	{
		if (position + 1 < inputStream.size() && IsDigit(inputStream[position + 1]))
		{
			return true;
		}
		return position + 2 < inputStream.size()
			&& inputStream[position + 1] == CodePointValue::FULL_STOP
			&& IsDigit(inputStream[position + 2]);
	}
	else if (firstCodePoint == CodePointValue::FULL_STOP)
	{
		return position + 1 < inputStream.size() && IsDigit(inputStream[position + 1]);
	}
	return IsDigit(firstCodePoint);
}

// https://www.w3.org/TR/css-syntax-3/#convert-a-string-to-a-number
FixedArray<Byte, NumberTokenValue::BYTES_FOR_VALUE> ConvertStringToNumber(const Vector<CodePoint>& string)
{
	const auto InterpretAsBase10Number = [&string](SizeType& position, unsigned* numDigits = nullptr)
	{
		const SizeType start = position;
		++position;
		while (position < string.size() && IsDigit(string[position]))
		{
			++position;
		}
		if (numDigits)
		{
			*numDigits = static_cast<unsigned>(position - start);
		}
		unsigned result = 0;
		unsigned base = 1;
		for (SizeType i = position; i-- > start;)
		{
			result += (string[i].GetBytes() - static_cast<unsigned>(CodePointValue::ZERO)) * base;
			base *= 10;
		}
		return result;
	};
	int s = 1, t = 1;
	int64_t i = 0;
	unsigned f = 0, d = 0, e = 0;
	SizeType position = 0;
	bool isInteger = true;
	if (string[position] == CodePointValue::PLUS_SIGN)
	{
		++position;
	}
	else if (string[position] == CodePointValue::HYPHEN_MINUS)
	{
		s = -1;
		++position;
	}
	if (IsDigit(string[position]))
	{
		i = InterpretAsBase10Number(position);
	}
	if (position < string.size() && string[position] == CodePointValue::FULL_STOP)
	{
		isInteger = false;
		++position;
	}
	if (position < string.size() && IsDigit(string[position]))
	{
		CSS_PARSER_ASSERT(!isInteger, "Expected full stop before digit when parsing floating number");
		f = InterpretAsBase10Number(position, &d);
	}
	if (position < string.size() &&
		(string[position] == CodePointValue::LATIN_CAPITAL_LETTER_E ||
			string[position] == CodePointValue::LATIN_SMALL_LETTER_E))
	{
		++position;
	}
	if (position < string.size())
	{
		if (string[position] == CodePointValue::PLUS_SIGN)
		{
			++position;
		}
		else if (string[position] == CodePointValue::HYPHEN_MINUS)
		{
			t = -1;
			isInteger = false;
			++position;
		}
	}
	if (position < string.size() && IsDigit(string[position]))
	{
		e = InterpretAsBase10Number(position, &d);
	}
	FixedArray<Byte, NumberTokenValue::BYTES_FOR_VALUE> result;
	if (isInteger)
	{
		CSS_PARSER_ASSERT(f == d == 0 && t == 1, "Bad number conversion");
		int64_t resultNum = s * i;
		int64_t exponentPow = 1;
		for (int64_t j = 0; j < e; ++j)
		{
			exponentPow *= 10;
		}
		resultNum *= exponentPow;
		new (result.data()) int64_t(resultNum);
	}
	else
	{
		new (result.data()) double(s * (i + f * std::pow(10, d)) * std::pow(10, t * e));
	}
	return result;
}

// https://www.w3.org/TR/css-syntax-3/#consume-number
bool ConsumeNumber(const Vector<CodePoint>& inputStream, SizeType& position, FixedArray<Byte, sizeof(NumberTokenValue)>& output)
{
	if (position == inputStream.size())
	{
		return false;
	}
	bool isInteger = true;
	Vector<CodePoint> repr;
	{
		const CodePoint& nextCodePoint = inputStream[position];
		if (nextCodePoint == CodePointValue::PLUS_SIGN || nextCodePoint == CodePointValue::HYPHEN_MINUS)
		{
			++position;
			repr.push_back(nextCodePoint);
		}
	}
	while (position < inputStream.size())
	{
		const CodePoint& nextCodePoint = inputStream[position];
		if (!IsDigit(nextCodePoint))
		{
			break;
		}
		repr.push_back(nextCodePoint);
		++position;
	}
	if (position + 1 < inputStream.size())
	{
		if (inputStream[position] == CodePointValue::FULL_STOP && IsDigit(inputStream[position + 1]))
		{
			repr.push_back(inputStream[position++]);
			repr.push_back(inputStream[position++]);
			isInteger = false;
			while (position < inputStream.size() && IsDigit(inputStream[position]))
			{
				repr.push_back(inputStream[position++]);
			}
		}
	}
	if (position + 1 < inputStream.size())
	{
		if (inputStream[position] == CodePointValue::LATIN_CAPITAL_LETTER_E ||
			inputStream[position] == CodePointValue::LATIN_SMALL_LETTER_E)
		{
			if (inputStream[position + 1] == CodePointValue::PLUS_SIGN ||
				inputStream[position + 1] == CodePointValue::HYPHEN_MINUS)
			{
				if (position + 2 < inputStream.size() && IsDigit(inputStream[position + 2]))
				{
					repr.push_back(inputStream[position++]);
					repr.push_back(inputStream[position++]);
					repr.push_back(inputStream[position++]);
					isInteger = false;
					while (position < inputStream.size() && IsDigit(inputStream[position]))
					{
						repr.push_back(inputStream[position++]);
					}
				}
			}
			else if (IsDigit(inputStream[position + 1]))
			{
				repr.push_back(inputStream[position++]);
				repr.push_back(inputStream[position++]);
				isInteger = false;
				while (position < inputStream.size() && IsDigit(inputStream[position]))
				{
					repr.push_back(inputStream[position++]);
				}
			}
		}
	}
	FixedArray<Byte, NumberTokenValue::BYTES_FOR_VALUE> value = ConvertStringToNumber(repr);
	new (output.data()) NumberTokenValue(std::move(value), isInteger);
	return true;
}

// https://www.w3.org/TR/css-syntax-3/#consume-numeric-token
bool ConsumeNumericToken(const Vector<CodePoint>& inputStream, SizeType& position, FixedArray<Byte, sizeof(Token)>& output)
{
	FixedArray<Byte, sizeof(NumberTokenValue)> number;
	if (!ConsumeNumber(inputStream, position, number))
	{
		return false;
	}
	if (DoThreeCodePointsStartIndentSequence(inputStream, position))
	{
		Vector<CodePoint> unit;
		if (!ConsumeIdentSequence(inputStream, position, unit))
		{
			return false;
		}
		DimensionTokenValue dimension(std::move(*reinterpret_cast<NumberTokenValue*>(number.data())), std::move(unit));
		new (output.data()) Token(Token::CreateDimension(std::move(dimension)));
		return true;
	}
	if (inputStream[position] == CodePointValue::PERCENTAGE_SIGN)
	{
		++position;
		new (output.data()) Token(Token::CreatePercentage(std::move(*reinterpret_cast<NumberTokenValue*>(number.data()))));
		return true;
	}
	new (output.data()) Token(Token::CreateNumber(std::move(*reinterpret_cast<NumberTokenValue*>(number.data()))));
	return true;
}

// https://www.w3.org/TR/css-syntax-3/#consume-the-remnants-of-a-bad-url
bool ConsumeRemnantsOfBadURL(const Vector<CodePoint>& inputStream, SizeType& position)
{
	for (; position < inputStream.size(); ++position)
	{
		if (inputStream[position] == CodePointValue::RIGHT_PARENTHESIS)
		{
			return true;
		}
		else if (AreTwoCodePointsValidEscape(inputStream, position))
		{
			FixedArray<Byte, sizeof(CodePoint)> dummy;
			if (!ConsumeEscapedCodePoint(inputStream, position, dummy))
			{
				return false;
			}
			continue;
		}
	}
	return true;
}

// https://www.w3.org/TR/css-syntax-3/#consume-url-token
bool ConsumeURL(const Vector<CodePoint>& inputStream, SizeType& position, FixedArray<Byte, sizeof(Token)>& output)
{
	Vector<CodePoint> value;
	while (position < inputStream.size() && IsWhitespace(inputStream[position]))
	{
		++position;
	}
	while (position < inputStream.size())
	{
		const CodePoint& next = inputStream[position++];
		if (next == CodePointValue::RIGHT_PARENTHESIS)
		{
			new (output.data()) Token(Token::CreateURL(std::move(value)));
			return true;
		}
		else if (IsWhitespace(next))
		{
			while (position < inputStream.size() && IsWhitespace(inputStream[position]))
			{
				++position;
			}
			if (position == inputStream.size())
			{
				break;
			}
			if (inputStream[position] == CodePointValue::RIGHT_PARENTHESIS)
			{
				++position;
				new (output.data()) Token(Token::CreateURL(std::move(value)));
				return true;
			}
			if (!ConsumeRemnantsOfBadURL(inputStream, position))
			{
				return false;
			}
			new (output.data()) Token(Token::CreateBadURL());
			return true;
		}
		else if (next == CodePointValue::QUOTATION_MARK
			|| next == CodePointValue::APOSTROPHE
			|| next == CodePointValue::LEFT_PARENTHESIS
			|| IsNonPrintable(next))
		{
			if (!ConsumeRemnantsOfBadURL(inputStream, position))
			{
				return false;
			}
			new (output.data()) Token(Token::CreateBadURL());
			return true;
		}
		else if (next == CodePointValue::REVERSE_SOLIDUS)
		{
			if (AreTwoCodePointsValidEscape(inputStream, position - 1))
			{
				FixedArray<Byte, sizeof(CodePoint)> escapedCodePoint;
				if (!ConsumeEscapedCodePoint(inputStream, position, escapedCodePoint))
				{
					return false;
				}
				value.push_back(*(reinterpret_cast<CodePoint*>(escapedCodePoint.data())));
				continue;
			}
			if (!ConsumeRemnantsOfBadURL(inputStream, position))
			{
				return false;
			}
			new (output.data()) Token(Token::CreateBadURL());
			return true;
		}
		else
		{
			value.push_back(next);
		}
	}
	return false;
}

// https://www.w3.org/TR/css-syntax-3/#consume-ident-like-token
bool ConsumeIdentLikeToken(const Vector<CodePoint>& inputStream, SizeType& position, FixedArray<Byte, sizeof(Token)>& output)
{
	Vector<CodePoint> string;
	if (!ConsumeIdentSequence(inputStream, position, string))
	{
		return false;
	}
	if (string.size() == 3 &&
		(string[0] == CodePointValue::LATIN_SMALL_LETTER_U || string[0] == CodePointValue::LATIN_CAPITAL_LETTER_U) &&
		(string[1] == CodePointValue::LATIN_SMALL_LETTER_R || string[1] == CodePointValue::LATIN_CAPITAL_LETTER_R) &&
		(string[2] == CodePointValue::LATIN_SMALL_LETTER_L || string[2] == CodePointValue::LATIN_CAPITAL_LETTER_L) &&
		position < inputStream.size() &&
		inputStream[position] == CodePointValue::LEFT_PARENTHESIS)
	{
		++position;
		while (position + 1 < inputStream.size() && IsWhitespace(inputStream[position]) && IsWhitespace(inputStream[position + 1]))
		{
			++position;
		}
		if (position + 1 < inputStream.size())
		{
			const CodePoint& next = inputStream[position];
			const CodePoint& afterNext = inputStream[position + 1];
			if ((next == CodePointValue::QUOTATION_MARK && afterNext == CodePointValue::APOSTROPHE) ||
				(IsWhitespace(next) && (afterNext == CodePointValue::QUOTATION_MARK || afterNext == CodePointValue::APOSTROPHE)))
			{
				new (output.data()) Token(Token::CreateFunction(std::move(string)));
				return true;
			}
		}
		return ConsumeURL(inputStream, position, output);
	}
	else if (position < inputStream.size() && inputStream[position] == CodePointValue::LEFT_PARENTHESIS)
	{
		new (output.data()) Token(Token::CreateFunction(std::move(string)));
		return true;
	}
	new (output.data()) Token(Token::CreateIdent(std::move(string)));
	return true;
}

bool ConsumeToken(const Vector<CodePoint>& inputStream, SizeType& position, FixedArray<Byte, sizeof(Token)>& output)
{
	if (!ConsumeComments(inputStream, position))
	{
		return false;
	}
	if (position == inputStream.size())
	{
		// EOF
		return true;
	}
	const CodePoint& nextCodePoint = inputStream[position++];
	if (IsWhitespace(nextCodePoint))
	{
		ConsumeWhitespace(inputStream, position, output);
		return true;
	}
	else if (nextCodePoint == CodePointValue::QUOTATION_MARK)
	{
		if (!ConsumeStringToken(inputStream, position, output, nextCodePoint))
		{
			return false;
		}
		return true;
	}
	else if (nextCodePoint == CodePointValue::NUMBER_SIGN)
	{
		{
			const CodePoint& nextCodePoint = inputStream[position];
			if (IsIdent(nextCodePoint) || AreTwoCodePointsValidEscape(inputStream, position))
			{
				const bool isID = DoThreeCodePointsStartIndentSequence(inputStream, position);
				Vector<CodePoint> ident;
				if (!ConsumeIdentSequence(inputStream, position, ident))
				{
					return false;
				}
				new (output.data()) Token(Token::CreateHash(HashTokenValue(std::move(ident), isID)));
				return true;
			}
		}
		new (output.data()) Token(Token::CreateDelim(nextCodePoint));
		return true;
	}
	else if (nextCodePoint == CodePointValue::APOSTROPHE)
	{
		if (!ConsumeStringToken(inputStream, position, output, nextCodePoint))
		{
			return false;
		}
		return true;
	}
	else if (nextCodePoint == CodePointValue::LEFT_PARENTHESIS)
	{
		new (output.data()) Token(Token::CreateLeftParenthesis());
		return true;
	}
	else if (nextCodePoint == CodePointValue::RIGHT_PARENTHESIS)
	{
		new (output.data()) Token(Token::CreateRightParenthesis());
		return true;
	}
	else if (nextCodePoint == CodePointValue::PLUS_SIGN)
	{
		if (DoThreeCodePointsStartNumber(inputStream, position - 1))
		{
			--position;
			if (!ConsumeNumericToken(inputStream, position, output))
			{
				return false;
			}
			return true;
		}
		new (output.data()) Token(Token::CreateDelim(nextCodePoint));
		return true;
	}
	else if (nextCodePoint == CodePointValue::COMMA)
	{
		new (output.data()) Token(Token::CreateComma());
		return true;
	}
	else if (nextCodePoint == CodePointValue::HYPHEN_MINUS)
	{
		if (DoThreeCodePointsStartNumber(inputStream, position - 1))
		{
			--position;
			if (!ConsumeNumericToken(inputStream, position, output))
			{
				return false;
			}
			return true;
		}
		else if (position + 1 < inputStream.size() &&
			inputStream[position] == CodePointValue::HYPHEN_MINUS &&
			inputStream[position + 1] == CodePointValue::GREATER_THAN_SIGN)
		{
			position += 2;
			new (output.data()) Token(Token::CreateCDC());
			return true;
		}
		else if (DoThreeCodePointsStartIndentSequence(inputStream, position - 1))
		{
			--position;
			if (!ConsumeIdentLikeToken(inputStream, position, output))
			{
				return false;
			}
			return true;
		}
		new (output.data()) Token(Token::CreateDelim(nextCodePoint));
		return true;
	}
	else if (nextCodePoint == CodePointValue::FULL_STOP)
	{
		if (DoThreeCodePointsStartNumber(inputStream, position - 1))
		{
			--position;
			if (!ConsumeNumericToken(inputStream, position, output))
			{
				return false;
			}
			return true;
		}
		new (output.data()) Token(Token::CreateDelim(nextCodePoint));
		return true;
	}
	else if (nextCodePoint == CodePointValue::COLON)
	{
		new (output.data()) Token(Token::CreateColon());
		return true;
	}
	else if (nextCodePoint == CodePointValue::SEMI_COLON)
	{
		new (output.data()) Token(Token::CreateSemiColon());
		return true;
	}
	else if (nextCodePoint == CodePointValue::LESS_THAN_SIGN)
	{
		if (position + 2 < inputStream.size() &&
			inputStream[position] == CodePointValue::EXCLAMATION_MARK &&
			inputStream[position + 1] == CodePointValue::HYPHEN_MINUS &&
			inputStream[position + 2] == CodePointValue::HYPHEN_MINUS)
		{
			position += 3;
			new (output.data()) Token(Token::CreateCDO());
			return true;
		}
		new (output.data()) Token(Token::CreateDelim(nextCodePoint));
		return true;
	}
	else if (nextCodePoint == CodePointValue::COMMERCIAL_AT)
	{
		if (DoThreeCodePointsStartIndentSequence(inputStream, position - 1))
		{
			--position;
			Vector<CodePoint> ident;
			if (!ConsumeIdentSequence(inputStream, position, ident))
			{
				return false;
			}
			new (output.data()) Token(Token::CreateAtKeyword(std::move(ident)));
			return true;
		}
		new (output.data()) Token(Token::CreateDelim(nextCodePoint));
		return true;
	}
	else if (nextCodePoint == CodePointValue::LEFT_SQUARE_BRACKET)
	{
		new (output.data()) Token(Token::CreateLeftSquareBracket());
		return true;
	}
	else if (nextCodePoint == CodePointValue::LEFT_CURLY_BRACKET)
	{
		new (output.data()) Token(Token::CreateLeftCurlyBracket());
		return true;
	}
	else if (nextCodePoint == CodePointValue::RIGHT_CURLY_BRACKET)
	{
		new (output.data()) Token(Token::CreateRightCurlyBracket());
		return true;
	}
	else if (IsDigit(nextCodePoint))
	{
		--position;
		if (!ConsumeNumericToken(inputStream, position, output))
		{
			return false;
		}
		return true;
	}
	else if (IsIdentStart(nextCodePoint))
	{
		--position;
		if (!ConsumeIdentLikeToken(inputStream, position, output))
		{
			return false;
		}
		return true;
	}
	else
	{
		new (output.data()) Token(Token::CreateDelim(nextCodePoint));
		return true;
	}
	return true;
}

bool TokenizeCodePoints(const Vector<CodePoint>& inputStream, Vector<Token>& output)
{
	// https://www.w3.org/TR/css-syntax-3/#consume-token
	Vector<Token> outputLocal;
	SizeType position = 0;
	while (position < inputStream.size())
	{
		FixedArray<Byte, sizeof(Token)> token;
		if (!ConsumeToken(inputStream, position, token))
		{
			return false;
		}
		output.push_back(std::move(*reinterpret_cast<Token*>(token.data())));
	}
	return true;

}
}