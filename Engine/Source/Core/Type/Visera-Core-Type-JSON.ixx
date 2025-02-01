module;
#include <Visera.h>
#include <rapidjson/document.h>
export module Visera.Core.Type:JSON;

import Visera.Core.Signal;
import Visera.Core.System.FileSystem;

export namespace VE
{
	using namespace rapidjson;

	class FJSON
	{
	public:
		using FNode = rapidjson::Value;
		FNode& operator[](StringView _Key) { return Root[_Key.data()]; };

		FJSON() = default;
		FJSON(const FPath& _JsonPath);

	private:
		Document Root;
	};

	FJSON::
	FJSON(const FPath& _JsonPath)
	{
		auto JsonFile = FileSystem::CreateFile(_JsonPath);
		JsonFile->Load();
		if (Root.Parse(JsonFile->GetRawData()).HasParseError())
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
	}

} // namespace VE