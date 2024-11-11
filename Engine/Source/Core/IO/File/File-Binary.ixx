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
		virtual void WriteToDisk()	 throw(RuntimeError) override;
		virtual void ReadFromDisk()  throw(RuntimeError) override;

		BinaryFile(const String& FilePath) noexcept : File{ FilePath } {};
		virtual ~BinaryFile() noexcept = default;
	};

	void BinaryFile::
	WriteToDisk() throw(RuntimeError)
	{
		std::ofstream OutputFile(Path, std::ios::binary);
		if (OutputFile.is_open())
		{
			OutputFile.write(reinterpret_cast<char*>(Data.data()), Data.size());

			if (OutputFile.fail())
			{ throw RuntimeError(std::format("Failed to write to {}", Path)); }

			OutputFile.close();
		}
		else throw RuntimeError(std::format("Failed to open {}", Path));	
	}

	void BinaryFile::
	ReadFromDisk() throw(RuntimeError)
	{
		std::ifstream InputFile(Path, std::ios::binary | std::ios::ate);
		if (InputFile.is_open())
		{
			InputFile.seekg(0, std::ios_base::end);
			Data.resize(InputFile.tellg());
			InputFile.seekg(0, std::ios_base::beg);

			InputFile.read(reinterpret_cast<char*>(Data.data()), Data.size());

			if (InputFile.fail() || InputFile.gcount() != Data.size())
			{ throw RuntimeError(std::format("Failed to read from {}", Path)); }

			InputFile.close();
		}
		else throw RuntimeError(std::format("Failed to open {}", Path));
	}

} // namespace VE