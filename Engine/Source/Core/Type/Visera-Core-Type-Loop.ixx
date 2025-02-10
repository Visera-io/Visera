module;
#include <Visera.h>
export module Visera.Core.Type:Loop;

export namespace VE
{

	template<typename T>
	class TLoop
	{
	public:
		struct FNode
		{
			T Data;
			FNode* Next{ nullptr };
		};

		auto GetHead() const -> const FNode* { return Head; }

		explicit TLoop() noexcept = delete;
		explicit TLoop(std::initializer_list<T> _InitNodes) noexcept;
		~TLoop() noexcept;

	private:
		FNode* Head{ nullptr };
	};

	template<typename T> TLoop<T>::
	TLoop(std::initializer_list<T> _InitNodes) noexcept
	{
		FNode** pCurrentNode = &Head;
		for (auto InitNode : _InitNodes)
		{
			*pCurrentNode = new FNode();
			(*pCurrentNode)->Data = InitNode;
			pCurrentNode = &((*pCurrentNode)->Next);
		}
		*pCurrentNode = Head;
	}

	template<typename T> TLoop<T>::
	~TLoop() noexcept
	{
		if (!Head) { return; }
		FNode* DeletingNode = nullptr;
		FNode* NextNode = Head->Next;
		while(NextNode != Head)
		{
			DeletingNode = NextNode;
			NextNode = DeletingNode->Next;
			delete DeletingNode;
		}
		delete Head;
		Head = nullptr;
	}
	
} // namespace VE