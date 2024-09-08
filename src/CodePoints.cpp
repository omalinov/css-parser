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

#include "CodePoints.h"
#include "CSSParserAssert.h"

namespace css_parser
{
CodePoint::CodePoint(CodePointValue value)
	: m_Value(static_cast<unsigned>(value))
{
}

CodePoint::CodePoint(unsigned value)
	: m_Value(value)
{
}

unsigned CodePoint::GetBytes() const
{
	return m_Value;
}

bool operator==(const CodePoint& lhs, const CodePoint& rhs)
{
	return lhs.GetBytes() == rhs.GetBytes();
}

bool operator==(const CodePoint& lhs, unsigned rhs)
{
	return lhs.GetBytes() == rhs;
}

bool operator==(unsigned lhs, const CodePoint& rhs)
{
	return lhs == rhs.GetBytes();
}

bool operator!=(const CodePoint& lhs, const CodePoint& rhs)
{
	return lhs.GetBytes() != rhs.GetBytes();
}

bool operator!=(const CodePoint& lhs, unsigned rhs)
{
	return lhs.GetBytes() != rhs;
}

bool operator!=(unsigned lhs, const CodePoint& rhs)
{
	return lhs != rhs.GetBytes();
}

bool operator>=(const CodePoint& lhs, const CodePoint& rhs)
{
	return lhs.GetBytes() >= rhs.GetBytes();
}

bool operator>=(const CodePoint& lhs, unsigned rhs)
{
	return lhs.GetBytes() >= rhs;
}

bool operator>=(unsigned lhs, const CodePoint& rhs)
{
	return lhs >= rhs.GetBytes();
}

bool operator<=(const CodePoint& lhs, const CodePoint& rhs)
{
	return lhs.GetBytes() <= rhs.GetBytes();
}

bool operator<=(const CodePoint& lhs, unsigned rhs)
{
	return lhs.GetBytes() <= rhs;
}

bool operator<=(unsigned lhs, const CodePoint& rhs)
{
	return lhs <= rhs.GetBytes();
}

bool operator>(const CodePoint& lhs, const CodePoint& rhs)
{
	return lhs.GetBytes() > rhs.GetBytes();
}

bool operator>(const CodePoint& lhs, unsigned rhs)
{
	return lhs.GetBytes() > rhs;
}

bool operator>(unsigned lhs, const CodePoint& rhs)
{
	return lhs > rhs.GetBytes();
}

bool operator<(const CodePoint& lhs, const CodePoint& rhs)
{
	return lhs.GetBytes() < rhs.GetBytes();
}

bool operator<(const CodePoint& lhs, unsigned rhs)
{
	return lhs.GetBytes() < rhs;
}

bool operator<(unsigned lhs, const CodePoint& rhs)
{
	return lhs < rhs.GetBytes();
}

bool operator==(const CodePoint& lhs, CodePointValue rhs)
{
	return lhs.GetBytes() == static_cast<unsigned>(rhs);
}

bool operator==(CodePointValue lhs, const CodePoint& rhs)
{
	return static_cast<unsigned>(lhs) == rhs.GetBytes();
}

bool operator!=(const CodePoint& lhs, CodePointValue rhs)
{
	return lhs.GetBytes() != static_cast<unsigned>(rhs);
}

bool operator!=(CodePointValue lhs, const CodePoint& rhs)
{
	return static_cast<unsigned>(lhs) != rhs.GetBytes();
}

bool operator>=(const CodePoint& lhs, CodePointValue rhs)
{
	return lhs.GetBytes() >= static_cast<unsigned>(rhs);
}

bool operator>=(CodePointValue lhs, const CodePoint& rhs)
{
	return static_cast<unsigned>(lhs) >= rhs.GetBytes();
}

bool operator<=(const CodePoint& lhs, CodePointValue rhs)
{
	return lhs.GetBytes() <= static_cast<unsigned>(rhs);
}

bool operator<=(CodePointValue lhs, const CodePoint& rhs)
{
	return static_cast<unsigned>(lhs) <= rhs.GetBytes();
}

bool operator>(const CodePoint& lhs, CodePointValue rhs)
{
	return lhs.GetBytes() > static_cast<unsigned>(rhs);
}

bool operator>(CodePointValue lhs, const CodePoint& rhs)
{
	return static_cast<unsigned>(lhs) > rhs.GetBytes();
}

bool operator<(const CodePoint& lhs, CodePointValue rhs)
{
	return lhs.GetBytes() < static_cast<unsigned>(rhs);
}

bool operator<(CodePointValue lhs, const CodePoint& rhs)
{
	return static_cast<unsigned>(lhs) < rhs.GetBytes();
}

bool IsNewline(const CodePoint& codePoint)
{
	// https://www.w3.org/TR/css-syntax-3/#newline
	return codePoint == CodePointValue::LINE_FEED;
}

bool IsLeadingSurrogate(const CodePoint& codePoint)
{
	return codePoint >= 0xD800 && codePoint <= 0xDBFF;
}

bool IsTrailingSurrogate(const CodePoint& codePoint)
{
	return codePoint >= 0xDC00 && codePoint <= 0xDFFF;
}

bool IsSurrogate(const CodePoint& codePoint)
{
	return IsLeadingSurrogate(codePoint) || IsTrailingSurrogate(codePoint);
}

bool IsScalarValue(const CodePoint& codePoint)
{
	return !IsSurrogate(codePoint);
}

bool IsWhitespace(const CodePoint& codePoint)
{
	return IsNewline(codePoint)
		|| codePoint == CodePointValue::CHARACTER_TABULATION
		|| codePoint == CodePointValue::SPACE;
}

bool IsDigit(const CodePoint& codePoint)
{
	return codePoint >= CodePointValue::ZERO && codePoint <= CodePointValue::NINE;
}

bool IsHexDigit(const CodePoint& codePoint)
{
	return (IsDigit(codePoint))
		|| (codePoint >= CodePointValue::LATIN_CAPITAL_LETTER_A && codePoint <= CodePointValue::LATIN_CAPITAL_LETTER_F)
		|| (codePoint >= CodePointValue::LATIN_SMALL_LETTER_A && codePoint <= CodePointValue::LATIN_SMALL_LETTER_F);
}

bool IsUppercaseLetter(const CodePoint& codePoint)
{
	return codePoint >= CodePointValue::LATIN_CAPITAL_LETTER_A && codePoint <= CodePointValue::LATIN_CAPITAL_LETTER_Z;
}

bool IsLowercaseLetter(const CodePoint& codePoint)
{
	return codePoint >= CodePointValue::LATIN_SMALL_LETTER_A && codePoint <= CodePointValue::LATIN_SMALL_LETTER_Z;
}

bool IsLetter(const CodePoint& codePoint)
{
	return IsUppercaseLetter(codePoint) || IsLowercaseLetter(codePoint);
}

bool IsNonASCII(const CodePoint& codePoint)
{
	return codePoint >= CodePointValue::CONTROL;
}

bool IsIdentStart(const CodePoint& codePoint)
{
	return IsLetter(codePoint) || IsNonASCII(codePoint) || codePoint == CodePointValue::LOW_LINE;
}

bool IsIdent(const CodePoint& codePoint)
{
	return IsIdentStart(codePoint) || IsDigit(codePoint) || codePoint == CodePointValue::HYPHEN_MINUS;
}

bool IsNonPrintable(const CodePoint& codePoint)
{
	return (codePoint >= CodePointValue::NULL_CODE_POINT && codePoint <= CodePointValue::BACKSPACE)
		|| (codePoint == CodePointValue::LINE_TABULATION)
		|| (codePoint >= CodePointValue::SHIFT_OUT && codePoint <= CodePointValue::INFORMATION_SEPARATOR_ONE)
		|| (codePoint == CodePointValue::DELETE);
}

// https://encoding.spec.whatwg.org/#bom-sniff
Encoding BOMSniff(const StringView& ioQueue, SizeType position)
{
	if (position + 3 > ioQueue.size())
	{
		return Encoding::Count;
	}
	const char BOM[3] = { ioQueue[position], ioQueue[position + 1], ioQueue[position + 2] };
	if (BOM[0] == 0xEF)
	{
		if (BOM[1] == 0xBB && BOM[2] == 0xBF)
		{
			return Encoding::UTF8;
		}
	}
	else if (BOM[0] == 0xFE)
	{
		if (BOM[1] == 0xFF)
		{
			return Encoding::UTF16BE;
		}
	}
	else if (BOM[0] == 0xFF)
	{
		if (BOM[1] == 0xFE)
		{
			return Encoding::UTF16LE;
		}
	}
	return Encoding::Count;
}

// Includes the input preprocessing.
// https://www.w3.org/TR/css-syntax-3/#input-preprocessing
void PushCodePoint(const CodePoint& codePoint, bool& isPreviousCarriageReturn, Vector<CodePoint>& output)
{
	CSS_PARSER_ASSERT(!IsSurrogate(codePoint), "Decoding a UTF8 stream should not generate surrogates.");
	if (codePoint == CodePointValue::FORM_FEED)
	{
		output.push_back(CodePoint(CodePointValue::LINE_FEED));
		isPreviousCarriageReturn = false;
		return;
	}
	if (codePoint == CodePointValue::CARRIAGE_RETURN)
	{
		isPreviousCarriageReturn = true;
		output.push_back(CodePoint(CodePointValue::LINE_FEED));
		return;
	}
	if (codePoint == CodePointValue::LINE_FEED)
	{
		if (!isPreviousCarriageReturn)
		{
			output.push_back(CodePoint(CodePointValue::LINE_FEED));
		}
		isPreviousCarriageReturn = false;
		return;
	}
	if (codePoint == CodePointValue::NULL_CODE_POINT)
	{
		output.push_back(CodePoint(CodePointValue::REPLACEMENT));
		isPreviousCarriageReturn = false;
		return;
	}
	output.push_back(codePoint);
}

// https://encoding.spec.whatwg.org/#concept-encoding-run
// https://encoding.spec.whatwg.org/#utf-8-decoder
void UTF8Decode(const StringView& input, SizeType& inputPosition, Vector<CodePoint>& output)
{
	unsigned codePoint = 0;
	unsigned char bytesSeen = 0;
	unsigned char bytesNeeded = 0;
	unsigned char lowerBoundary = 0x80;
	unsigned char upperBoundary = 0xBF;
	bool isPreviousCarriageReturn = false;
	while (inputPosition < input.size())
	{
		unsigned char byte = input[inputPosition];
		if (byte == '\0')
		{
			if (bytesNeeded != 0)
			{
				PushCodePoint(CodePoint(CodePointValue::REPLACEMENT), isPreviousCarriageReturn, output);
				return;
			}
			return;
		}
		if (bytesNeeded == 0)
		{
			if (byte >= 0x00 && byte <= 0x7F)
			{
				PushCodePoint(CodePoint(byte), isPreviousCarriageReturn, output);
			}
			else if (byte >= 0xC2 && byte <= 0xDF)
			{
				bytesNeeded = 1;
				codePoint = byte & 0x1F;
			}
			else if (byte >= 0xE0 && byte <= 0xEF)
			{
				if (byte == 0xE0)
				{
					lowerBoundary = 0xA0;
				}
				else if (byte == 0xED)
				{
					upperBoundary = 0x9F;
				}
				bytesNeeded = 2;
				codePoint = byte & 0xF;
			}
			else if (byte >= 0xF0 && byte <= 0xF4)
			{
				if (byte == 0xF0)
				{
					lowerBoundary = 0x90;
				}
				else if (byte == 0xF4)
				{
					upperBoundary = 0x8F;
				}
				bytesNeeded = 3;
				codePoint = byte & 0x7;
			}
			else
			{
				PushCodePoint(CodePoint(CodePointValue::REPLACEMENT), isPreviousCarriageReturn, output);
			}
			++inputPosition;
			continue;
		}
		if (byte < lowerBoundary || byte > upperBoundary)
		{
			codePoint = 0;
			bytesNeeded = 0;
			bytesSeen = 0;
			lowerBoundary = 0x80;
			upperBoundary = 0xBF;
			PushCodePoint(CodePoint(CodePointValue::REPLACEMENT), isPreviousCarriageReturn, output);
			continue;
		}
		lowerBoundary = 0x80;
		upperBoundary = 0xBF;
		codePoint = codePoint << 6 | (byte & 0x3F);
		++inputPosition;
		if (++bytesSeen != bytesNeeded)
		{
			continue;
		}
		PushCodePoint(CodePoint(codePoint), isPreviousCarriageReturn, output);
		codePoint = 0;
		bytesNeeded = 0;
		bytesSeen = 0;
	}
	if (bytesNeeded != 0)
	{
		PushCodePoint(CodePoint(CodePointValue::REPLACEMENT), isPreviousCarriageReturn, output);
	}
}


// https://www.w3.org/TR/css-syntax-3/#input-byte-stream
// When parsing a stylesheet, the stream of Unicode code points that comprises the input to the
// tokenization stage might be initially seen by the user agent as a stream of bytes
// (typically coming over the network or from the local file system). If so, the user agent must decode these bytes
// into code points according to a particular character encoding.
// https://encoding.spec.whatwg.org/#decode
// Decode stylesheet’s stream of bytes with fallback encoding fallback, and return the result.
bool CreateCodePointsStream(const char* text, unsigned size, Vector<CodePoint>& output)
{
	StringView ioQueue(text, size);
	SizeType ioQueuePosition = 0;
	const Encoding BOMEncoding = BOMSniff(ioQueue, ioQueuePosition);
	if (BOMEncoding != Encoding::Count)
	{
		if (BOMEncoding != Encoding::UTF8)
		{
			CSS_PARSER_ASSERT(false, "Unsupported encoding");
			return false;
		}
		// Read three bytes from ioQueue, if BOMEncoding is UTF-8; otherwise read two bytes. (Do nothing with those bytes.)
		ioQueuePosition += 3;
	}
	output.clear();
	UTF8Decode(ioQueue, ioQueuePosition, output);
	return true;
}
}