#ifndef FIBONACCI_HEAP_HPP
#define FIBONACCI_HEAP_HPP

#include <vector>
#include <list>
#include <math.h>

typedef long long int lli;
#define GOLDEN_RATIO_ROUND_DOWN 1.618

template <class V> class FibonacciHeap;

template<class T_NodeDesc>
class Element {
protected:
    Element<T_NodeDesc> *left = this;
    Element<T_NodeDesc> *right = this;
    Element<T_NodeDesc> *parent = nullptr;
    Element<T_NodeDesc> *child = nullptr;

    T_NodeDesc key;
    lli degree = 0;
    bool mark = false;

public:
    Element(T_NodeDesc key) : key(key) {};
    Element(Element<T_NodeDesc> *other) : key(other->key), degree(other->degree), mark(other->mark) {
        this->left = other->left;
        this->right = other->right;
        this->parent = other->parent;
        this->child = other->child;
    }

    T_NodeDesc getKey() { return this->key; }
    Element<T_NodeDesc> *getLeft() { return this->left; }
    Element<T_NodeDesc> *getRight() { return this->right; }
    Element<T_NodeDesc> *getChild() { return this->child; }
    Element<T_NodeDesc> *getParent() { return this->parent; }
    bool isMarked() { return this->mark; }
    lli getDegree() { return this->degree; }

    friend class FibonacciHeap<T_NodeDesc>;
};


template<class T_NodeDesc>
class FibonacciHeap {
private:
    // Copy constructor and assignment operator are not implemented
    // Hiding them to avoid misusage (Rule of three)
    // Since the point here is show the algorithmic part of the data structure
    FibonacciHeap(const FibonacciHeap<T_NodeDesc> &other);
    FibonacciHeap<T_NodeDesc> &operator=(const FibonacciHeap<T_NodeDesc> &other);

protected:
    Element<T_NodeDesc> *min;
    lli n;

    lli _D(lli n) {
        return log(n)/log(GOLDEN_RATIO_ROUND_DOWN);
    }

    void _deleteAll(Element<T_NodeDesc> *x) {
        if (x != nullptr) {
            Element<T_NodeDesc> *y = x;
            do {
                Element<T_NodeDesc> *z = y;
                y = y->right;
                this->_deleteAll(z->child);
                delete z;
            } while (y != x);
        }
    }

    Element<T_NodeDesc> *_unite(Element<T_NodeDesc> *x, Element<T_NodeDesc> *y) {
        if (x == nullptr) {
            return y;
        } else if (y == nullptr) {
            return x;
        } else if (x->key > y->key) {
            return this->_unite(y, x);
        } else {
            Element<T_NodeDesc> *xRight = x->left;
            Element<T_NodeDesc> *yRight = y->left;

            x->left = yRight;
            xRight->right = y;

            y->left = xRight;
            yRight->right = x;

            return x;
        }
    }

    void _link(Element<T_NodeDesc> *y, Element<T_NodeDesc> *x) {
        y->left->right = y->right;
        y->right->left = y->left;

        if (x->child == nullptr) {
            x->child = y;
            y->right = y;
            y->left = y;
        } else {
            Element<T_NodeDesc> *child = x->child;
            y->right = child;
            y->left = child->left;
            child->left->right = y;
            child->left = y;
        }

        y->parent = x;
        x->degree++;
        y->mark = false;
    }

    void _fillListWithElements(Element<T_NodeDesc> *x, std::list<Element<T_NodeDesc>*> &A) {
        Element<T_NodeDesc> *last = x;
        Element<T_NodeDesc> *w = last;
        do {
            w = w->right;
            A.push_back(w);
        } while (w != last);
    }

    void _consolidate() {
        lli D = this->_D(this->n);
        std::vector<Element<T_NodeDesc>*> A(D + 1, nullptr);

        std::list<Element<T_NodeDesc>*> elements;
        this->_fillListWithElements(this->min, elements);

        for (auto x : elements) {
            lli d = x->degree;

            while (A.at(d) != nullptr) {
                Element<T_NodeDesc> *y = A.at(d);

                if (x->key > y->key) {
                    std::swap(x, y);
                }

                this->_link(y, x);

                A.at(d) = nullptr;
                d++;
            }

            A.at(d) = x;
        }

        this->min = nullptr;
        for (lli i = 0; i < (lli)A.size(); i++) {
            if (A.at(i) != nullptr) {
                A.at(i)->right = A.at(i);
                A.at(i)->left = A.at(i);
                this->min = this->_unite(this->min, A.at(i));
            }
        }
    }

    void _cut(Element<T_NodeDesc> *x, Element<T_NodeDesc> *y) {
        x->left->right = x->right;
        x->right->left = x->left;

        if (y->child == x) {
            if (x->right == x) {
                y->child = nullptr;
            } else {
                y->child = x->right;
            }
        }

        y->degree--;

        x->left = x;
        x->right = x;
        x->parent = nullptr;
        x->mark = false;

        this->min = this->_unite(this->min, x);
    }

    void _cascadingCut(Element<T_NodeDesc> *y) {
        Element<T_NodeDesc> *z = y->parent;

        if (z != nullptr) {
            if (!y->mark) {
                y->mark = true;
            } else {
                this->_cut(y, z);
                this->_cascadingCut(z);
            }
        }
    }

public:
    FibonacciHeap() {
        this->min = nullptr;
        this->n = 0;
    };

    ~FibonacciHeap() {
        if (this->min != nullptr) {
            this->_deleteAll(this->min);
        }
    }

    bool isEmpty() const {
        return this->min == nullptr;
    }

    Element<T_NodeDesc> *insert(T_NodeDesc key) {
        Element<T_NodeDesc> *x = new Element<T_NodeDesc>(key);
        
        this->min = this->_unite(this->min, x);
        this->n++;

        return x;
    }

    void unite(FibonacciHeap<T_NodeDesc> *heap) {
        this->min = this->_unite(this->min, heap->min);
        this->n += heap->n;
        heap->min = nullptr;
        heap->n = 0;
    }

    T_NodeDesc getMin() const {
        return this->min->key;
    }

    T_NodeDesc extractMin() {
        Element<T_NodeDesc> *z = this->min;
        if (z == nullptr) {
            throw std::invalid_argument("heap is empty");
        }

        Element<T_NodeDesc> *x = z->child;
        if (x != nullptr) {
            Element<T_NodeDesc> *last = x;

            do {
                x = x->right;
                x->parent = nullptr;
            } while (x != last);

            this->min = this->_unite(this->min, x);
        }

        z->left->right = z->right;
        z->right->left = z->left;

        if (z == z->right) {
            this->min = nullptr;
        } else {
            this->min = z->right;
            this->_consolidate();
        }
        
        this->n--;
        T_NodeDesc min = z->key;
        delete z;
        return min;
    }

    void decreaseKey(Element<T_NodeDesc>* x, T_NodeDesc k) {
        if (k > x->key) {
            throw std::invalid_argument("new key is greater than current key");
        }

        x->key = k;
        Element<T_NodeDesc> *y = x->parent;

        if (y != nullptr && x->key < y->key) {
            this->_cut(x, y);
            this->_cascadingCut(y);
        }

        if (x->key < this->min->key) {
            this->min = x;
        }
    }

    void deleteElement(Element<T_NodeDesc> *x) {
        this->decreaseKey(x, std::numeric_limits<T_NodeDesc>::min());
        this->extractMin();
    }
};

#endif // FIBONACCI_HEAP_H
