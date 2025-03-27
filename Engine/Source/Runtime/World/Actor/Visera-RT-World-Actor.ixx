module;
#include <Visera.h>
export module Visera.Runtime.World.Actor;
import Visera.Runtime.World.Object;

export namespace VE
{

    class AActor : public VObject
    {
    public:


        virtual void Create()	override;
        virtual void Destroy()	override;

    private:

    };

    template<typename T>
    concept AActorType = std::is_class_v<AActor>;

    void AActor::Create()
    {
        AttachTransformComponent();
        AttachMeshComponent();
    }

    void AActor::Destroy()
    {

    }

} // namespace VE