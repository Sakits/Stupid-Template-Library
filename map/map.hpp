#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>

#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu 
{

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> 
class map;

template<
	class KeyType,
	class T,
	class Compare = std::less<KeyType>
>
class RBTree
{
	friend class map<KeyType, T, Compare>;
	typedef pair<const KeyType, T> value_type;

private:
	struct Node
    {
		value_type ValueField;
        bool Col;
        Node *LT, *RT, *Fa, *nxt, *pre;

		Node() = delete;

        Node(const KeyType &_Key, const T &_Val, const bool _Col = Red) : 
		ValueField(value_type(_Key, _Val)), Col(_Col), LT(nullptr), RT(nullptr), Fa(nullptr), nxt(nullptr), pre(nullptr) {}

		Node(const Node* const &other) 
		: LT(nullptr), RT(nullptr), Fa(nullptr), nxt(nullptr), pre(nullptr), Col(other->Col), ValueField(other->ValueField) {}

		~Node() 
		{
			delete LT; 
			delete RT; 
		}

        const KeyType Key() const {return ValueField.first;}

        T &Val() {return ValueField.second;}

        const T &Val() const {return ValueField.second;}

		const bool get_Col() const {return Col;}
    };

	Node *Root, *Begin, *End;
	int Size;
    enum ColorSet {Red, Black};
    Compare cmp; 

public:
    RBTree() : Root(nullptr), Begin(nullptr), End(nullptr), Size(0) {}

    ~RBTree() {delete Root;}

	const int get_size() const {return Size;}

	std :: pair <Node*, Node*> copy(Node *&x, const Node *const &y)
	{
		if (!y)
		{
			x = nullptr;
			return std :: pair<Node*, Node*>(nullptr, nullptr);
		}

		x = new Node(y);

		Node *ans1 = nullptr, *ans2 = nullptr;
		if (y->LT)
		{
			std :: pair <Node*, Node*> ans = copy(x->LT, y->LT);
			ans1 = ans.first;
			x->pre = ans.second;
			x->LT->Fa = x;
			x->pre->nxt = x;
		}
		if (y->RT)
		{
			std :: pair <Node*, Node*> ans = copy(x->RT, y->RT);
			x->nxt = ans.first;
			ans2 = ans.second;
			x->RT->Fa = x;
			x->nxt->pre = x;
		}

		return std :: pair<Node*, Node*>(ans1 ? ans1 : x, ans2 ? ans2 : x);
	}

	RBTree(const RBTree &other)
	{
		cmp = other.cmp; 
		Size = other.Size;
		Root = Begin = End = nullptr;

		std :: pair<Node*, Node*> tmp = copy(Root, other.Root);

		Begin = tmp.first;
		End = tmp.second;
	}

	RBTree &operator= (const RBTree &other)
	{
		if (this == &other) return *this;

		cmp = other.cmp; 
		Size = other.Size;
		delete Root; 
		Root = Begin = End = nullptr; 

		std :: pair<Node*, Node*> tmp = copy(Root, other.Root);

		Begin = tmp.first; 
		End = tmp.second;

		return *this;
	}

    void left_rotate(Node* const &x)
    {
        Node *RT = x->RT;

        x->RT = RT->LT;
        if (RT->LT)
            RT->LT->Fa = x;

        RT->Fa = x->Fa;

        if (x->Fa)
            if (x->Fa->LT == x)
                x->Fa->LT = RT;
            else
                x->Fa->RT = RT;
        else
            Root = RT;

        x->Fa = RT;

        RT->LT = x;
    }

    void right_rotate(Node* const &x)
    {
        Node *LT = x->LT;

        x->LT = LT->RT;
        if (LT->RT)
            LT->RT->Fa = x;

        LT->Fa = x->Fa;
        if (x->Fa)
            if (x->Fa->RT == x)
                x->Fa->RT = LT;
            else
                x->Fa->LT = LT;
        else
            Root = LT;

        x->Fa = LT;

        LT->RT = x;
    }

    Node* find(const KeyType &Key, int ty = 0)
    {
        Node *x = Root, *Fa = nullptr;
        while (x)
        {
            Fa = x;
            if (!cmp(x->Key(), Key) && !cmp(Key, x->Key())) return x;
            x = cmp(x->Key(), Key) ? x->RT : x->LT;
        }

        return ty ? Fa : nullptr;
    }

    std :: pair <Node*, bool> insert(const KeyType &Key, const T &Val)
    {
		Node* Fa = find(Key, 1);
		
		if (Fa && !cmp(Fa->Key(), Key) && !cmp(Key, Fa->Key()))
			return std :: make_pair(Fa, 0);

		Size++;
        Node *x = new Node(Key, Val);
		Node *ans = x;

		if (!Begin || cmp(Key, Begin->Key())) Begin = x;
		if (!End || cmp(End->Key(), Key)) End = x;

        x->Fa = Fa;

        if (Fa)
            if (cmp(Fa->Key(), Key))
			{
                Fa->RT = x;
				x->nxt = Fa->nxt; 
				(Fa->nxt) && (Fa->nxt->pre = x);
				x->pre = Fa; 
				Fa->nxt = x;
			}
            else
			{
                Fa->LT = x; 
				x->pre = Fa->pre; 
				(Fa->pre) && (Fa->pre->nxt = x);
				Fa->pre = x; 
				x->nxt = Fa;
			}
        else
            Root = x;

        while (Fa && Fa->Col == Red)
        {
            Node* Gfa = Fa->Fa;
            Node* Unc = (Fa == Gfa->LT) ? Gfa->RT : Gfa->LT;

            if (Unc && Unc->Col == Red)
            {
                Fa->Col = Unc->Col = Black;
                Gfa->Col = Red;
                x = Gfa; Fa = x->Fa;
            }
            else
            {
                if (Fa == Gfa->LT)
                {
                    if (x == Fa->RT)
                        left_rotate(Fa), std :: swap(x, Fa);
                
                    Fa->Col = Black;
                    Gfa->Col = Red;
                    right_rotate(Gfa);
                }
                else
                {
                    if (x == Fa->LT)
                        right_rotate(Fa), std :: swap(x, Fa);

                    Fa->Col = Black;
                    Gfa->Col = Red;
                    left_rotate(Gfa);
                }
            }
        }

        if (!Fa && x->Col == Red) x->Col = Black;

		return std :: make_pair(ans, 1);
    }

    void erase(Node* &x)
    {
		if (x == Begin) Begin = Begin->nxt;
		if (x == End) End = End->pre;

        Node *Fa = x->Fa, *p = nullptr;
        bool DelCol = x->Col;
		Size--;

        if (!x->LT)
        {
            Node *RT = x->RT;
            p = RT;

            if (Fa)
                if (Fa->LT == x)
                    Fa->LT = RT;
                else
                    Fa->RT = RT;
            else
                Root = RT;

            if (RT) RT->Fa = Fa;
        }
        else if (!x->RT)
        {
            Node *LT = x->LT;
            p = LT;

            if (Fa)
                if (Fa->LT == x)
                    Fa->LT = LT;
                else
                    Fa->RT = LT;
            else
                Root = LT;

            if (LT) LT->Fa = Fa;
        }
        else
        {
            Node *y = x->RT;
            while (y->LT) y = y->LT;

            DelCol = y->Col;

            Node* RT = y->RT;
            p = RT;
            if (y->Fa != x)
            {
                Fa = y->Fa;
                Fa->LT = y->RT;
                if (y->RT)
                    y->RT->Fa = Fa;

                y->RT = x->RT;
                x->RT->Fa = y;
            }
            else
                Fa = y;

            if (x->Fa)
                if (x->Fa->LT == x)
                    x->Fa->LT = y;
                else
                    x->Fa->RT = y;
            else
                Root = y;

            y->Fa = x->Fa;
            y->LT = x->LT;
            x->LT->Fa = y;
            y->Col = x->Col;
        }

		(x->pre) && (x->pre->nxt = x->nxt);
		(x->nxt) && (x->nxt->pre = x->pre);
		x->pre = x->nxt = 0;
		x->LT = x->RT = nullptr;
		delete x;

        if (DelCol == Black)
        {
            x = p;
            while (x != Root && (!x || x->Col == Black))
            {
                if (x == Fa->LT)
                {
                    Node *Bro = Fa->RT;
                    if (Bro->Col == Red)
                    {
                        Bro->Col = Black;
                        Fa->Col = Red;
                        left_rotate(Fa);
                        Bro = Fa->RT;
                    }

                    if ((!Bro->LT || Bro->LT->Col == Black) && (!Bro->RT || Bro->RT->Col == Black))
                    {
                        Bro->Col = Red;
                        x = Fa;
                        Fa = x->Fa;
                    }
                    else
                    {
                        if (!Bro->RT || Bro->RT->Col == Black)
                        {
                            Node *Nie = Bro->LT;
                            Nie->Col = Black;
                            Bro->Col = Red;
                            right_rotate(Bro);
                            Bro = Nie;
                        }

                        Bro->Col = Fa->Col;
                        Fa->Col = Black;
                        Bro->RT->Col = Black;
                        left_rotate(Fa);
                        x = Root;
                    }
                }
                else
                {
                    Node* Bro = Fa->LT;
                    if (Bro->Col == Red)
                    {
                        Bro->Col = Black;
                        Fa->Col = Red;
                        right_rotate(Fa);
                        Bro = Fa->LT;
                    }
                    if ((!Bro->LT || Bro->LT->Col == Black) && (!Bro->RT || Bro->RT->Col == Black))
                    {
                        Bro->Col = Red;
                        x = Fa;
                        Fa = x->Fa;
                    }
                    else
                    {
                        if (!Bro->LT || Bro->LT->Col == Black)
                        {
                            Node* Nie = Bro->RT;
                            Nie->Col = Black;
                            Bro->Col = Red;
                            left_rotate(Bro);
                            Bro = Nie;
                        }

                        Bro->Col = Fa->Col;
                        Fa->Col = Black;
                        Bro->LT->Col = Black;
                        right_rotate(Fa);
                        x = Root;
                    }
                }
            }

            if (x) x->Col = Black;
        }
    }
};

template<
	class Key,
	class T,
	class Compare
> 
class map 
{
	typedef RBTree<Key, T, Compare> RBT;
	typedef typename RBT :: Node Node;

private:
	RBT* Tr;

public:

	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */

	typedef pair<const Key, T> value_type;

	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */

	class const_iterator;
	class iterator 
	{
		friend class map;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */

	private:
		RBT *Belong;
		Node *Ptr;
		
	public:
		iterator() : Belong(nullptr), Ptr(nullptr) {}

		iterator(RBT* const &_Belong, Node* const &node) : Belong(_Belong), Ptr(node) {}

		iterator(const iterator &other) : Belong(other.Belong), Ptr(other.Ptr) {}

		/**
		 * TODO iter++
		 */

		iterator operator++(int) 
		{
			if (!Ptr) throw invalid_iterator();
			iterator tmp = *this;
			if (Ptr) Ptr = Ptr->nxt;
			return tmp;
		}

		/**
		 * TODO ++iter
		 */

		iterator & operator++() 
		{
			if (!Ptr) throw invalid_iterator();
			if (Ptr) Ptr = Ptr->nxt;
			return *this;
		}

		/**
		 * TODO iter--
		 */

		iterator operator--(int)
		{
			iterator tmp = *this;
			
			if (Ptr) 
				Ptr = Ptr->pre;
			else
				Ptr = Belong->End;

			if (!Ptr) throw invalid_iterator();
			return tmp;
		}

		/**
		 * TODO --iter
		 */

		iterator & operator--()
		{
			if (Ptr) 
				Ptr = Ptr->pre;
			else
				Ptr = Belong->End;

			if (!Ptr) throw invalid_iterator();
			return *this;
		}

		/**
		 * an operator to check whether two iterators are same (pointing to the same memory).
		 */

		value_type & operator*() const {return Ptr->ValueField;}

		bool operator==(const iterator &rhs) const {return Ptr == rhs.Ptr && Belong == rhs.Belong;}

		bool operator==(const const_iterator &rhs) const {return Ptr == rhs.Ptr && Belong == rhs.Belong;}

		/**
		 * some other operator for iterator.
		 */

		bool operator!=(const iterator &rhs) const {return Ptr != rhs.Ptr || Belong != rhs.Belong;}

		bool operator!=(const const_iterator &rhs) const {return Ptr != rhs.Ptr || Belong != rhs.Belong;}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */

		value_type* operator->() const noexcept {return &(Ptr->ValueField);}

	};
	class const_iterator 
	{
		friend class map;
		
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */

	private:
		RBT *Belong;
		Node* Ptr;

	public:
		const_iterator() : Belong(nullptr), Ptr(nullptr) {}

		const_iterator(RBT* const &_Belong, Node* const &node) : Belong(_Belong), Ptr(node) {}

		const_iterator(const iterator &other) : Belong(other.Belong), Ptr(other.Ptr) {}

		const_iterator(const const_iterator &other) : Belong(other.Belong), Ptr(other.Ptr) {}
		
		/**
		 * TODO iter++
		 */

		const_iterator operator++(int) 
		{
			if (!Ptr) throw invalid_iterator();
			const_iterator tmp = *this;
			if (Ptr) Ptr = Ptr->nxt;
			return tmp;
		}

		/**
		 * TODO ++iter
		 */

		const_iterator & operator++() 
		{
			if (!Ptr) throw invalid_iterator();
			if (Ptr) Ptr = Ptr->nxt;
			return *this;
		}

		/**
		 * TODO iter--
		 */

		const_iterator operator--(int)
		{
			const_iterator tmp = *this;
			
			if (Ptr) 
				Ptr = Ptr->pre;
			else
				Ptr = Belong->End;

			if (!Ptr) throw invalid_iterator();
			return tmp;
		}

		/**
		 * TODO --iter
		 */

		const_iterator & operator--()
		{
			if (Ptr) 
				Ptr = Ptr->pre;
			else
				Ptr = Belong->End;

			if (!Ptr) throw invalid_iterator();
			return *this;
		}

		/**
		 * an operator to check whether two iterators are same (pointing to the same memory).
		 */

		value_type & operator*() const {return Ptr->ValueField;}

		bool operator==(const iterator &rhs) const {return Ptr == rhs.Ptr && Belong == rhs.Belong;}

		bool operator==(const const_iterator &rhs) const {return Ptr == rhs.Ptr && Belong == rhs.Belong;}

		/**
		 * some other operator for iterator.
		 */

		bool operator!=(const iterator &rhs) const {return Ptr != rhs.Ptr || Belong != rhs.Belong;}

		bool operator!=(const const_iterator &rhs) const {return Ptr != rhs.Ptr || Belong != rhs.Belong;}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */

		value_type* operator->() const noexcept {return &(Ptr->ValueField);}

	};

	/**
	 * TODO two constructors
	 */

	map() : Tr(new RBT()) {}

	map(const map &other) : Tr(new RBT(*(other.Tr))) {}

	/**
	 * TODO assignment operator
	 */

	map & operator=(const map &other) 
	{
		if (this == &other) return *this;
		*Tr = *other.Tr;
		return *this;
	}

	/**
	 * TODO Destructors
	 */
	
	~map() 
	{
		delete Tr;
	}

	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */

	T & at(const Key &key) 
	{
		Node* Ptr = Tr->find(key);
		if (!Ptr) throw index_out_of_bound();
		return Ptr->Val();
	}

	const T & at(const Key &key) const 
	{
		Node* Ptr = Tr->find(key);
		if (!Ptr) throw index_out_of_bound();
		return Ptr->Val();
	}

	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */

	T & operator[](const Key &key) 
	{
		return Tr->insert(key, T()).first->Val();
	}

	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */

	const T & operator[](const Key &key) const 
	{
		Node* Ptr = Tr->find(key);
		if (!Ptr) throw index_out_of_bound();
		return Ptr->Val();
	}

	/**
	 * return a iterator to the beginning
	 */

	iterator begin() 
	{
		return iterator(Tr, Tr->Begin);
	}

	const_iterator cbegin() const 
	{
		return const_iterator(Tr, Tr->Begin);
	}

	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */

	iterator end() 
	{
		return iterator(Tr, nullptr);
	}

	const_iterator cend() const 
	{
		return const_iterator(Tr, nullptr);
	}

	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */

	bool empty() const 
	{
		return !Tr->get_size();
	}

	/**
	 * returns the number of elements.
	 */

	size_t size() const 
	{
		return Tr->get_size();
	}

	/**
	 * clears the contents
	 */

	void clear() 
	{
		delete Tr;
		Tr = new RBT();
	}

	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */

	pair<iterator, bool> insert(const value_type &value) 
	{
		std :: pair<Node*, bool> ans = Tr->insert(value.first, value.second);
		return pair<iterator, bool>(iterator(Tr, ans.first), ans.second);
	}

	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */

	void erase(iterator pos) 
	{
		if (pos.Ptr && Tr->find(pos.Ptr->Key()) == pos.Ptr)
			Tr->erase(pos.Ptr);
		else 
			throw invalid_iterator();
	}

	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */

	size_t count(const Key &key) const 
	{
		return Tr->find(key) != nullptr;
	}

	/**
	 * finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */

	iterator find(const Key &key) 
	{
		Node* ans = Tr->find(key);
		return iterator(Tr, ans ? ans : nullptr);
	}

	const_iterator find(const Key &key) const 
	{
		Node* ans = Tr->find(key);
		return const_iterator(Tr, ans ? ans : nullptr);
	}
};
}

#endif