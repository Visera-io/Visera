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
			FNode() = default;
			FNode(const T& _Data) : Data{ _Data } {};
			FNode(T&& _Data) : Data{ _Data } {};
		};

		auto Emplace(T&& _Data)				-> FNode*		{ FNode* NextNode = CurrentNode->Next; CurrentNode->Next = new FNode(_Data); CurrentNode->Next->Next = NextNode; return CurrentNode->Next; }
		auto Insert(const T& _Data)			-> TLoop&		{ FNode* NextNode = CurrentNode->Next; CurrentNode->Next = new FNode(_Data); CurrentNode->Next->Next = NextNode; return *this; }
		auto Peek()					const	-> const FNode* { return CurrentNode; }
		auto Advance()						-> FNode*	{ FNode* Prev = CurrentNode; CurrentNode = CurrentNode->Next; return Prev; }

		explicit TLoop() noexcept = delete;
		explicit TLoop(std::initializer_list<T> _InitNodes) noexcept;
		~TLoop() noexcept;

	private:
		FNode* CurrentNode{ nullptr };
	};

	template<typename T> TLoop<T>::
	TLoop(std::initializer_list<T> _InitNodes) noexcept
	{
		FNode** pCurrentNode = &CurrentNode;
		for (auto InitNode : _InitNodes)
		{
			*pCurrentNode = new FNode(std::move(InitNode));
			pCurrentNode = &((*pCurrentNode)->Next);
		}
		*pCurrentNode = CurrentNode;
	}

	template<typename T> TLoop<T>::
	~TLoop() noexcept
	{
		if (!CurrentNode) { return; }
		FNode* DeletingNode = nullptr;
		FNode* NextNode = CurrentNode->Next;
		while(NextNode != CurrentNode)
		{
			DeletingNode = NextNode;
			NextNode = DeletingNode->Next;
			delete DeletingNode;
		}
		delete CurrentNode;
		CurrentNode = nullptr;
	}
	
} // namespace VE