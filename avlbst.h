#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"
#include "print_bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:

    AVLTree();
    ~AVLTree();

    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO


protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);


    AVLNode<Key, Value>* insertAvlNode(AVLNode<Key, Value>* node, const std::pair<const Key, Value> &new_item);
    AVLNode<Key, Value>* removeAvlNode(AVLNode<Key, Value> *root, AVLNode<Key, Value> *&node);

    AVLNode<Key, Value>* leftLeftRotation(AVLNode<Key, Value>*);

    AVLNode<Key, Value>* rightRightRotation(AVLNode<Key, Value>*);

    AVLNode<Key, Value>* leftRightRotation(AVLNode<Key, Value>*);

    AVLNode<Key, Value>* rightLeftRotation(AVLNode<Key, Value>*);

    int max(int n1, int n2);

    int height(AVLNode<Key, Value>* node);
};


template<class Key, class Value>
AVLTree<Key, Value>::AVLTree() : BinarySearchTree<Key, Value>()
{
}

template<typename Key, typename Value>
AVLTree<Key, Value>::~AVLTree()
{
    this->clear();
}

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    auto* node = static_cast<AVLNode<Key, Value>*>(this->root_);
    this->root_ = insertAvlNode(node, new_item);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::insertAvlNode(AVLNode<Key, Value>* node, const std::pair<const Key, Value> &new_item) {
    if (node == nullptr) {
        node = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    } else if (new_item.first < node->getKey()) {
        AVLNode<Key, Value> *left = node->getLeft();
        AVLNode<Key, Value> *tmp = insertAvlNode(left, new_item);
        node->setLeft(tmp);
        tmp->setParent(node);

        if (height(node->getLeft()) - height(node->getRight()) == 2) {
            node = new_item.first < node->getLeft()->getKey() ? leftLeftRotation(node) : leftRightRotation(node);
        }
//        return node;
    } else if (new_item.first > node->getKey()) {
        AVLNode<Key, Value> *right = node->getRight();
        AVLNode<Key, Value> *tmp = insertAvlNode(right, new_item);
        node->setRight(tmp);
        tmp->setParent(node);

        if (height(node->getRight()) - height(node->getLeft()) == 2) {
            node = new_item.first > node->getRight()->getKey() ? rightRightRotation(node) : rightLeftRotation(node);
        }
    } else {
        node->setValue(new_item.second);
    }

    node->setBalance(max(height(node->getLeft()), height(node->getRight())) + 1);

    return node;
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    auto* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (node != nullptr) {
        this->root_ = removeAvlNode(static_cast<AVLNode<Key, Value>*>(this->root_), node);
    }
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::removeAvlNode(AVLNode<Key, Value> *root, AVLNode<Key, Value> *&node) {
    if (root == nullptr || node == nullptr) return nullptr;

    if (node->getKey() < root->getKey()) {
        AVLNode<Key, Value> *tmp = removeAvlNode(root->getLeft(), node);
        root->setLeft(tmp);
        if(tmp != nullptr) tmp->setParent(root);

        if (height(root->getRight()) - height(root->getLeft()) == 2) {
            AVLNode<Key, Value> *pAvlNode = root->getRight();
            root = height(pAvlNode->getLeft()) > height(pAvlNode->getRight()) ?
                   rightLeftRotation(root) : rightRightRotation(root);
        }
    } else if (node->getKey() > root->getKey()) {
        AVLNode<Key, Value> *tmp = removeAvlNode(root->getRight(), node);
        root->setRight(tmp);
        if(tmp != nullptr) tmp->setParent(root);

        if (height(root->getLeft()) - height(root->getRight()) == 2) {
            AVLNode<Key, Value> *pAvlNode = root->getLeft();
            root =  height(pAvlNode->getRight()) > height(pAvlNode->getLeft()) ?
                    leftRightRotation(root) : leftLeftRotation(root);
        }
    } else {
        if (root->getLeft() == nullptr && root->getRight() == nullptr ) {// 叶子节点
            delete root;
            return nullptr;
        }

        if (root->getLeft() && root->getRight()) {// 同时有左、右节点
            auto *pre = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(root));

            nodeSwap(root, pre);
            AVLNode<Key, Value> *tmp = root;
            root = pre;
            pre = tmp;

            if (root->getLeft() == pre) {
                root->setLeft(pre->getLeft());
                if (pre->getLeft()) {
                    pre->getLeft()->setParent(root);
                }
            } else {
                pre->getParent()->setRight(pre->getLeft());
                if (pre->getLeft()) {
                    pre->getLeft()->setParent(pre->getParent());
                }
            }
            delete pre;
        } else {// 仅又一个节点
            AVLNode<Key, Value> *tmp = root;
            if (root->getLeft()) {
                root = root->getLeft();
            } else {
                root = root->getRight();
            }
            delete tmp;
        }
    }

    return root;
}

template<typename Key, typename Value>
int AVLTree<Key, Value>::max(int n1, int n2) {
    return n1 > n2 ? n1 : n2;
}

template<typename Key, typename Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value>* node) {
    return node == nullptr ? 0 : node->getBalance();
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::leftLeftRotation(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* tmp;
    tmp = node->getLeft();
    node->setLeft(tmp->getRight());
    tmp->setRight(node);
    tmp->setParent(node->getParent());
    node->setParent(tmp);

    node->setBalance(max(height(node->getLeft()), height(node->getRight())) + 1);
    tmp->setBalance(max(height(tmp->getLeft()), node->getBalance()) + 1);

    return tmp;
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rightRightRotation(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* tmp;
    tmp = node->getRight();
    node->setRight(tmp->getLeft());
    tmp->setLeft(node);

    tmp->setParent(node->getParent());
    node->setParent(tmp);

    node->setBalance(max(height(node->getLeft()), height(node->getRight())) + 1);
    tmp->setBalance(max(height(tmp->getRight()), node->getBalance()) + 1);

    return tmp;
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::leftRightRotation(AVLNode<Key, Value>* node) {
    node->setLeft(rightRightRotation(node->getLeft()));
    return leftLeftRotation(node);
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rightLeftRotation(AVLNode<Key, Value>* node) {
    node->setRight(leftLeftRotation(node->getRight()));
    return rightRightRotation(node);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}
#endif
