module;
#include <Visera.h>

export module Visera.Core.System.FileSystem.File:BinaryFile;
import :BasicFile;

import Visera.Core.System.FileSystem.Path;
import Visera.Core.Signal;

export namespace VE
{
	
    class FBinaryFile :public FBasicFile
    {
    public:
 	    virtual void SaveAs(const FPath& FilePath,   Int32 SaveModes)	throw(SIOFailure) override;
 	    virtual void LoadFrom(const FPath& FilePath, Int32 LoadModes)	throw(SIOFailure) override;

 	    FBinaryFile(const FPath& FilePath) noexcept : FBasicFile{ FilePath } {};
 	    virtual ~FBinaryFile() noexcept = default;

    protected:
        size_t GetBinaryFileSize() const;
    };

    void FBinaryFile::
    SaveAs(const FPath& FilePath, Int32 SaveModes) throw(SIOFailure)
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
    LoadFrom(const FPath& FilePath, Int32 LoadModes) throw(SIOFailure)
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