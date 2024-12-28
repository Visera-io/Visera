module;
#include <Visera.h>

export module Visera.Core.IO.File:Interface;

export import Visera.Core.Signal;

import Visera.Core.Log;

export namespace VE
{	

	class File
	{
	public:
		static inline Bool IfExists(StringView FilePath) { return std::filesystem::exists(FilePath); }
		static inline void CreateIfNotExists(StringView FilePath) throw(RuntimeError);

		virtual void SaveAs(StringView FilePath,   Int32 SaveModes)	throw(RuntimeError) = 0; //Check out the demo below
		virtual void LoadFrom(StringView FilePath, Int32 LoadModes)	throw(RuntimeError) = 0; //Check out the demo below
		virtual void Save(Int32 SaveModes = 0x0) throw(RuntimeError) { SaveAs(Path,   SaveModes); }
		virtual void Load(Int32 LoadModes = 0x0) throw(RuntimeError) { LoadFrom(Path, LoadModes); }

		virtual void WriteAll(Array<Byte>&& NewData) { Data = std::move(NewData); }
		virtual void WriteAll(const Array<Byte>& NewData) { Data = NewData; }

		void ClearData() { Data.resize(0); }
		void ResizeData(UInt64 NewDataSize) { Data.resize(NewDataSize); }

		Bool IsEmpty()  const { return Data.empty(); }
		Bool IsOpened() const { return IOStream.Address != nullptr; }
		auto GetSize() const -> size_t { return Data.size(); }
		auto GetPath() const -> StringView { return Path; }
		auto GetData() const -> const Array<Byte>& { return Data; }

		void SetPath(const String& NewPath) { Path = NewPath; }

		File(const String& FilePath) noexcept : Path{ FilePath } {};
		virtual ~File() noexcept = default;

	protected:
		auto OpenIStream(Int32 IModes) -> std::ifstream*;
		auto GetInputFileSize() const -> size_t;
		void CloseIStream();
		auto OpenOStream(Int32 OModes) -> std::ofstream*;
		void CloseOStream();
	protected:
		String			Path;
		Array<Byte>		Data;
		union { void* Address = nullptr; std::ifstream* IStream; std::ofstream* OStream; } IOStream;
	};

	void File::
	SaveAs(StringView FilePath, Int32 SaveModes)	throw(RuntimeError)
	{
		//Tips: Add SaveModes via SaveModes |= NewMode
		if (auto* OutputFile = OpenOStream(SaveModes))
		{
			OutputFile->write(reinterpret_cast<char*>(Data.data()), Data.size());

			if (OutputFile->fail())
			{ throw RuntimeError(std::format("Failed to write to {}", FilePath)); }

			CloseOStream();
		}
		else throw RuntimeError(std::format("Failed to open {}", FilePath));	
	}

	void File::
	LoadFrom(StringView FilePath, Int32 LoadModes) throw(RuntimeError)
	{
		//Tips: Add LoadModes via LoadModes |= NewMode
		if (auto* InputFile = OpenIStream(LoadModes))
		{
			Data.resize(GetInputFileSize());

			InputFile->read(reinterpret_cast<char*>(Data.data()), Data.size());

			if (InputFile->fail() || InputFile->gcount() != Data.size())
			{ throw RuntimeError(std::format("Failed to read from {}", FilePath)); }

			CloseIStream();
		}
		else throw RuntimeError(std::format("Failed to open {}", FilePath));
	}

	void File::
	CreateIfNotExists(StringView FilePath) throw(RuntimeError)
	{
		if (IfExists(FilePath) == False)
		{
			Log::Debug("About to create a new file at {}...", FilePath);
			std::ofstream NewFile(FilePath.data());
			if (NewFile.is_open()) NewFile.close();
			else throw RuntimeError(std::format("Failed to create a new file at {}", FilePath));
		}
	}

	size_t File::GetInputFileSize() const
	{
		VE_ASSERT(IsOpened());
		IOStream.IStream->seekg(0, std::ios_base::end);

		auto FileSize = static_cast<size_t>(IOStream.IStream->tellg());
		if (FileSize >= 4 * OneGByte)
		{ Log::Warn("Be careful! You are loading a large file ({}GB)", double(FileSize) / OneGByte); }

		IOStream.IStream->seekg(0, std::ios_base::beg);

		return FileSize;
	}

	std::ifstream* File::
	OpenIStream(Int32 IModes)
	{ 
		if (IOStream.IStream) return nullptr;
		IOStream.IStream = new std::ifstream(Path, IModes);
		if (!IOStream.IStream->is_open()) return nullptr;
		return IOStream.IStream;
	}

	void File::
	CloseIStream()
	{
		VE_ASSERT(IOStream.IStream != nullptr);
		IOStream.IStream->close();
		delete IOStream.IStream;
		IOStream.IStream = nullptr;
	}

	std::ofstream* File::
	OpenOStream(Int32 OModes)
	{
		if (IOStream.OStream) return nullptr;
		IOStream.OStream = new std::ofstream(Path, OModes);
		if (!IOStream.OStream->is_open()) return nullptr;
		return IOStream.OStream;
	}

	void File::
	CloseOStream()
	{
		VE_ASSERT(IOStream.OStream != nullptr);
		IOStream.OStream->close();
		delete IOStream.OStream;
		IOStream.OStream = nullptr;
	}

} // namespace VE