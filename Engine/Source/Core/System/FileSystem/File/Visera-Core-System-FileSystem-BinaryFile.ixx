module;
#include <Visera.h>

export module Visera.Core.System.FileSystem:BinaryFile;
import Visera.Core.System.FileSystem.Path;
import :File;

import Visera.Core.Signal;

export namespace VE
{
	
    class FBinaryFile :public FFile
    {
    public:
 	    virtual void SaveAs(const FPath& FilePath,   Int32 SaveModes)	throw(SIOFailure) override;
 	    virtual void LoadFrom(const FPath& FilePath, Int32 LoadModes)	throw(SIOFailure) override;

 	    FBinaryFile(const FPath& FilePath) noexcept : FFile{ FilePath } {};
 	    virtual ~FBinaryFile() noexcept = default;
    };

    void FBinaryFile::
    SaveAs(const FPath& FilePath, Int32 SaveModes) throw(SIOFailure)
    {
 	    SaveModes |= std::ios::binary;
 	    if (auto* OutputFile = OpenOStream(SaveModes))
 	    {
 		    OutputFile->write(reinterpret_cast<char*>(Data.data()), Data.size());

 		    if (OutputFile->fail())
 		    { throw SIOFailure(Text("Failed to write to {}", "FilePath.GetHandle().c_str()")); }

 		    CloseOStream();
 	    }
 	    else throw SIOFailure(Text("Failed to open {}", "FilePath.GetHandle().c_str()"));	
    }

    void FBinaryFile::
    LoadFrom(const FPath& FilePath, Int32 LoadModes) throw(SIOFailure)
    {
 	    LoadModes |= std::ios::binary;
 	    if (auto* InputFile = OpenIStream(LoadModes))
 	    {
 		    Data.resize(GetInputFileSize());

 		    InputFile->read(reinterpret_cast<char*>(Data.data()), Data.size());

 		    if (InputFile->fail() || InputFile->gcount() != Data.size())
 		    { throw SIOFailure(Text("Failed to read from {}", "FilePath.GetHandle().c_str()")); }

 		    CloseIStream();
 	    }
 	    else throw SIOFailure(Text("Failed to open {}", "FilePath.GetHandle().c_str()"));
    }

} // namespace VE