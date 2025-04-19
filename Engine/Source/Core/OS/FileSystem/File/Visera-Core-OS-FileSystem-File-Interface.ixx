module;
#include <Visera.h>
export module Visera.Core.OS.FileSystem.File:Interface;
#define VE_MODULE_NAME "File:Interface"
import Visera.Core.Type;
import Visera.Core.Log;
import Visera.Core.Signal;

export namespace VE
{

    class IFile
    {
    public:
 	    virtual void SaveAs(const FPath& FilePath,   Int32 SaveModes); //Check out the demo below
 	    virtual void LoadFrom(const FPath& FilePath, Int32 LoadModes); //Check out the demo below
 	    virtual void Save(Int32 SaveModes = 0x0) { SaveAs(Path,   SaveModes); }
 	    virtual void Load(Int32 LoadModes = 0x0) { LoadFrom(Path, LoadModes); }

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

 	    IFile(const FPath& FilePath) noexcept : Path{ FilePath } {};
 	    virtual ~IFile() noexcept = default;

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

    void IFile::
    SaveAs(const FPath& FilePath, Int32 SaveModes)
    {
 	    //Tips: Add SaveModes via SaveModes |= NewMode
 	    if (auto* OutputFile = OpenOStream(SaveModes))
 	    {
 		    OutputFile->write(Data.data(), Data.size());

 		    if (OutputFile->fail())
 		    {
 		    	String ErrorInfo = Text("Failed to write to {}! -- throw(SIOFailure)",  Path.ToPlatformString());
 		    	VE_LOG_ERROR("{}", ErrorInfo);
	 		    throw SIOFailure(ErrorInfo);
 		    }

 		    CloseOStream();
 	    }
 	    else
 	    {
 	    	String ErrorInfo = Text("Failed to open {}! -- throw(SIOFailure)",  Path.ToPlatformString());
 	    	VE_LOG_ERROR("{}", ErrorInfo);
 	    	throw SIOFailure(ErrorInfo);
 	    }
    }

    void IFile::
    LoadFrom(const FPath& FilePath, Int32 LoadModes)
    {
 	    //Tips: Add LoadModes via LoadModes |= NewMode
 	    if (auto* InputFile = OpenIStream(LoadModes))
 	    {
            std::stringstream Buffer;
            Buffer << InputFile->rdbuf();
            
 		    CloseIStream();

            Data = std::move(Buffer.str());
 	    }
 	    else
 	    {
 	    	String ErrorInfo = Text("Failed to open {}! -- throw(SIOFailure)", Path.ToPlatformString());
 	    	VE_LOG_ERROR("{}", ErrorInfo);
	 	    throw SIOFailure(ErrorInfo);
 	    }
    }

    std::ifstream* IFile::
    OpenIStream(Int32 IModes)
    { 
 	    if (IOStream.IStream) return nullptr;
 	    IOStream.IStream = new std::ifstream(Path.ToPlatformString().data(), IModes);
 	    if (!IOStream.IStream->is_open()) return nullptr;
 	    return IOStream.IStream;
    }

    void IFile::
    CloseIStream()
    {
 	    VE_ASSERT(IOStream.IStream != nullptr);
 	    IOStream.IStream->close();
 	    delete IOStream.IStream;
 	    IOStream.IStream = nullptr;
    }

    std::ofstream* IFile::
    OpenOStream(Int32 OModes)
    {
 	    if (IOStream.OStream) return nullptr;
 	    IOStream.OStream = new std::ofstream(Path.ToPlatformString().data(), OModes);
 	    if (!IOStream.OStream->is_open()) return nullptr;
 	    return IOStream.OStream;
    }

    void IFile::
    CloseOStream()
    {
 	    VE_ASSERT(IOStream.OStream != nullptr);
 	    IOStream.OStream->close();
 	    delete IOStream.OStream;
 	    IOStream.OStream = nullptr;
    }

} // namespace VE