#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu
{

	/**
 * a container like std::priority_queue which is a heap internal.
 */
	template <typename T, class Compare = std::less<T> >
	// typedef int T;
	class priority_queue
	{
	private:
		struct Node
		{
			friend class priority_queue;

		private:
			T Val;
			Node *Left, *Right;

		public:
			Node(const T &_Val) : Val(_Val), Left(nullptr), Right(nullptr) {}

			~Node()
			{
				delete Left;
				delete Right;
			}
		};

		Node *Root;
		int Size;
		Compare cmp;

	public:
		/**
		 * TODO constructors
		 */
		priority_queue() : Root(nullptr), Size(0) {}

		void Copy(Node *&x, const Node *const &y)
		{
			if (!y)
				return;
			x = new Node(y->Val);
			Copy(x->Left, y->Left);
			Copy(x->Right, y->Right);
		}

		priority_queue(const priority_queue &other)
		{
			Size = other.Size;
			cmp = other.cmp;
			Copy(Root, other.Root);
		}
		/**
		 * TODO deconstructor
		 */
		~priority_queue() { delete Root; }
		/**
		 * TODO Assignment operator
		 */
		priority_queue &operator=(const priority_queue &other)
		{
			if (this == &other) return *this;
			Size = other.Size;
			cmp = other.cmp;
			delete Root;
			Copy(Root, other.Root);
			return *this;
		}
		/**
		 * get the top of the queue.
		 * @return a reference of the top element.
		 * throw container_is_empty if empty() returns true;
		 */
		const T &top() const
		{
			if (empty()) throw container_is_empty();
			return Root->Val;
		}

		Node *Heap_Merge(Node *&x, Node *&y)
		{
			if (!x || !y)
				return x ? x : y;

			if (cmp(x->Val, y->Val))
				std ::swap(x, y);

			x->Right = Heap_Merge(x->Right, y);

			// if (rand() & 1)
				std ::swap(x->Left, x->Right);

			return x;
		}

		/**
		 * TODO
		 * push new element to the priority queue.
		 */
		void push(const T &e)
		{
			Size++;
			Node* NewNode = new Node(e);
			Root = Heap_Merge(Root, NewNode);
		}
		/**
		 * TODO
		 * delete the top element.
		 * throw container_is_empty if empty() returns true;
		 */
		void pop()
		{
			if (empty()) throw container_is_empty();
			Size--;
			Node *Left = Root->Left, *Right = Root->Right;
			Root->Left = Root->Right = nullptr;
			delete Root;
			Root = Heap_Merge(Left, Right);
		}
		/**
		 * return the number of the elements.
		 */
		size_t size() const
		{
			return Size;
		}
		/**
		 * check if the container has at least an element.
		 * @return true if it is empty, false if it has at least an element.
		 */
		bool empty() const
		{
			return Size == 0;
		}
		/**
		 * return a merged priority_queue with at least O(logn) complexity.
		 */
		void merge(priority_queue &other)
		{
			Root = Heap_Merge(Root, other.Root);
			Size += other.Size;
			other.Root = nullptr;
			other.Size = 0;
		}
	};

}

#endif
