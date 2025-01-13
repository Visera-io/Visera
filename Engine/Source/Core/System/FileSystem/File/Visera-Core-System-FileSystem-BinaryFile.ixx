 module;
 #include <Visera.h>

 export module Visera.Core.System.FileSystem:BinaryFile;
 import :File;

 import Visera.Core.Signal;

 export namespace VE
 {
	
 	class FBinaryFile :public FFile
 	{
 	public:
 		virtual void SaveAs(StringView FilePath,   Int32 SaveModes)	throw(SIOFailure) override;
 		virtual void LoadFrom(StringView FilePath, Int32 LoadModes)	throw(SIOFailure) override;

 		FBinaryFile(const String& FilePath) noexcept : FFile{ FilePath } {};
 		virtual ~FBinaryFile() noexcept = default;
 	};

 	void FBinaryFile::
 	SaveAs(StringView FilePath, Int32 SaveModes) throw(SIOFailure)
 	{
 		SaveModes |= std::ios::binary;
 		if (auto* OutputFile = OpenOStream(SaveModes))
 		{
 			OutputFile->write(reinterpret_cast<char*>(Data.data()), Data.size());

 			if (OutputFile->fail())
 			{ throw SIOFailure(Text("Failed to write to {}", FilePath)); }

 			CloseOStream();
 		}
 		else throw SIOFailure(Text("Failed to open {}", FilePath));	
 	}

 	void FBinaryFile::
 	LoadFrom(StringView FilePath, Int32 LoadModes) throw(SIOFailure)
 	{
 		LoadModes |= std::ios::binary;
 		if (auto* InputFile = OpenIStream(LoadModes))
 		{
 			Data.resize(GetInputFileSize());

 			InputFile->read(reinterpret_cast<char*>(Data.data()), Data.size());

 			if (InputFile->fail() || InputFile->gcount() != Data.size())
 			{ throw SIOFailure(Text("Failed to read from {}", FilePath)); }

 			CloseIStream();
 		}
 		else throw SIOFailure(Text("Failed to open {}", FilePath));
 	}

 } // namespace VE