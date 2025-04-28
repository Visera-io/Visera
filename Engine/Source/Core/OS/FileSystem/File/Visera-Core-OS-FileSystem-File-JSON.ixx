module;
#include <Visera.h>
#include <rapidjson/document.h>
export module Visera.Core.OS.FileSystem.File:JSONFile;
import :Interface;

import Visera.Core.Type;
import Visera.Core.Signal;

export namespace VE
{
	using namespace rapidjson;

	class FJSONFile : public IFile
	{
	public:
		auto inline
		Parse() -> FJSON;

		FJSONFile(const FPath& FilePath) noexcept : IFile{ FilePath } {};
		virtual ~FJSONFile() noexcept = default;
	};

	FJSON FJSONFile::
	Parse()
	{
		if (IsEmpty()) { Load(); }

		Document Root;
		if (Root.Parse(Data.data()).HasParseError())
		{
			const char* ErrorMessage{};
			switch (Root.GetParseError())
			{
			case kParseErrorDocumentEmpty:
				ErrorMessage = "The document is empty."; break;
			case kParseErrorDocumentRootNotSingular:
				ErrorMessage = "The document root must not follow by other values."; break;
			case kParseErrorValueInvalid:
				ErrorMessage = "Invalid value."; break;
			case kParseErrorObjectMissName:
				ErrorMessage = "Missing a name for object member."; break;
			case kParseErrorObjectMissColon:
				ErrorMessage = "Missing a colon after a name of object member."; break;
			case kParseErrorObjectMissCommaOrCurlyBracket:
				ErrorMessage = "Missing a comma or '}' after an object member."; break;
			case kParseErrorArrayMissCommaOrSquareBracket:
				ErrorMessage = "Missing a comma or ']' after an array element."; break;
			case kParseErrorStringUnicodeEscapeInvalidHex:
				ErrorMessage = "Incorrect hex digit after \\u escape in string."; break;
			case kParseErrorStringUnicodeSurrogateInvalid:
				ErrorMessage = "The surrogate pair in string is invalid."; break;
			case kParseErrorStringEscapeInvalid:
				ErrorMessage = "Invalid escape character in string."; break;
			case kParseErrorStringMissQuotationMark:
				ErrorMessage = "Missing a closing quotation mark in string."; break;
			case kParseErrorStringInvalidEncoding:
				ErrorMessage = "Invalid encoding in string."; break;
			case kParseErrorNumberTooBig:
				ErrorMessage = "Number too big to be stored in double."; break;
			case kParseErrorNumberMissFraction:
				ErrorMessage = "Miss fraction part in number."; break;
			case kParseErrorNumberMissExponent:
				ErrorMessage = "Miss exponent in number."; break;
			case kParseErrorTermination:
				ErrorMessage = "Parsing was terminated."; break;
			case kParseErrorUnspecificSyntaxError:
				ErrorMessage = "Unspecific syntax error."; break;
			default:
				throw SRuntimeError("Unexcepted JSON Parsing Error!");
			}
			throw SRuntimeError(Text("Failed to parse JSON - {}!", ErrorMessage));
		}
		return FJSON{std::move(Root)};
	}

} // namespace VE