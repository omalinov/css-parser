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

namespace css_parser
{
enum class CodePointValue : unsigned
{
	NULL_CODE_POINT = 0x0000,
	BACKSPACE = 0x0008,
	CHARACTER_TABULATION = 0x0009,
	LINE_FEED = 0x000A,
	LINE_TABULATION = 0x000B,
	FORM_FEED = 0x000C,
	CARRIAGE_RETURN = 0x000D,
	SHIFT_OUT = 0x000E,
	INFORMATION_SEPARATOR_ONE = 0x001F,
	SPACE = 0x0020,
	EXCLAMATION_MARK = 0x0021,
	QUOTATION_MARK = 0x0022,
	NUMBER_SIGN = 0x0023,
	PERCENTAGE_SIGN = 0x0025,
	APOSTROPHE = 0x0027,
	LEFT_PARENTHESIS = 0x0028,
	RIGHT_PARENTHESIS = 0x0029,
	ASTERISK = 0x002A,
	PLUS_SIGN = 0x002B,
	COMMA = 0x002C,
	HYPHEN_MINUS = 0x002D,
	FULL_STOP = 0x002E,
	SOLIDUS = 0x002F,
	ZERO = 0x0030,
	NINE = 0x0039,
	COLON = 0x003A,
	SEMI_COLON = 0x003B,
	LESS_THAN_SIGN = 0x003C,
	GREATER_THAN_SIGN = 0x003E,
	COMMERCIAL_AT = 0x0040,
	LATIN_CAPITAL_LETTER_A = 0x0041,
	LATIN_CAPITAL_LETTER_E = 0x0045,
	LATIN_CAPITAL_LETTER_F = 0x0046,
	LATIN_CAPITAL_LETTER_L = 0x004C,
	LATIN_CAPITAL_LETTER_R = 0x0052,
	LATIN_CAPITAL_LETTER_U = 0x0055,
	LATIN_CAPITAL_LETTER_Z = 0x005A,
	LEFT_SQUARE_BRACKET = 0x005B,
	REVERSE_SOLIDUS = 0x005C,
	RIGHT_SQUARE_BRACKET = 0x005D,
	LOW_LINE = 0x005F,
	LATIN_SMALL_LETTER_A = 0x0061,
	LATIN_SMALL_LETTER_E = 0x0065,
	LATIN_SMALL_LETTER_F = 0x0066,
	LATIN_SMALL_LETTER_L = 0x006C,
	LATIN_SMALL_LETTER_R = 0x0072,
	LATIN_SMALL_LETTER_U = 0x0075,
	LATIN_SMALL_LETTER_Z = 0x007A,
	LEFT_CURLY_BRACKET = 0x007B,
	RIGHT_CURLY_BRACKET = 0x007D,
	DELETE = 0x007F,
	CONTROL = 0x0080,
	REPLACEMENT = 0xFFFD,
	MAXIMUM_ALLOWED_CODE_POINT = 0x10FFFF // https://www.w3.org/TR/css-syntax-3/#maximum-allowed-code-point
};

enum class Encoding
{
	UTF8,
	UTF16BE,
	UTF16LE,
	Count
};

// https://infra.spec.whatwg.org/#code-point
// A code point is a Unicode code point and is represented as "U+" followed by four-to-six ASCII upper hex digits,
// in the range U+0000 to U+10FFFF, inclusive. A code point’s value is its underlying number.
struct CodePoint
{
public:
	explicit CodePoint(CodePointValue value);
	explicit CodePoint(unsigned value);
	unsigned GetBytes() const;
private:
	unsigned m_Value;
};

bool operator==(const CodePoint& lhs, const CodePoint& rhs);
bool operator!=(const CodePoint& lhs, const CodePoint& rhs);
bool operator>=(const CodePoint& lhs, const CodePoint& rhs);
bool operator<=(const CodePoint& lhs, const CodePoint& rhs);
bool operator>(const CodePoint& lhs, const CodePoint& rhs);
bool operator<(const CodePoint& lhs, const CodePoint& rhs);

bool operator==(const CodePoint& lhs, unsigned rhs);
bool operator==(unsigned lhs, const CodePoint& rhs);
bool operator!=(const CodePoint& lhs, unsigned rhs);
bool operator!=(unsigned lhs, const CodePoint& rhs);
bool operator>=(const CodePoint& lhs, unsigned rhs);
bool operator>=(unsigned lhs, const CodePoint& rhs);
bool operator<=(const CodePoint& lhs, unsigned rhs);
bool operator<=(unsigned lhs, const CodePoint& rhs);
bool operator>(const CodePoint& lhs, unsigned rhs);
bool operator>(unsigned lhs, const CodePoint& rhs);
bool operator<(const CodePoint& lhs, unsigned rhs);
bool operator<(unsigned lhs, const CodePoint& rhs);

bool operator==(const CodePoint& lhs, CodePointValue rhs);
bool operator==(CodePointValue lhs, const CodePoint& rhs);
bool operator!=(const CodePoint& lhs, CodePointValue rhs);
bool operator!=(CodePointValue lhs, const CodePoint& rhs);
bool operator>=(const CodePoint& lhs, CodePointValue rhs);
bool operator>=(CodePointValue lhs, const CodePoint& rhs);
bool operator<=(const CodePoint& lhs, CodePointValue rhs);
bool operator<=(CodePointValue lhs, const CodePoint& rhs);
bool operator>(const CodePoint& lhs, CodePointValue rhs);
bool operator>(CodePointValue lhs, const CodePoint& rhs);
bool operator<(const CodePoint& lhs, CodePointValue rhs);
bool operator<(CodePointValue lhs, const CodePoint& rhs);

// A leading surrogate is a code point that is in the range U+D800 to U+DBFF, inclusive.
bool IsLeadingSurrogate(const CodePoint& codePoint);
// A trailing surrogate is a code point that is in the range U+DC00 to U+DFFF, inclusive.
bool IsTrailingSurrogate(const CodePoint& codePoint);
// A surrogate is a leading surrogate or a trailing surrogate.
bool IsSurrogate(const CodePoint& codePoint);
// A scalar value is a code point that is not a surrogate.
bool IsScalarValue(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#newline
bool IsNewline(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#whitespace
bool IsWhitespace(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#digit
bool IsDigit(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#hex-digit
bool IsHexDigit(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#uppercase-letter
bool IsUppercaseLetter(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#lowercase-letter
bool IsLowercaseLetter(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#letter
bool IsLetter(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#non-ascii-code-point
bool IsNonASCII(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#ident-start-code-point
bool IsIdentStart(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#ident-code-point
bool IsIdent(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#non-printable-code-point
bool IsNonPrintable(const CodePoint& codePoint);
// https://www.w3.org/TR/css-syntax-3/#input-byte-stream
// When parsing a stylesheet, the stream of Unicode code points that comprises the input to the
// tokenization stage might be initially seen by the user agent as a stream of bytes
// (typically coming over the network or from the local file system). If so, the user agent must decode these bytes
// into code points according to a particular character encoding.
// https://encoding.spec.whatwg.org/#decode
// Decode stylesheet's stream of bytes with fallback encoding fallback, and return the result.
bool CreateCodePointsStream(const char* text, unsigned size, Vector<CodePoint>& output);
}