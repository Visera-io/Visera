module;
#include <Visera>

export module Visera.Engine.Core.IO.File:Interface;

export import Visera.Engine.Core.Signal;

export namespace VE
{	

	class File
	{
	public:
		virtual void WriteToDisk()	 throw(RuntimeError) = 0;
		virtual void ReadFromDisk()  throw(RuntimeError) = 0;

		virtual void Write(Array<Byte>&& NewData)		{ Data = std::move(NewData); }
		virtual void Write(const Array<Byte>& NewData)	{ Data = NewData; }
		virtual auto Read() const -> const Array<Byte>&	{ return Data; }

		Bool IsEmpty() const { return Data.empty(); }
		auto GetPath() const -> StringView				{ return Path; }
		void SetPath(const String& NewPath)				{ Path = NewPath;}

		File(const String& FilePath) noexcept : Path{ FilePath } {};
		virtual ~File() noexcept = default;
	protected:
		String			Path;
		Array<Byte>		Data;
	};

} // namespace VE