#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H
#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <queue>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "heap.h"
#include "heap_exception.h"

template <class TKey, class TValue>
class fibonacci_heap_creator;

template <class TKey, class TValue>
class fibonacci_heap : public heap<TKey, TValue>
{
private:
    //fibonacci_heap(comparator<TKey> *key_comparator);
    fibonacci_heap(Node<TKey, TValue>* node, comparator<TKey> *key_comparator);

    void update_min(Node<TKey, TValue> *node);
    Node<TKey, TValue>* unlink(Node<TKey, TValue> *node);
    void consolidate();
    Node<TKey, TValue> *find_minimum();
    void set_minimum(Node<TKey, TValue> *node);
    Node<TKey, TValue>* union_root(Node<TKey, TValue> *node_1, Node<TKey, TValue> *node_2);
public:
    fibonacci_heap(comparator<TKey> *key_comparator);
    ~fibonacci_heap();
    void make_heap(comparator<TKey> *key_comparator);
    void insert(const TKey key, const TValue value);
    TValue get_minimum();
    void remove_minimum();
    void union_heap(heap<TKey, TValue> &H_2);
    friend class fibonacci_heap_creator<TKey, TValue>;
};
template<class TKey, class TValue>
fibonacci_heap<TKey, TValue>::fibonacci_heap(comparator<TKey> *key_comparator)
{
    fibonacci_heap<TKey, TValue>::make_heap(key_comparator);
}
template<class TKey, class TValue>
fibonacci_heap<TKey, TValue>::fibonacci_heap(Node<TKey, TValue>* node, comparator<TKey> *key_comparator)
{
    this->heap_ptr = node;
    this->key_comparator = key_comparator;
}
template<class TKey, class TValue>
fibonacci_heap<TKey, TValue>::~fibonacci_heap()
{
    if (this->heap_ptr == nullptr)
    {
        return;
    }
    std::vector<Node<TKey, TValue>*> v {this->heap_ptr};

    while (!v.empty())
    {
        Node<TKey, TValue>* node = v.back();
        v.pop_back();
        if (node == nullptr)
        {
            continue;
        }
        Node<TKey, TValue>* right = node->right;
        while (right != node && right)
        {
            v.push_back(right->child);
            Node<TKey, TValue>* node_next = right->right;
            delete right;
            right = node_next;
        }
        v.push_back(node->child);
        delete node;
    }
}
template<class TKey, class TValue>
void fibonacci_heap<TKey, TValue>::make_heap(comparator<TKey> *key_comparator)
{
    this->key_comparator = key_comparator;
    this->heap_ptr = nullptr;
}
template<class TKey, class TValue>
void fibonacci_heap<TKey, TValue>::update_min(Node<TKey, TValue >*node)
{
    Node<TKey, TValue>* current = find_minimum();
    compare_t compare_result;
    compare_result = (*(this->key_comparator))(node->key, current->key);
    if (current == nullptr)
    {
        set_minimum(node);
    }
    else if ((node != NULL) && (compare_result == LESS))
    {
        set_minimum(node);
    }
}
template<class TKey, class TValue>
Node<TKey, TValue>* fibonacci_heap<TKey, TValue>::unlink(Node<TKey, TValue> *node)
{
    Node<TKey, TValue>* left = node->left;
    Node<TKey, TValue>* right = node->right;
    if (left == nullptr)
    {
        return nullptr;
    }

    if (left == right)
    {
        left->left = nullptr;
        left->right = nullptr;
    }
    else
    {
        left->right = right;
        right->left = left;
    }

    return left;
}
template<class TKey, class TValue>
void fibonacci_heap<TKey, TValue>::consolidate()
{
    Node<TKey, TValue>* root = find_minimum();
    if (root == nullptr)
    {
        return;
    }
    std::unordered_map<int, Node<TKey, TValue>*> ranked;
    ranked[root->degree] = root;
    root->parent = nullptr;
    Node<TKey, TValue>* node = root->right;
    while (node != nullptr)
    {
        node->parent = nullptr;
        Node<TKey, TValue>* melded = node;
        node = node->right;
        auto got = ranked.find(node->degree);
        if ((got != ranked.end()) && (got->second == node))
        {
            node = nullptr;
        }
        while (ranked.find(melded->degree) != ranked.end())
        {
            int rank = melded->degree;
            melded = union_root(melded, ranked[rank]);
            ranked.erase(rank);
        }
        ranked[melded->degree] = melded;
        update_min(melded);
    }
}
template<class TKey, class TValue>
Node<TKey, TValue> *fibonacci_heap<TKey, TValue>::find_minimum()
{
    return this->heap_ptr;
}
template<class TKey, class TValue>
void fibonacci_heap<TKey, TValue>::set_minimum(Node<TKey, TValue> *node)
{
    this->heap_ptr = node;
}
template<class TKey, class TValue>
Node<TKey, TValue>* fibonacci_heap<TKey, TValue>::union_root(Node<TKey, TValue> *_node1, Node<TKey, TValue> *_node2)
{
    Node<TKey, TValue>* node1 = _node1;
    Node<TKey, TValue>* node2 = _node2;
    compare_t compare_result;
    compare_result = (*(this->key_comparator))(_node1->key, _node2->key);
    if (compare_result == GREAT)
    {
        node1 = _node2;
        node2 = _node1;
    }
    unlink(node2);
    node2->parent = nullptr;
    node2->left = nullptr;
    node2->right = nullptr;;
    node2->marked = false;
    node2->parent = node1;
    node1->degree++;
    Node<TKey, TValue>* child = node1->child;
    if (child == nullptr)
    {
        node1->child = node2;
    }
    else
    {
        Node<TKey, TValue>* left = child->left;
        if (left == nullptr)
        {
            child->left = node2;
            child->right = node2;
            node2->left = child;
            node2->right = child;
        }
        else
        {
            node2->left = left;
            node2->right = child;
            left->right = node2;
            child->left = node2;
        }
    }
    return node1;
}
template<class TKey, class TValue>
void fibonacci_heap<TKey, TValue>::insert(const TKey key, const TValue value)
{
    if(std::find(this->keys.begin(), this->keys.end(), key) != this->keys.end())
    {
        throw typename heap<TKey, TValue>::insert_error_exception(key);
    }
    this->keys.insert(key);
    fibonacci_heap<TKey, TValue>* h_2 = new fibonacci_heap<TKey, TValue>(this->key_comparator);
    Node<TKey, TValue> *node = new Node<TKey, TValue>();
    node->key = key;
    node->value = value;
    h_2->heap_ptr = node;
    union_heap(*h_2);
    h_2->heap_ptr = nullptr;
    delete h_2;
}
template<class TKey, class TValue>
TValue fibonacci_heap<TKey, TValue>::get_minimum()
{
    if(this->heap_ptr == nullptr)
    {
        throw typename heap<TKey, TValue>::get_minimum_error_exception();
    }
    return this->heap_ptr->value;
}
template<class TKey, class TValue>
void fibonacci_heap<TKey, TValue>::remove_minimum()
{
    if(this->heap_ptr == nullptr)
    {
        throw typename heap<TKey, TValue>::get_minimum_error_exception();
    }
    Node<TKey, TValue>* root = find_minimum();
    set_minimum(root->left);
    unlink(root);
    fibonacci_heap* h = new fibonacci_heap(root->child, this->key_comparator);
    union_heap(*h);
    h->set_minimum(nullptr);
    delete h;
    consolidate();
    root->parent = nullptr;
    root->right = nullptr;
    root->left = nullptr;
    root->child = nullptr;
    typename std::set<TKey>::iterator key_iter = std::find(this->keys.begin(), this->keys.end(), root->key);
    this->keys.erase(key_iter);
}
template<class TKey, class TValue>
void fibonacci_heap<TKey, TValue>::union_heap(heap<TKey, TValue> &H_2)
{
    size_t old_size = this->keys.size() + H_2.keys.size();
    std::set<TKey> union_keys;
    union_keys.insert(this->keys.begin(), this->keys.end());
    union_keys.insert(H_2.keys.begin(), H_2.keys.end());
    if(old_size != union_keys.size())
    {
      throw typename heap<TKey, TValue>::union_heap_error_exception();
    }
    Node<TKey, TValue> *node_1 = this->heap_ptr;
    Node<TKey, TValue> *node_2 = H_2.heap_ptr;
    if (node_2 ==nullptr)
    {
        return;
    }
    if (node_1 == nullptr)
    {
        this->heap_ptr = node_2;
        return;
    }
    Node<TKey, TValue> *left_1 = node_1->left;
    Node<TKey, TValue> *left_2 = node_2->left;
    if (left_1 == nullptr)
    {
        if (left_2 != nullptr)
        {
            node_1->left = left_2;
            node_1->right = node_2;
            left_2->right = node_1;
            node_2->left = node_1;
        }
        else
        {
            node_1->left = node_2;
            node_1->right = node_2;
            node_2->left = node_1;
            node_2->right = node_1;
        }
    }
    else
    {
        if (left_2 != nullptr)
        {
            left_1->right = node_2;
            node_1->left = left_2;
            left_2->right = node_1;
            node_2->left = left_1;
        }
        else
        {
            node_2->left = left_1;
            node_2->right = node_1;
            left_1->right = node_2;
            node_1->left = node_2;
        }
    }
    update_min(node_2);
    H_2.heap_ptr = nullptr;
}

template <class TKey, class TValue>
class fibonacci_heap_creator : public heap_creator<TKey, TValue>
{
public:
    fibonacci_heap<TKey, TValue>* create(comparator<TKey> *key_comparator) override
    {
        fibonacci_heap<TKey, TValue> *result = new fibonacci_heap<TKey, TValue>(key_comparator);
        return result;
    }
    ~fibonacci_heap_creator()
    {

    }
};
#endif // FIBONACCI_HEAP_H
