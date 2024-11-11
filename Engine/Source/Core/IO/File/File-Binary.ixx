module;
#include <Visera>

export module Visera.Engine.Core.IO.File:Binary;
import :Interface;

import Visera.Engine.Core.Signal;

export namespace VE
{	

	class BinaryFile :public File
	{
	public:
		virtual void SaveAs(StringView FilePath)	 throw(RuntimeError) override;
		virtual void LoadFrom(StringView FilePath)   throw(RuntimeError) override;

		BinaryFile(const String& FilePath) noexcept : File{ FilePath } {};
		virtual ~BinaryFile() noexcept = default;
	};

	void BinaryFile::
	SaveAs(StringView FilePath) throw(RuntimeError)
	{
		if (auto* OutputFile = OpenOStream(std::ios::binary))
		{
			OutputFile->write(reinterpret_cast<char*>(Data.data()), Data.size());

			if (OutputFile->fail())
			{ throw RuntimeError(std::format("Failed to write to {}", FilePath)); }

			CloseOStream();
		}
		else throw RuntimeError(std::format("Failed to open {}", FilePath));	
	}

	void BinaryFile::
	LoadFrom(StringView FilePath) throw(RuntimeError)
	{
		if (auto* InputFile = OpenIStream(std::ios::binary))
		{
			Data.resize(GetInputFileSize());

			InputFile->read(reinterpret_cast<char*>(Data.data()), Data.size());

			if (InputFile->fail() || InputFile->gcount() != Data.size())
			{ throw RuntimeError(std::format("Failed to read from {}", FilePath)); }

			CloseIStream();
		}
		else throw RuntimeError(std::format("Failed to open {}", FilePath));
	}

} // namespace VE