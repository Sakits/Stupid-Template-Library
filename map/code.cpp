#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>


#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu {

class exception {
protected:
	const std::string variant = "";
	std::string detail = "";
public:
	exception() {}
	exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
	virtual std::string what() {
		return variant + " " + detail;
	}
};

class index_out_of_bound : public exception {
	/* __________________________ */
};

class runtime_error : public exception {
	/* __________________________ */
};

class invalid_iterator : public exception {
	/* __________________________ */
};

class container_is_empty : public exception {
	/* __________________________ */
};
}

#endif
#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <utility>

namespace sjtu {

template<class T1, class T2>
class pair {
public:
	T1 first;
	T2 second;
	constexpr pair() : first(), second() {}
	pair(const pair &other) = default;
	pair(pair &&other) = default;
	pair(const T1 &x, const T2 &y) : first(x), second(y) {}
	template<class U1, class U2>
	pair(U1 &&x, U2 &&y) : first(x), second(y) {}
	template<class U1, class U2>
	pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
	template<class U1, class U2>
	pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}
};

}

#endif



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
	typedef pair<const KeyType, T> value_type;
private:
	struct Node
    {
		value_type* ValueField;
        int Size;
        Node *LT, *RT, *Fa;
        bool Col;

        const KeyType Key() const {return ValueField->first;}

        T &Val() {return ValueField->second;}

		const int get_size() const {return Size;}

		Node()
		{
			ValueField = nullptr;
			LT = RT = Fa = nullptr;
			Size = 1;
			Col = Red;
		}

		Node(const Node &other) : Size(other.Size), Col(other.Col)
		{
			LT = RT = Fa = nullptr;
			ValueField = other.ValueField ? new value_type(*other.ValueField) : nullptr;
		}

        Node(const KeyType &_Key, const T &_Val)
        {
			ValueField = new value_type(_Key, _Val);
            Size = 1;
            Col = Red;
            LT = RT = Fa = nullptr;
        }

        ~Node() {delete LT; delete RT; delete ValueField;}

    };

    enum ColorSet {Red, Black};
    Compare cmp;

public:
	Node *Root, *Begin, *End;

    RBTree() 
	{
		Root = Begin = End = new Node;
		Root->Col = Black;
	}
    ~RBTree() {delete Root;}

	bool cmpnode(const Node * const &a, const Node * const &b)
	{
		if (a == End) return 0;
		if (b == End) return 1;
		return cmp(a->Key(), b->Key());
	}

	bool cmpnode(const Node * const &a, const KeyType &bKey)
	{
		if (a == End) return 0;
		return cmp(a->Key(), bKey);
	}

	bool cmpnode(const KeyType &aKey, const Node * const &b)
	{
		if (b == End) return 1;
		return cmp(aKey, b->Key());
	}

	void copy(Node *&x, Node *const &y, Node *const &Fa, Node *&Begin, Node * &End)
	{
		if (!y)
		{
			delete x;
			x = nullptr;
			return;
		}

		if (x == nullptr)
			x = new Node(*y);
		else
		{
			delete x->ValueField;
			x->Col = y->Col;
			x->Size = y->Size;
			x->ValueField = y->ValueField ? new value_type(*(y->ValueField)) : nullptr;
		}

		x->Fa = Fa;
		(!Begin || (x->ValueField && cmpnode(x, Begin))) && (Begin = x);
		(!x->ValueField) && (End = x);

		copy(x->LT, y->LT, x, Begin, End);
		copy(x->RT, y->RT, x, Begin, End);
	}

	static void get_pre(Node* &y)
	{
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

			y = !y->Fa ? nullptr : y->Fa;
		}
	}

	static void get_next(Node* &y)
	{
		// printf("yKey:%d RT:%d\n", y->Key(), y->RT);
		if (y->RT)
		{
			y = y->RT;
			// printf("y:%d %d\n", y->Key(), y->LT);
			while (y->LT)
				y = y->LT;
		}
		else
		{
			while(y->Fa && y->Fa->RT == y)
				y = y->Fa;

			y = !y->Fa ? nullptr : y->Fa;
		}
	}

	RBTree (const RBTree &other)
	{
		cmp = other.cmp;
		Root = Begin = End = nullptr;
		copy(Root, other.Root, nullptr, Begin, End);
	}

	RBTree &operator= (const RBTree &other)
	{
		if (this == &other) return *this;
		cmp = other.cmp;
		Begin = End = nullptr;
		copy(Root, other.Root, nullptr, Begin, End);
		return *this;
	}

    void update(Node* const &x)
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

        update(x); update(RT);
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

        update(x); update(LT);
    }

    Node* find(const KeyType &Key, int ty = 0)
    {
        Node *x = Root, *Fa = nullptr;
        while (x)
        {
			// if (x != End)
			// 	printf("Key:%d %d %d\n", Key, x->Key(), cmpnode(x, Key));
            Fa = x;
            if (x != End && !cmpnode(x, Key) && !cmpnode(Key, x)) return x;
            x = cmpnode(x, Key) ? x->RT : x-> LT;
        }

        return ty ? Fa : nullptr;
    }

    std :: pair <Node*, bool> insert(const KeyType &Key, const T &Val)
    {
		Node* Fa = find(Key, 1);
		
		if (Fa && Fa != End && !cmpnode(Fa, Key) && !cmpnode(Key, Fa))
			return std :: make_pair(Fa, 0);

        Node *x = new Node(Key, Val);
		Node *ans = x;
		if (cmpnode(Key, Begin))
			Begin = x;

        x->Fa = Fa;
        if (Fa)
            if (cmpnode(Fa, Key))
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

		return std :: make_pair(ans, 1);
    }

    bool erase(Node* &x)
    {
		if (x == Begin)
			get_next(Begin);

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

		x->LT = x->RT = nullptr;
		delete x;

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
		return 1;
    }

	void dfs(Node* x)
	{
		if (!x) return;
		// printf("x:%d\n", x->ValueField);
		if (x->ValueField)
		{
			printf("x:%d ", x->Key(), x->Fa);
			if (x == Root)
				printf("Root\n");
			else
				if (x->Fa->ValueField)
					printf("%d\n", x->Fa->Key());
				else
					printf("END\n");		
		}
		else
			printf("END\n");
		// puts("?????????");
		dfs(x->LT); dfs(x->RT);
	}
};

template<
	class Key,
	class T,
	class Compare
> class map {
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
	class iterator {
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		Node *Ptr;
		iterator() {
			Ptr = nullptr;
		}

		iterator(Node* const &node)
		{
			Ptr = node;
		}

		iterator(const iterator &other) {
			Ptr = other.Ptr;
		}
		
		/**
		 * TODO iter++
		 */
		iterator operator++(int) 
		{
			iterator tmp = *this;
			if (Ptr) RBT :: get_next(Ptr);
			if (!Ptr) throw invalid_iterator();
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++() 
		{
			if (Ptr) RBT :: get_next(Ptr);
			if (!Ptr) throw invalid_iterator();
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int)
		{
			iterator tmp = *this;
			if (Ptr) RBT :: get_pre(Ptr);
			if (!Ptr) throw invalid_iterator();
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--()
		{
			if (Ptr) RBT :: get_pre(Ptr);
			if (!Ptr) throw invalid_iterator();
			return *this;
		}
		/**
		 * an operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {return *(Ptr->ValueField);}
		bool operator==(const iterator &rhs) const {return Ptr == rhs.Ptr;}
		bool operator==(const const_iterator &rhs) const {return Ptr == rhs.Ptr;}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {return Ptr != rhs.Ptr;}
		bool operator!=(const const_iterator &rhs) const {return Ptr != rhs.Ptr;}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {return &*(Ptr->ValueField);}
	};
	class const_iterator {
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		Node* Ptr;
		const_iterator() {
			Ptr = nullptr;
			// TODO
		}

		const_iterator(Node* const &node)
		{
			Ptr = node;
		}

		const_iterator(const iterator &other) {
			Ptr = other.Ptr;
			// TODO
		}

		const_iterator(const const_iterator &other) {
			Ptr = other.Ptr;
			// TODO
		}
		
		/**
		 * TODO iter++
		 */
		const_iterator operator++(int) 
		{
			const_iterator tmp = *this;
			if (Ptr) RBT :: get_next(Ptr);
			if (!Ptr) throw invalid_iterator();
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		const_iterator & operator++() 
		{
			if (Ptr) RBT :: get_next(Ptr);
			if (!Ptr) throw invalid_iterator();
			return *this;
		}
		/**
		 * TODO iter--
		 */
		const_iterator operator--(int)
		{
			const_iterator tmp = *this;
			if (Ptr) RBT :: get_pre(Ptr);
			if (!Ptr) throw invalid_iterator();
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		const_iterator & operator--()
		{
			if (Ptr) RBT :: get_pre(Ptr);
			if (!Ptr) throw invalid_iterator();
			return *this;
		}
		/**
		 * an operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {return *(Ptr->ValueField);}
		bool operator==(const iterator &rhs) const {return Ptr == rhs.Ptr;}
		bool operator==(const const_iterator &rhs) const {return Ptr == rhs.Ptr;}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {return Ptr != rhs.Ptr;}
		bool operator!=(const const_iterator &rhs) const {return Ptr != rhs.Ptr;}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {return &*(Ptr->ValueField);}
	};
	/**
	 * TODO two constructors
	 */
	map() {Tr = new RBT();}
	map(const map &other) 
	{
		Tr = new RBT(*(other.Tr));
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) 
	{
		if (this == &other)
			return *this;

		*Tr = *other.Tr;

		return *this;
	}
	/**
	 * TODO Destructors
	 */
	~map() {delete Tr;}
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
		Node* Ptr = Tr->find(key);
		return Ptr ? Ptr->Val() : Tr->insert(key, T()).first->Val();
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
	iterator begin() {return iterator(Tr->Begin);}
	const_iterator cbegin() const {return const_iterator(Tr->Begin);}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {return iterator(Tr->End);}
	const_iterator cend() const {return const_iterator(Tr->End);}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {return !(Tr->Root->get_size() - 1);}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {return Tr->Root->get_size() - 1;}
	/**
	 * clears the contents
	 */
	void clear() {delete Tr; Tr = new RBT;}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) 
	{
		std :: pair<Node*, bool> ans = Tr->insert(value.first, value.second);
		return pair<iterator, bool>(iterator(ans.first), ans.second);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) 
	{
		if (pos.Ptr && pos.Ptr != Tr->End && Tr->find(pos.Ptr->Key()) == pos.Ptr)
			Tr->erase(pos.Ptr);
		else 
			throw invalid_iterator();
	}

	void dfs()
	{
		Tr->dfs(Tr->Root);
	}

	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {return Tr->find(key) != nullptr;}
	/**
	 * finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) 
	{
		Node* ans = Tr->find(key);
		return iterator(ans ? ans : Tr->End);
	}
	const_iterator find(const Key &key) const 
	{
		Node* ans = Tr->find(key);
		return const_iterator(ans ? ans : Tr->End);
	}
};

}

#endif


#include<iostream>
#include<cstdio>
#include<algorithm>
#include<cstring>

long long aa=13131,bb=5353,MOD=(long long)(1e9+7),now=1;
int rand()
{
	for(int i=1;i<3;i++)
		now=(now * aa + bb) % MOD;
	return now;
}
class Key{
public:
	int x;
	Key(const Key &other):x(other.x){}	
	Key(int x):x(x){}
	int num()const{
		return x;	
	}
};
class Data{
public:
	int* x;
	Data():x(new int(1)){}
	Data(int p):x(new int(p)){}
	Data(const Data &other):x(new int(*(other.x))){}
	~Data(){
		delete x;
	}
	Data & operator = (const Data &other){
		if(this == &other) return *this;
		*x = *(other.x);
		return *this;	
	}
	int num()const{
		return *x;	
	}
};
struct cmp{
	bool operator ()(const Key &a,const Key &b)const{return a.x > b.x;} 	
};
sjtu::map<Key,Data,cmp> map;
int num = 5000;
void test_insert(){
	puts("Test: insert");
	std::cout<<"empty:"<<map.empty()<<std::endl;
	std::cout<<"size:"<<map.size()<<std::endl;
	int num = 1000;
	for(int i=1;i<=num;i++)
	{
		int tmp(rand() % 10000);
		map[Key(tmp)] = Data(rand());
	}
	for(int i=1;i<=num;i++)
	{
		try{	
			map.at(Key(rand() % 10000)) = Data(rand());	
		}catch(...){}
	}
	for(int i=1;i<=num;i++){
		Key key(rand() % 10000);
		Data data(rand());
		sjtu::pair<sjtu::map<Key,Data,cmp>::iterator,bool> it = map.insert(sjtu::map<Key,Data,cmp>::value_type(key,data));
		if (it.second) std::cout<<(it.first->second).num()<<' ';
		else std::cout<<"0 ";
	}
	puts("");
	for(int i=1;i<=num;i++){
		int tmp(rand() % 10000);
		std::cout<<(map[Key(tmp)]).num()<<' ';
	}
	puts("");
	for(int i=1;i<=num;i++){
		int tmp(rand() % 10000);
		try{
			std::cout<<(map.at(Key(tmp))).num()<<' ';
		}catch(...){}
	}
	puts("");
}
void Print(){
	for(sjtu::map<Key,Data,cmp>::const_iterator it(map.cbegin());it!=map.cend();++it)
		std::cout<<(it->first).num()<<' '<<((*it).second).num()<<' ';
	puts("");
}
void test_erase(){
	puts("Test: erase");
	for(int i=1;i<=100 && !map.empty();i++){
		int p = rand() % map.size();
		sjtu::map<Key,Data,cmp>::iterator it(map.begin());
		while (p--) it++;
		map.erase(it);
	}
	Print();
}
void test_const_at(){
	puts("Test: const_at");
	const sjtu::map<Key,Data,cmp> mm(map);
	for(int i=1;i<=num;i++){
		int tmp(rand() % 10000);
		try{
			std::cout<<(mm.at(Key(tmp))).num()<<' ';
		}catch(...){}
	}
	
	for(int i=1;i<=num;i++){
		int tmp(rand() % 10000);
		try{
			std::cout<<(mm[Key(tmp)]).num()<<' ';
		}catch(...){}
	}
	puts("");
}
void test_count(){
	puts("Test: count");
	const sjtu::map<Key,Data,cmp> mm(map);
	for(int i=1;i<=num;i++){
		int tmp(rand() % 10000);
		try{
			std::cout<<(int)(mm.count(Key(tmp)))<<' ';
		}catch(...){}
	}
	puts("");
}
void test_find(){
	puts("Test: find");
	const sjtu::map<Key,Data,cmp> mm(map);
	for(int i=1;i<=num;i++){
		int tmp(rand() % 10000);
		sjtu::map<Key,Data,cmp>::iterator it(map.find(Key(tmp)));
		if(it == map.end()) std::cout<<"END ";
		else std::cout<<(it->first).num()<<' '<<(it->second).num()<<' ';
	}
	puts("");
	
	for(int i=1;i<=num;i++){
		int tmp(rand() % 10000);
		sjtu::map<Key,Data,cmp>::const_iterator it(mm.find(Key(tmp)));
		if(it == mm.cend()) std::cout<<"END ";
		else std::cout<<(it->first).num()<<' '<<(it->second).num()<<' ';
	}
	puts("");
}
void test_throw(){
	puts("Test: throw");
	sjtu::map<Key,Data,cmp>::iterator it;
	it = map.begin();
	try{
		it--;	
	}catch(...){puts("OK1");}
	try{
		--it;	
	}catch(...){puts("OK2");}
	it = map.end();
	try{
		it++;	
	}catch(...){puts("OK3");}
	try{
		++it;	
	}catch(...){puts("OK4");}
	sjtu::map<Key,Data,cmp> new_map;
	it = new_map.begin();
	try{
		it++;	
	}catch(...){puts("OK5");}
	try{
		++it;	
	}catch(...){puts("OK6");}
}
void test_const_throw(){
	puts("Test: const_throw");
	const sjtu::map<Key,Data,cmp> mm(map);
	sjtu::map<Key,Data,cmp>::const_iterator it;
	it = mm.cbegin();
	try{
		it--;	
	}catch(...){puts("OK7");}
	try{
		--it;	
	}catch(...){puts("OK8");}
	it = map.cend();
	try{
		it++;	
	}catch(...){puts("OK9");}
	try{
		++it;	
	}catch(...){puts("OK10");}
	const sjtu::map<Key,Data,cmp> new_map;
	it = new_map.cbegin();
	try{
		it++;	
	}catch(...){puts("OK11");}
	try{
		++it;	
	}catch(...){puts("OK12");}
}
void test_copy()
{
	puts("Test: copy");
	sjtu::map<Key,Data,cmp> m1(map),m2;
	m2 = map;
	map.clear();
	std::cout<<map.size()<<' '<<map.empty()<<std::endl;
	sjtu::map<Key,Data,cmp>::iterator it1(m1.begin()),it2(m2.begin());
	for(;it1!=m1.end() || it2!=m2.end();it1++,it2++)
	{
		std::cout<<(it1->first).num()<<' '<<(it1->second).num()<<' '<<(it2->first).num()<<' '<<(it2->second).num()<<' ';
	}
	puts("");
}
void test_iterator()
{
	sjtu::map<Key,Data,cmp>::iterator it1 = map.begin();
	for(int i=1;i<=3;i++)
	{
		int tmp = rand() % 5;
		while (tmp--)
		{
			try{
				sjtu::map<Key,Data,cmp>::const_iterator it2(it1++);
				sjtu::map<Key,Data,cmp>::const_iterator it3(++it1);
				sjtu::map<Key,Data,cmp>::iterator it4(it1++);
				sjtu::map<Key,Data,cmp>::iterator it5(++it1);
				std::cout<<(it2->first).num()<<' '<<(it2->second).num()<<std::endl;
				std::cout<<(it3->first).num()<<' '<<(it3->second).num()<<std::endl;
				std::cout<<(it4->first).num()<<' '<<(it4->second).num()<<std::endl;
				std::cout<<((*it5).first).num()<<' '<<((*it5).second).num()<<std::endl;
			}catch(...){}
		}
	}
}
int main(){
	test_insert();
	test_erase();
	test_const_at();
	test_count();
	test_find();
	test_copy();
	test_iterator();
}
