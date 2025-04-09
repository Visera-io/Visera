module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.Render.Scene.Primitive:Interface;

import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.Media.Model;
import Visera.Core.Math.Basic;

import Visera.Runtime.Render.RHI;

export namespace VE
{

    class IPrimitive
    {
    public:
        virtual auto
        GetVertexData()      const -> const void*   = 0;
        virtual auto
        GetVertexCount()     const -> UInt64        = 0;
        virtual auto
        GetVertexByteSize()  const -> UInt64        = 0;
        virtual auto
        GetIndexData()       const ->  const void*  = 0;
        virtual auto
        GetIndexCount()      const -> UInt64        = 0;
        virtual auto
        GetIndexByteSize()   const -> UInt64        = 0;

        Bool inline
        HasVertex() const { return GetVertexCount() != 0; }
        Bool inline
        HasIndex()  const  { return GetIndexCount() != 0; }

        struct FBoundingBox { Vector3F MaxPosition = Vector3F::Constant(LowerBound<Float>()), MinPosition = Vector3F::Constant(UpperBound<Float>()), Center = Vector3F::Constant(0); };
        auto inline
        GetBoundingBox() const -> const FBoundingBox& { return BoundingBox; }

        auto inline
        GetCPUVertexBufferSize() const -> UInt64 { return GetVertexCount() * GetVertexByteSize(); }
        auto inline
        GetCPUIndexBufferSize()  const -> UInt64 { return GetIndexCount() * GetIndexByteSize(); }
        auto inline
        GetGPUVertexBufferSize() const -> UInt64 { return VBO->GetSize(); }
        auto inline
        GetGPUIndexBufferSize()  const -> UInt64 { return IBO->GetSize(); }

        auto inline
        GetGPUVertexBuffer() const -> SharedPtr<const RHI::FBuffer> { return VBO; }
        auto inline
        GetGPUIndexBuffer()  const -> SharedPtr<const RHI::FBuffer> { return IBO; }
    
        IPrimitive() = delete;
        IPrimitive(SharedPtr<const FModel> _Model, UInt64 _VBOSize, UInt64 _IBOSize);
        virtual ~IPrimitive() = default;

    protected:
        WeakPtr<const FModel>   ModelReference;

        SharedPtr<RHI::FBuffer> VBO;
		SharedPtr<RHI::FBuffer> IBO;

        FBoundingBox BoundingBox;
    };

    IPrimitive::
    IPrimitive(SharedPtr<const FModel> _Model, UInt64 _VBOSize, UInt64 _IBOSize)
        :
        ModelReference{ _Model },
        VBO{ RHI::CreateVertexBuffer(_VBOSize) },
        IBO{ RHI::CreateIndexBuffer(_IBOSize)  }
    {
        if (!VBO || !VBO->GetSize() ||
            !Bool(VBO->GetUsages() & RHI::EBufferUsage::Vertex) ||
            !Bool(VBO->GetUsages() & RHI::EBufferUsage::TransferDestination))
        { throw SRuntimeError("Failed to create Primitive! -- Created an incorrect VBO!"); }

        if (IBO && (!IBO->GetSize() ||
            !Bool(IBO->GetUsages() & RHI::EBufferUsage::Index)  ||
            !Bool(IBO->GetUsages() & RHI::EBufferUsage::TransferDestination)))
        { throw SRuntimeError("Failed to create Primitive! -- Created an incorrect IBO!"); }
    }

}// namespace VE