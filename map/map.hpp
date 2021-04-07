/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map;

template<
	class KeyType,
	class T,
	class Compare = std::less<KeyType>
>
class RBTree
{
	friend class map<KeyType, T, Compare>;
    static const int inf = (1ll << 31) - 1;
private:
    enum ColorSet {Red, Black};
    Compare cmp;

    struct Node
    {
        typedef std :: pair<const KeyType, T> value_type;

		value_type ValueField;
        int Size;
        Node *LT, *RT, *Fa;
        bool Col;

        const KeyType Key() const {return ValueField.first;}

        T &Val() {return ValueField.second;}

		int get_size() {return Size;}

		Node() 
		{
			Size = 0; 
			Col = Red;
			LT = RT = Fa = nullptr;
		}

        Node(KeyType _Key, T _Val) : ValueField(make_pair(_Key, _Val))
        {
            Size = 1;
            Col = Red;
            LT = RT = Fa = nullptr;
        }

        ~Node() {delete LT; delete RT;}

    };

public:
	Node *Root, *Begin, *End;

    RBTree() {Root = nullptr; Begin = End;}
    ~RBTree() {delete Root;}

	void copy(Node *&x, const Node *const &y)
	{
		if (!y)
		{
			delete x;
			x = nullptr;
			return;
		}

		if (!x)
			x = new Node(y->ValueField);
		else
			x->ValueField = y->ValueField;

		copy(x->Left, y->Left);
		copy(x->Right, y->Right);
	}

	RBTree (const RBTree &other)
	{
		cmp = other.cmp;
		copy(Root, other.Root);
	}

	RBTree &operator= (const RBTree &other)
	{
		if (this == &other) return this;
		cmp = other.cmp;
		copy(Root, other.Root);
		return this;
	}

    void Update(Node* const &x)
    {
        x->Size = (x->LT ? x->LT->Size : 0) + (x->RT ? x->RT->Size : 0) + 1;
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

        Update(x); Update(RT);
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

        Update(x); Update(LT);
    }

    Node* find(const KeyType &Key, int ty = 0)
    {
        Node *x = Root, *Fa = nullptr;
        while (x)
        {
            Fa = x;
            if (!cmp(x->Key(), Key) && !cmp(Key, x->Key())) return x;
            x = cmp(x->Key(), Key) ? x->RT : x-> LT;
        }

        return ty ? Fa : nullptr;
    }

    std :: pair<Node*, bool> insert(const KeyType &Key, const T &Val)
    {
		Node* Fa = find(Key, 1);
		if (!cmp(Fa->Key(), Key) && !cmp(Key, Fa->Key()))
			return std :: make_pair(Fa, 0);

        Node *x = new Node(Key, Val), *pos = x;

		if (Begin == End || cmp(Key, Begin->Key()))
			Begin = x;

		if (!End->LT || cmp(End->LT->Key(), Key))
			End->LT = x;

        x->Fa = Fa;
        if (Fa)
            if (cmp(Fa->Key(), Key))
                Fa->RT = x;
            else
                Fa->LT = x;
        else
            Root = x;

        Node* y = Fa;
        while (y)
            ++y->Size, y = y->Fa;

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

        if (!Fa && x->Col == Red)
            x->Col = Black;

		return std :: make_pair(pos, 1);
    }

    void erase(const KeyType &Key)
    {
        Node *x = find(Key);
		if (!x) return;

		// if (x == Begin)

        Node *Fa = x->Fa, *p = nullptr;
        bool DelCol = x->Col;

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

            if (RT)
                RT->Fa = Fa;
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

            if (LT)
                LT->Fa = Fa;
        }
        else
        {
            Node *y = x->RT;
            while (y->LT)
                y = y->LT;

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
            y->Size = x->Size;
        }

        Node *y = Fa;
        while (y)
            --y->Size, y = y->Fa;

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

            if (x)
                x->Col = Black;
        }
    }

	Node* get_pre(const Node* &x)
    {
		if (x == Begin)
			return nullptr;

        Node* y = x;
		if (y->LT)
		{
			y = y->LT;
			while (y->RT)
				y = y->RT;
		}
		else
		{
			while(y->Fa && y->Fa->LT == y)
				y = y->Fa;

			y = (y == Root) ? nullptr : y->Fa;
		}

		return y;
    }

	Node* get_next(const Node* &x)
    {
		if (x == End)
			return nullptr;

        Node* y = x;
		if (y->RT)
		{
			y = y->RT;
			while (y->LT)
				y = y->LT;
		}
		else
		{
			while(y->Fa && y->Fa->RT == y)
				y = y->Fa;

			y = (y == Root) ? End : y->Fa;
		}

		return y;
    }
};



template<
	class Key,
	class T,
	class Compare
> 
class map {
	typedef RBTree<Key, T, Compare> RBT;
	typedef typename RBT :: Node Node;
private:
	RBT Tr;
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
	class iterator {
	private:
		Node* pos;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		iterator() {
			// TODO
		}
		iterator(const iterator &other) {
			// TODO
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {}
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {}
		/**
		 * TODO --iter
		 */
		iterator & operator--() {}
		/**
		 * an operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {}
		bool operator==(const iterator &rhs) const {}
		bool operator==(const const_iterator &rhs) const {}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {}
		bool operator!=(const const_iterator &rhs) const {}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
		public:
			const_iterator() {
				// TODO
			}
			const_iterator(const const_iterator &other) {
				// TODO
			}
			const_iterator(const iterator &other) {
				// TODO
			}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
			iterator operator++(int) {}
			/**
			 * TODO ++iter
			 */
			iterator & operator++() {}
			/**
			 * TODO iter--
			 */
			iterator operator--(int) {}
			/**
			 * TODO --iter
			 */
			iterator & operator--() {}
			/**
			 * an operator to check whether two iterators are same (pointing to the same memory).
			 */
			value_type & operator*() const {}
			bool operator==(const iterator &rhs) const {}
			bool operator==(const const_iterator &rhs) const {}
			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator &rhs) const {}
			bool operator!=(const const_iterator &rhs) const {}

			/**
			 * for the support of it->first. 
			 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
			 */
			value_type* operator->() const noexcept {}
	};
	/**
	 * TODO two constructors
	 */
	map() {}
	map(const map &other) {}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {}
	/**
	 * TODO Destructors
	 */
	~map() {}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {}
	const T & at(const Key &key) const {}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {}
	const_iterator cbegin() const {}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {}
	const_iterator cend() const {}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {}
	/**
	 * clears the contents
	 */
	void clear() {}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {}
	const_iterator find(const Key &key) const {}
};

}

#endif
