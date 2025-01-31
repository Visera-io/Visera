module;
#include <Visera.h>
export module Visera.Core.System.FileSystem:File;

import Visera.Core.System.FileSystem.Path;
import Visera.Core.Signal;
import Visera.Core.Log;

export namespace VE
{

    class FFile
    {
    public:
 	    virtual void SaveAs(const FPath& FilePath,   Int32 SaveModes) throw(SIOFailure); //Check out the demo below
 	    virtual void LoadFrom(const FPath& FilePath, Int32 LoadModes) throw(SIOFailure); //Check out the demo below
 	    virtual void Save(Int32 SaveModes = 0x0) throw(SIOFailure) { SaveAs(Path,   SaveModes); }
 	    virtual void Load(Int32 LoadModes = 0x0) throw(SIOFailure) { LoadFrom(Path, LoadModes); }

 	    virtual void WriteAll(Array<Byte>&& NewData) { Data = std::move(NewData); }
 	    virtual void WriteAll(const Array<Byte>& NewData) { Data = NewData; }

 	    void ClearData() { Data.resize(0); }
 	    void ResizeData(UInt64 NewDataSize) { Data.resize(NewDataSize); }

 	    Bool IsEmpty()  const { return Data.empty(); }
 	    Bool IsOpened() const { return IOStream.Address != nullptr; }
 	    auto GetSize() const -> size_t { return Data.size(); }
 	    auto GetPath() const -> const FPath& { return Path; }
 	    auto GetData() const -> const Array<Byte>& { return Data; }

 	    void SetPath(const FPath& NewPath) { Path = NewPath; }

 	    FFile(const FPath& FilePath) noexcept : Path{ FilePath } {};
 	    virtual ~FFile() noexcept = default;

    protected:
 	    auto OpenIStream(Int32 IModes) -> std::ifstream*;
 	    auto GetInputFileSize() const -> size_t;
 	    void CloseIStream();
 	    auto OpenOStream(Int32 OModes) -> std::ofstream*;
 	    void CloseOStream();
    protected:
 	    FPath			Path;
 	    Array<Byte>		Data;
 	    union { void* Address = nullptr; std::ifstream* IStream; std::ofstream* OStream; } IOStream;
    };

    void FFile::
    SaveAs(const FPath& FilePath, Int32 SaveModes)	throw(SIOFailure)
    {
 	    //Tips: Add SaveModes via SaveModes |= NewMode
 	    if (auto* OutputFile = OpenOStream(SaveModes))
 	    {
 		    OutputFile->write(reinterpret_cast<char*>(Data.data()), Data.size());

 		    if (OutputFile->fail())
 		    { throw SIOFailure(Text("Failed to write to {}", "FilePath.GetHandle().c_str()")); }

 		    CloseOStream();
 	    }
 	    else throw SIOFailure(Text("Failed to open {}", "FilePath.GetHandle().c_str()"));	
    }

    void FFile::
    LoadFrom(const FPath& FilePath, Int32 LoadModes) throw(SIOFailure)
    {
 	    //Tips: Add LoadModes via LoadModes |= NewMode
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

    size_t FFile::
    GetInputFileSize() const
    {
 	    VE_ASSERT(IsOpened());
 	    IOStream.IStream->seekg(0, std::ios_base::end);

 	    auto FileSize = static_cast<size_t>(IOStream.IStream->tellg());
 	    if (FileSize >= 4 * OneGByte)
 	    { Log::Warn("Be careful! You are loading a large file ({}GB)", double(FileSize) / OneGByte); }

 	    IOStream.IStream->seekg(0, std::ios_base::beg);

 	    return FileSize;
    }

    std::ifstream* FFile::
    OpenIStream(Int32 IModes)
    { 
 	    if (IOStream.IStream) return nullptr;
 	    IOStream.IStream = new std::ifstream(Path.GetHandle(), IModes);
 	    if (!IOStream.IStream->is_open()) return nullptr;
 	    return IOStream.IStream;
    }

    void FFile::
    CloseIStream()
    {
 	    VE_ASSERT(IOStream.IStream != nullptr);
 	    IOStream.IStream->close();
 	    delete IOStream.IStream;
 	    IOStream.IStream = nullptr;
    }

    std::ofstream* FFile::
    OpenOStream(Int32 OModes)
    {
 	    if (IOStream.OStream) return nullptr;
 	    IOStream.OStream = new std::ofstream(Path.GetHandle(), OModes);
 	    if (!IOStream.OStream->is_open()) return nullptr;
 	    return IOStream.OStream;
    }

    void FFile::
    CloseOStream()
    {
 	    VE_ASSERT(IOStream.OStream != nullptr);
 	    IOStream.OStream->close();
 	    delete IOStream.OStream;
 	    IOStream.OStream = nullptr;
    }

} // namespace VE