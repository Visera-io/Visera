module;
#include <Visera.h>
export module Visera.Core.OS.FileSystem.File:BinaryFile;
import :Interface;

import Visera.Core.Type;
import Visera.Core.Signal;

export namespace VE
{
	
    class FBinaryFile :public IFile
    {
    public:
 	    virtual void SaveAs(const FPath& FilePath,   Int32 SaveModes) override;
 	    virtual void LoadFrom(const FPath& FilePath, Int32 LoadModes) override;

 	    FBinaryFile(const FPath& FilePath) noexcept : IFile{ FilePath } {};
 	    virtual ~FBinaryFile() noexcept = default;

    protected:
        size_t GetBinaryFileSize() const;
    };

    void FBinaryFile::
    SaveAs(const FPath& FilePath, Int32 SaveModes)
    {
 	    SaveModes |= std::ios::binary;
 	    if (auto* OutputFile = OpenOStream(SaveModes))
 	    {
 		    OutputFile->write(reinterpret_cast<char*>(Data.data()), Data.size());

 		    if (OutputFile->fail())
 		    { throw SIOFailure(Text("Failed to write to {}", "FilePath.GetData().c_str()")); }

 		    CloseOStream();
 	    }
 	    else throw SIOFailure(Text("Failed to open {}", "FilePath.GetData().c_str()"));	
    }

    void FBinaryFile::
    LoadFrom(const FPath& FilePath, Int32 LoadModes)
    {
 	    LoadModes |= std::ios::binary;
 	    if (auto* InputFile = OpenIStream(LoadModes))
 	    {
 		    Data.resize(GetBinaryFileSize());

 		    InputFile->read(Data.data(), Data.size());

 		    if (InputFile->fail() || InputFile->gcount() != Data.size())
 		    { throw SIOFailure(Text("Failed to read from {}", "FilePath.GetData().c_str()")); }

 		    CloseIStream();
 	    }
 	    else throw SIOFailure(Text("Failed to open {}", "FilePath.GetData().c_str()"));
    }

    size_t FBinaryFile::
    GetBinaryFileSize() const
    {
 	    VE_ASSERT(IsOpened());
 	    IOStream.IStream->seekg(0, std::ios_base::end);
 	    auto FileSize = static_cast<size_t>(IOStream.IStream->tellg());
 	    IOStream.IStream->seekg(0, std::ios_base::beg);

 	    return FileSize;
    }


} // namespace VE