module;
#include <Visera.h>
export module Visera.Core.System.FileSystem.File:BasicFile;

import Visera.Core.System.FileSystem.Path;
import Visera.Core.Signal;
import Visera.Core.Log;

export namespace VE
{

    class FBasicFile
    {
    public:
 	    virtual void SaveAs(const FPath& FilePath,   Int32 SaveModes) throw(SIOFailure); //Check out the demo below
 	    virtual void LoadFrom(const FPath& FilePath, Int32 LoadModes) throw(SIOFailure); //Check out the demo below
 	    virtual void Save(Int32 SaveModes = 0x0) throw(SIOFailure) { SaveAs(Path,   SaveModes); }
 	    virtual void Load(Int32 LoadModes = 0x0) throw(SIOFailure) { LoadFrom(Path, LoadModes); }

        auto Access()             -> void*          { return Data.data(); }
        void Resize(UInt64 _Size)                   { Data.resize(_Size); }
        auto GetData()      const -> StringView     { return Data; }
        auto GetRawData()   const -> const char*    { return Data.data(); }
        auto CopyData()     const -> String         { return Data; }
 	    auto GetSize()      const -> size_t         { return Data.size(); }
 	    auto GetPath()      const -> const FPath&   { return Path; }
        void Clear()                                { Data.clear(); }

 	    Bool IsEmpty()  const { return Data.empty(); }
 	    Bool IsOpened() const { return IOStream.Address != nullptr; }

 	    FBasicFile(const FPath& FilePath) noexcept : Path{ FilePath } {};
 	    virtual ~FBasicFile() noexcept = default;

    protected:
 	    auto OpenIStream(Int32 IModes) -> std::ifstream*;
 	    void CloseIStream();
 	    auto OpenOStream(Int32 OModes) -> std::ofstream*;
 	    void CloseOStream();
    protected:
 	    FPath			Path;
 	    String		    Data;
 	    union { void* Address = nullptr; std::ifstream* IStream; std::ofstream* OStream; } IOStream;
    };

    void FBasicFile::
    SaveAs(const FPath& FilePath, Int32 SaveModes)	throw(SIOFailure)
    {
 	    //Tips: Add SaveModes via SaveModes |= NewMode
 	    if (auto* OutputFile = OpenOStream(SaveModes))
 	    {
 		    OutputFile->write(Data.data(), Data.size());

 		    if (OutputFile->fail())
 		    { throw SIOFailure(Text("Failed to write to {}", "FilePath.GetData().c_str()")); }

 		    CloseOStream();
 	    }
 	    else throw SIOFailure(Text("Failed to open {}", "FilePath.GetData().c_str()"));	
    }

    void FBasicFile::
    LoadFrom(const FPath& FilePath, Int32 LoadModes) throw(SIOFailure)
    {
 	    //Tips: Add LoadModes via LoadModes |= NewMode
 	    if (auto* InputFile = OpenIStream(LoadModes))
 	    {
            std::stringstream Buffer;
            Buffer << InputFile->rdbuf();
            
 		    CloseIStream();

            Data = std::move(Buffer.str());
 	    }
 	    else throw SIOFailure(Text("Failed to open {}", "FilePath.GetData().c_str()"));
    }

    std::ifstream* FBasicFile::
    OpenIStream(Int32 IModes)
    { 
 	    if (IOStream.IStream) return nullptr;
 	    IOStream.IStream = new std::ifstream(Path.GetData(), IModes);
 	    if (!IOStream.IStream->is_open()) return nullptr;
 	    return IOStream.IStream;
    }

    void FBasicFile::
    CloseIStream()
    {
 	    VE_ASSERT(IOStream.IStream != nullptr);
 	    IOStream.IStream->close();
 	    delete IOStream.IStream;
 	    IOStream.IStream = nullptr;
    }

    std::ofstream* FBasicFile::
    OpenOStream(Int32 OModes)
    {
 	    if (IOStream.OStream) return nullptr;
 	    IOStream.OStream = new std::ofstream(Path.GetData(), OModes);
 	    if (!IOStream.OStream->is_open()) return nullptr;
 	    return IOStream.OStream;
    }

    void FBasicFile::
    CloseOStream()
    {
 	    VE_ASSERT(IOStream.OStream != nullptr);
 	    IOStream.OStream->close();
 	    delete IOStream.OStream;
 	    IOStream.OStream = nullptr;
    }

} // namespace VE