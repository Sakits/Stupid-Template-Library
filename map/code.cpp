#pragma GCC optimize(2)
#pragma GCC optimize(3)
#pragma GCC optimize("Ofast")
#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <utility>
int counterr = 0;

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

#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>

//#define debug

namespace sjtu {
    template<class Key, class T, class Compare = std::less<Key> >
    class map {
    public:
        typedef pair<const Key, T> value_type;
        
        class iterator;
        
        class const_iterator;
    
    private:
        enum colorType {
            RED, BLACK
        };
        
        enum positionType {
            NONE, OUTBOARD, INBOARD, BOTH
        };
        
        class Node {
        public:
            enum rotateType {
                LEFT, RIGHT, PARENT
            };
            
            colorType color = RED;
            value_type *value = nullptr;
            Node *parent = nullptr;
            Node *left = nullptr;
            Node *right = nullptr;
            
            Node() = default;
            
            explicit Node(const value_type &o, colorType c) : value(new value_type(o)), color(c) {}
            
            explicit Node(value_type *o, colorType c) : value(o), color(c) {}
            
            Node(const Node &o) : value(new value_type(*o.value)), color(o.color) {}
            
            Node &operator=(const Node &o) = delete;
            
            ~Node() {
                delete value;
            }
            
            Node *&selfFromParent() {
                if (isRoot())return parent->parent;
                else if (isLeft())return parent->left;
                else return parent->right;
            }
            
            Key getKey() const {
                return value->first;
            }
            
            void setColor(colorType c) {
                color = c;
            }
            
            bool isRed() const {
                return color == RED;
            }
            
            bool isLeft() const {
                if (parent->left == this)return true;
                else return false;
            }
            
            bool isRight() const {
                if (parent->right == this)return true;
                else return false;
            }
            
            bool isHeader() const {
                if (parent == nullptr)return true;
                return parent->parent == this && value == nullptr;
            }
            
            bool isRoot() const {
                if (parent == nullptr)return false;
                return parent->parent == this && value != nullptr;
            }
            
            int childNumber() const {
                int cnt = 0;
                if (left != nullptr)cnt++;
                if (right != nullptr)cnt++;
                return cnt;
            }
            
            int redChildNumber() const {
                int cnt = 0;
                if (left != nullptr && left->isRed())cnt++;
                if (right != nullptr && right->isRed())cnt++;
                return cnt;
            }
            
            positionType redChildPosition() {
                switch (redChildNumber()) {
                    case 0:
                        return NONE;
                    case 1:
                        if (isLeft()) {
                            if (left != nullptr && left->isRed())return OUTBOARD;
                            else return INBOARD;
                        }
                        else {
                            if (right != nullptr && right->isRed())return OUTBOARD;
                            else return INBOARD;
                        }
                    default:
                        return BOTH;
                }
            }
            
            rotateType findRotateType() {
                if (isRoot())return PARENT;
                else if (isLeft())return LEFT;
                else return RIGHT;
            }
            
            Node *&self(rotateType r) {
                switch (r) {
                    case LEFT:
                        return parent->left;
                    case RIGHT:
                        return parent->right;
                    default:
                        return parent->parent;
                }
            }
            
            void rotateLeft() {
                rotateType r = findRotateType();
                self(r) = right;
                right = right->left;
                if (right != nullptr)right->parent = this;
                self(r)->parent = parent;
                self(r)->left = this;
                parent = self(r);
            }
            
            void rotateRight() {
                rotateType r = findRotateType();
                self(r) = left;
                left = left->right;
                if (left != nullptr)left->parent = this;
                self(r)->parent = parent;
                self(r)->right = this;
                parent = self(r);
            }
            
            void swapColor(Node *other) {
                colorType c = other == nullptr ? BLACK : other->color;
                if (other != nullptr)other->setColor(color);
                setColor(c);
            }
            
            void swapPos(Node *other) {
                if (other->parent == this)other->swapPos(this);
                else if (parent == other) {
                    bool le = isLeft();
                    Node *l = left, *r = right, *sib = getSibling();
                    other->selfFromParent() = this, parent = other->parent, other->parent = this;
                    if (l != nullptr)l->parent = other;
                    if (r != nullptr)r->parent = other;
                    if (sib != nullptr)sib->parent = this;
                    other->left = l, other->right = r;
                    if (le)right = sib, left = other;
                    else left = sib, right = other;
                }
                else {
                    selfFromParent() = other;
                    other->selfFromParent() = this;
                    if (left != nullptr)left->parent = other;
                    if (right != nullptr)right->parent = other;
                    Node *l = left, *r = right, *p = parent;
                    left = other->left, right = other->right, parent = other->parent;
                    if (other->left != nullptr)other->left->parent = this;
                    if (other->right != nullptr)other->right->parent = this;
                    other->left = l, other->right = r, other->parent = p;
                }
            }
            
            Node *getSibling() const {
                if (isRoot())return nullptr;
                else if (isLeft())return parent->right;
                else return parent->left;
            }
            
            Node *findSuccessor() {
                Node *y = this; 

                if (y->right)
                {
                    counterr++;
                    y = y->right;
                    while (y->left)
                        y = y->left;//, counterr++;
                }
                else
                {
                    counterr++;
                    while(y->parent && y->parent->right == y)
                        y = y->parent;//, counterr++;;

                    y = !y->parent ? nullptr : y->parent;
                }

                return y;
            }
        };
        
        Node *header = nullptr; //header's parent is root, left is leftmost, right is rightmost
        int nodeNumber = 0;
        Compare compare;
        
        inline bool less(const Key &a, const Key &b) const {
            return compare(a, b);
        }
        
        inline bool equal(const Key &a, const Key &b) const {
            return !(less(a, b) || less(b, a));
        }
        
        void recursionClear(Node *now) {
            if (now == nullptr)return;
            recursionClear(now->left);
            recursionClear(now->right);
            delete now;
        }
        
        Node *recursionConstruct(Node *pa, Node *now) {
            if (now == nullptr)return nullptr;
            Node *temp = new Node(*now);
            temp->parent = pa;
            temp->left = recursionConstruct(temp, now->left);
            temp->right = recursionConstruct(temp, now->right);
            return temp;
        }
        
        Node *findNode(const Key &o) const {
            Node *temp = header->parent;
            while (temp != nullptr) {
                if (equal(o, temp->getKey()))return temp;
                temp = less(o, temp->getKey()) ? temp->left : temp->right;
            }
            return nullptr;
        }
        
        void rebalanceInsert(Node *now) {
            //rebalance red parent and black uncle
            if (now->isLeft()) {
                if (now->parent->isLeft()) {
                    now->parent->parent->rotateRight();
                    now->parent->swapColor(now->parent->right);
                }
                else { //parent->isRight()
                    now->parent->rotateRight();
                    now->parent->rotateLeft();
                    now->swapColor(now->left);
                }
            }
            else { //isRight()
                if (now->parent->isRight()) {
                    now->parent->parent->rotateLeft();
                    now->parent->swapColor(now->parent->left);
                }
                else { //parent->isLeft()
                    now->parent->rotateLeft();
                    now->parent->rotateRight();
                    now->swapColor(now->right);
                }
            }
        }
        
        Node *findInsertPos(const value_type &value) {
            Node *pa = header;
            Node *now = header->parent;
            while (now != nullptr) {
                if (now->redChildNumber() == 2) {
                    now->left->setColor(BLACK), now->right->setColor(BLACK);
                    if (!now->isRoot()) {
                        now->setColor(RED);
                        if (now->parent->isRed())rebalanceInsert(now);
                    }
                }
                pa = now;
                now = less(value.first, now->getKey()) ? now->left : now->right;
            }
            return pa;
        }
        
        iterator RedBlackTreeInsert(Node *insertPos, const value_type &value) {
            nodeNumber++;
            if (insertPos == header) { //empty tree
                header->parent = new Node(value, BLACK);
                header->parent->parent = header;
                header->left = header->right = header->parent;
                return iterator(header->parent);
            }
            Node *insertNode = new Node(value, RED);
            if (less(value.first, insertPos->getKey())) {
                if (insertPos == header->left)header->left = insertNode;
                insertPos->left = insertNode;
            }
            else {
                if (insertPos == header->right)header->right = insertNode;
                insertPos->right = insertNode;
            }
            insertNode->parent = insertPos;
            if (insertNode->parent->isRed())rebalanceInsert(insertNode);
            return iterator(insertNode);
        }
        
        void rebalanceErase(Node *now) {
            if (now->isRoot())return;
            if (now->getSibling() != nullptr && now->getSibling()->isRed()) {
                now->parent->swapColor(now->getSibling());
                if (now->isLeft())now->parent->rotateLeft();
                else now->parent->rotateRight();
            }
            positionType pt;
            if (now->getSibling() == nullptr || (pt = now->getSibling()->redChildPosition()) == NONE) {
                if (now->parent->isRed()) {
                    now->parent->swapColor(now->getSibling());
                    return;
                }
                if (now->getSibling() != nullptr)now->getSibling()->setColor(RED);
                rebalanceErase(now->parent);
            }
            else if (pt == OUTBOARD) {
                now->getSibling()->setColor(now->parent->color), now->parent->setColor(BLACK);
                if (now->isLeft())now->parent->rotateLeft();
                else now->parent->rotateRight();
                now->parent->getSibling()->setColor(BLACK);
            }
            else { //INBOARD && BOTH
                if (now->isLeft())now->getSibling()->rotateRight(), now->parent->rotateLeft();
                else now->getSibling()->rotateLeft(), now->parent->rotateRight();
                now->parent->parent->setColor(now->parent->color), now->parent->setColor(BLACK);
            }
        }
        
        void RedBlackTreeErase(Node *now) {
            nodeNumber--;
            if (now->childNumber() == 2) {
                Node *su = now->findSuccessor();
                now->swapPos(su), now->swapColor(su);
            }
            if (now->childNumber() == 1) {
                if (now->left != nullptr) {
                    now->left->parent = now->parent;
                    now->left->setColor(BLACK);
                    now->selfFromParent() = now->left;
                }
                else {
                    now->right->parent = now->parent;
                    now->right->setColor(BLACK);
                    now->selfFromParent() = now->right;
                }
            }
            else { //now->childNumber() == 0
                if (!now->isRed())rebalanceErase(now);
                now->selfFromParent() = nullptr;
            }
            delete now;
        }
    
    public:
        class iterator {
            friend class map;
        
        public:
            Node *node = nullptr;
            
            bool isInvalid() const {
                return node == nullptr;
            }
        
        public:
            iterator() = default;
            
            iterator(const iterator &o) : node(o.node) {}
            
            explicit iterator(Node *o) : node(o) {}
            
            value_type *operator->() const {
                if (isInvalid() || node->isHeader())throw invalid_iterator();
                return node->value;
            }
        };
        
        map() : header(new Node) {
            header->parent = nullptr;
            header->left = header;
            header->right = header;
        }
        
        ~map() {
            recursionClear(header->parent);
            delete header;
        }
        
        iterator begin() {
            return iterator(header->left);
        }
        
        pair<iterator, bool> insert(const value_type &value) {
            Node *temp = findNode(value.first);
            if (temp != nullptr)return pair<iterator, bool>(iterator(temp), false);
            return pair<iterator, bool>(RedBlackTreeInsert(findInsertPos(value), value), true);
        }
    };
}

#endif
#include <iostream>
#include <cassert>
#include <string>

class Integer {
public:
	static int counter;
	int val;
	
	Integer(int val) : val(val) {
		counter++;
	}

	Integer(const Integer &rhs) {
		val = rhs.val;
		counter++;
	}

	Integer& operator = (const Integer &rhs) {
		assert(false);
	}

	~Integer() {
		counter--;
	}
};

int Integer::counter = 0;

class Compare {
public:
	bool operator () (const Integer &lhs, const Integer &rhs) const {
		return lhs.val < rhs.val;
	}
};

void tester(void) {
	sjtu::map<Integer, std::string, Compare> map;
	for (int i = 0; i < 100000; ++i) {
		std::string string = "";
		for (int number = i; number; number /= 10) {
			char digit = '0' + number % 10;
			string = digit + string;
		}
		if (i & 1) 
        {
			// map[Integer(i)] = string;
			auto result = map.insert(sjtu::pair<Integer, std::string>(Integer(i), string));
			// assert(!result.second);
		} 
        else 
        {
			auto result = map.insert(sjtu::pair<Integer, std::string>(Integer(i), string));
			assert(result.second);
		}
	}

	sjtu::map<Integer, std::string, Compare>::iterator tmp=map.begin();
	for (int time = 0; time <= 300; time++) {
		sjtu::map<Integer, std::string, Compare>::iterator iterator(tmp);
		for (int i = 1; i <= 100000; i++)
            // if (rand() & 1)
			    iterator++;
            // else
                // iterator.node->findSuccessor();
	}       

    std :: cout << counterr << std :: endl;
}


int main(void) {
    srand(19260817);
	std::ios::sync_with_stdio(false);
	std::cin.tie(0);
	std::cout.tie(0);
	tester();
	// std::cout << Integer::counter << std::endl;
}
