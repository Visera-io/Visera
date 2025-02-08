module;
#include <Visera.h>
export module Visera.Template.Pattern:Prototype;

export namespace VE { namespace Template
{

	template<typename T>
	class FPrototype
	{
	public:
		virtual T Clone() const = 0;
	};

} } // namespace VE::Template