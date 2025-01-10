module;
#include <Visera.h>

export module Visera.Core.IO:BinaryFile;
import :File;

import Visera.Core.Signal;

export namespace VE
{
	

class BinaryFile :public File
{
public:
	virtual void SaveAs(StringView FilePath,   Int32 SaveModes)	throw(RuntimeError) override;
	virtual void LoadFrom(StringView FilePath, Int32 LoadModes)	throw(RuntimeError) override;

	BinaryFile(const String& FilePath) noexcept : File{ FilePath } {};
	virtual ~BinaryFile() noexcept = default;
};

void BinaryFile::
SaveAs(StringView FilePath, Int32 SaveModes) throw(RuntimeError)
{
	SaveModes |= std::ios::binary;
	if (auto* OutputFile = OpenOStream(SaveModes))
	{
		OutputFile->write(reinterpret_cast<char*>(Data.data()), Data.size());

		if (OutputFile->fail())
		{ throw RuntimeError(Text("Failed to write to {}", FilePath)); }

		CloseOStream();
	}
	else throw RuntimeError(Text("Failed to open {}", FilePath));	
}

void BinaryFile::
LoadFrom(StringView FilePath, Int32 LoadModes) throw(RuntimeError)
{
	LoadModes |= std::ios::binary;
	if (auto* InputFile = OpenIStream(LoadModes))
	{
		Data.resize(GetInputFileSize());

		InputFile->read(reinterpret_cast<char*>(Data.data()), Data.size());

		if (InputFile->fail() || InputFile->gcount() != Data.size())
		{ throw RuntimeError(Text("Failed to read from {}", FilePath)); }

		CloseIStream();
	}
	else throw RuntimeError(Text("Failed to open {}", FilePath));
}

} // namespace VE