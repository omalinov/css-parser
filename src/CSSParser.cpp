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

#include "CSSParser/CSSParser.h"
#include "CommonTypes.h"
#include "CSSParserAssert.h"
#include <cstdint>

namespace css_parser
{
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
	static CodePoint CreateReplacement()
	{
		return CodePoint(0xFFFD);
	}

	static CodePoint CreateCarriageReturn()
	{
		return CodePoint(0x000D);
	}

	static CodePoint CreateFormFeed()
	{
		return CodePoint(0x000C);
	}

	static CodePoint CreateLineFeed()
	{
		return CodePoint(0x000A);
	}

	static CodePoint CreateNull()
	{
		return CodePoint(0x0000);
	}

	CodePoint(unsigned value)
		: m_Value(value)
	{
	}

	// A leading surrogate is a code point that is in the range U+D800 to U+DBFF, inclusive.
	bool IsLeadingSurrogate() const
	{
		return m_Value >= 0xD800 && m_Value <= 0xDBFF;
	}
	
	// A trailing surrogate is a code point that is in the range U+DC00 to U+DFFF, inclusive.
	bool IsTrailingSurrogate() const
	{
		return m_Value >= 0xDC00 && m_Value <= 0xDFFF;
	}

	// A surrogate is a leading surrogate or a trailing surrogate.
	bool IsSurrogate() const
	{
		return IsLeadingSurrogate() || IsTrailingSurrogate();
	}

	// A scalar value is a code point that is not a surrogate.
	bool IsScalarValue() const
	{
		return !IsSurrogate();
	}

	unsigned GetBytes() const
	{
		return m_Value;
	}
private:
	unsigned m_Value = 0;
};

bool operator==(const CodePoint& lhs, const CodePoint& rhs)
{
	return lhs.GetBytes() == rhs.GetBytes();
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
	CSS_PARSER_ASSERT(!codePoint.IsSurrogate(), "Decoding a UTF8 stream should not generate surrogates.");
	if (codePoint == CodePoint::CreateFormFeed())
	{
		output.push_back(CodePoint::CreateLineFeed());
		isPreviousCarriageReturn = false;
		return;
	}
	if (codePoint == CodePoint::CreateCarriageReturn())
	{
		isPreviousCarriageReturn = true;
		output.push_back(CodePoint::CreateLineFeed());
		return;
	}
	if (codePoint == CodePoint::CreateLineFeed())
	{
		if (!isPreviousCarriageReturn)
		{
			output.push_back(CodePoint::CreateLineFeed());
		}
		isPreviousCarriageReturn = false;
		return;
	}
	if (codePoint == CodePoint::CreateNull())
	{
		output.push_back(CodePoint::CreateReplacement());
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
				PushCodePoint(CodePoint::CreateReplacement(), isPreviousCarriageReturn, output);
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
				PushCodePoint(CodePoint::CreateReplacement(), isPreviousCarriageReturn, output);
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
			PushCodePoint(CodePoint::CreateReplacement(), isPreviousCarriageReturn, output);
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
		PushCodePoint(CodePoint::CreateReplacement(), isPreviousCarriageReturn, output);
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


bool Parse(const char* text, unsigned size)
{
	Vector<CodePoint> inputStream;
	if (!CreateCodePointsStream(text, size, inputStream))
	{
		return false;
	}
	return true;
}
}