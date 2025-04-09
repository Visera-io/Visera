module;
#include <Visera.h>
#include <rapidjson/document.h>
export module Visera.Core.Type:JSON;

export namespace VE
{
    using namespace rapidjson;

    class FJSON
    {
    public:
        using FNode = rapidjson::Value;
        const FNode& operator[](StringView _Key) const { return Root[_Key.data()]; };

        FJSON() = default;
        FJSON(Document&& _Document) : Root{ std::move(_Document) } {};

    private:
        Document Root;
    };

} // namespace VE