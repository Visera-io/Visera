module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.Render.Scene.Primitive:Interface;

import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.Media.Model;

import Visera.Runtime.Render.RHI;

export namespace VE
{

    class IPrimitive
    {
    public:
        using FIndex  = UInt32;

        virtual auto
        GetVertexCount()     -> UInt64        = 0;
        virtual auto
        GetVertexByteSize()  -> UInt64        = 0;
        virtual auto
        GetVerticesData()    -> const Float*  = 0;
        virtual auto
        GetIndexCount()      -> UInt64        = 0;
        virtual auto
        GetIndexByteSize()  -> UInt64         = 0;
        virtual auto
        GetIndicesData()     -> const FIndex* = 0;

        auto inline
        GetCPUVertexBufferSize()-> UInt64 { return GetVertexCount() * GetVertexByteSize(); }
        auto inline
        GetCPUIndexBufferSize() -> UInt64 { return GetIndexCount() * GetIndexByteSize(); }
        auto inline
        GetGPUVertexBufferSize()-> UInt64 { return VBO->GetSize(); }
        auto inline
        GetGPUIndexBufferSize() -> UInt64 { return IBO->GetSize(); }

        auto inline
        GetVertexBufferView() const -> WeakPtr<const RHI::FBuffer> { return VBO; }
        auto inline
        GetIndexBufferView()  const -> WeakPtr<const RHI::FBuffer> { return IBO; }
    
        IPrimitive() = delete;
        IPrimitive(SharedPtr<const FModel> _Model, SharedPtr<RHI::FBuffer> _VBO, SharedPtr<RHI::FBuffer> _IBO);
        virtual ~IPrimitive() = default;

    protected:
        WeakPtr<const FModel>   ModelReference;

        SharedPtr<RHI::FBuffer> VBO;
		SharedPtr<RHI::FBuffer> IBO;
    };

    IPrimitive::
    IPrimitive(SharedPtr<const FModel> _Model,
               SharedPtr<RHI::FBuffer> _VBO,
               SharedPtr<RHI::FBuffer> _IBO)
        :
        ModelReference{ _Model },
        VBO{ std::move(_VBO) },
        IBO{ std::move(_IBO) }
    {
        if (!VBO || !VBO->GetSize() ||
            !Bool(VBO->GetUsages() & RHI::EBufferUsage::Vertex) ||
            !Bool(VBO->GetUsages() & RHI::EBufferUsage::TransferDestination))
        { throw SRuntimeError("Failed to create Primitive! -- Create a correct VBO at the derived class!"); }

        if (IBO && (!IBO->GetSize() ||
            !Bool(IBO->GetUsages() & RHI::EBufferUsage::Index)  ||
            !Bool(IBO->GetUsages() & RHI::EBufferUsage::TransferDestination)))
        { throw SRuntimeError("Failed to create Primitive! -- Create a correct IBO at the derived class!"); }
    }

}// namespace VE