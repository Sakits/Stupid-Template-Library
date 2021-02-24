#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"



/**
 * a container like std::priority_queue which is a heap internal.
 */
	// template <typename T, class Compare = std::less <T> >

typedef int T;
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

	public:
		/**
		 * TODO constructors
		 */
		priority_queue() : Root(nullptr) {}

		void Copy(Node *&x, const Node *&y)
		{
			if (!y)
				return;
			x = new Node(y.Val);
			Copy(x->Left, y->Left);
			Copy(x->Right, y->Right);
		}

		priority_queue(const priority_queue &other)
		{
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
			delete Root;
			Copy(Root, other.Root);
		}
		/**
		 * get the top of the queue.
		 * @return a reference of the top element.
		 * throw container_is_empty if empty() returns true;
		 */
		const T &top() const
		{
			return Root->Val;
		}
		/**
		 * TODO
		 * push new element to the priority queue.
		 */
		void push(const T &e)
		{
		}


		Node* Heap_Merge(Node* &x, Node* &y)
		{
			if (!x || !y) return x ? x : y;

			if (x -> Val > y -> Val) swap(x, y);

			x -> Right = Heap_Merge(x -> Right, y);

			if (rand() & 1)
				swap(x -> Left, x -> Right);


			return x;
		}

		/**
		 * TODO
		 * delete the top element.
		 * throw container_is_empty if empty() returns true;
		 */
		void pop()
		{
			
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
			return Size != 0;
		}
		/**
		 * return a merged priority_queue with at least O(logn) complexity.
		 */
		void merge(priority_queue &other)
		{
		}
	};


#endif
