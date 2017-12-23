#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

private:
    struct Node;
    class AVLTree;

    AVLTree tree;

public:
  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

  TreeMap()
  {}

  TreeMap(std::initializer_list<value_type> list)
  {
    for(auto&& entry : list) {
        tree.insert(entry.first, entry.second);
    }
  }

  TreeMap(const TreeMap& other)
  {
    for(ConstIterator it = other.cbegin(); it != other.cend(); ++it) {
        tree.insert((*it).first, (*it).second);
    }
  }

  TreeMap(TreeMap&& other)
  {
    this->tree.root = other.tree.root;
    this->tree.size = other.tree.size;

    other.tree.initAVLTree();
  }

  TreeMap& operator=(const TreeMap& other)
  {
    if(*this == other) return *this;

    tree.clear();
    tree.initAVLTree();

    for(ConstIterator it = other.cbegin(); it != other.cend(); ++it) {
        tree.insert((*it).first, (*it).second);
    }

    return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
    if(*this == other) return *this;

    this->tree.clear();
    this->tree.root = other.tree.root;
    this->tree.size = other.tree.size;

    other.tree.initAVLTree();

    return *this;
  }

  bool isEmpty() const
  {
    return tree.getSize() == 0;
  }

  mapped_type& operator[](const key_type& key)
  {
    return tree.insert(key)->value.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    Node *tmp = tree.findKey(key);
    if(tmp == nullptr)
        throw std::out_of_range("No such element");
    return tmp->value.second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    Node *tmp = tree.findKey(key);
    if(tmp == nullptr)
        throw std::out_of_range("No such element");
    return tmp->value.second;
  }

  const_iterator find(const key_type& key) const
  {
    Node *tmp = tree.findKey(key);
    if(tmp == nullptr) return cend();
    return ConstIterator(tmp, tree.getFirstNode());
  }

  iterator find(const key_type& key)
  {
    Node *tmp = tree.findKey(key);
    if(tmp == nullptr) return end();
    return Iterator(ConstIterator(tmp, tree.getFirstNode()));
  }

  void remove(const key_type& key)
  {
    tree.deleteKey(key);
  }

  void remove(const const_iterator& it)
  {
    tree.deleteKey(it->first);
  }

  size_type getSize() const
  {
    return tree.getSize();
  }

  bool operator==(const TreeMap& other) const
  {
    if(this->tree.getSize() != other.getSize()) return false;

     for(ConstIterator it = other.cbegin(); it != other.cend(); ++it) {
        ConstIterator cit = this->find(it->first);
        if(cit == cend() || cit->second != it->second) return false;
    }

    return true;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    return Iterator(cbegin());
  }

  iterator end()
  {
    return Iterator(cend());
  }

  const_iterator cbegin() const
  {
    return ConstIterator(tree.getFirstNode(), tree.getFirstNode());
  }

  const_iterator cend() const
  {
    return ConstIterator(tree.getLastNode(), tree.getFirstNode());
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename KeyType, typename ValueType>
struct TreeMap<KeyType, ValueType>::Node
{
    using key_type = typename TreeMap::key_type;
    using mapped_type = typename TreeMap::mapped_type;

    Node *left, *right, *parent;
    std::pair<const key_type, mapped_type> value;

    Node(const key_type key, mapped_type mapped_value) : value(key, mapped_value) {
        this->left = this->right = this->parent = nullptr;
    }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::AVLTree
{
public:
    Node* root;
    std::size_t size;

    using key_type = typename TreeMap::key_type;
    using mapped_type = typename TreeMap::mapped_type;
    using size_type = std::size_t;

    AVLTree() {
        root = new Node(key_type(), mapped_type()); //guard
        size = 0;
    }

    Node* insert(const key_type& key) {
        return insert(key, mapped_type());
    }

    Node* insert(const key_type& key, mapped_type mapped_value) {
        if(size == 0) {
            Node* node = new Node(key, mapped_value);
            node->parent = root; //root is a guard
            root->left = node;
            root = node;
            ++size;
            return root;
        }

        Node* current = root;
        while(true) {
            if(current->value.first == key)
                return current;

            if(current->value.first > key) {
                if(current->left == nullptr) {
                    current->left = new Node(key, mapped_value);
                    current->left->parent = current;
                    Node* created = current->left;
                    ++size;
                    rebalance(current);

                    return created;
                } else {
                    current = current->left;
                    continue;
                }
            } else {
                if(current->right == nullptr) {
                    current->right = new Node(key, mapped_value);
                    current->right->parent = current;
                    Node* created = current->right;
                    ++size;
                    rebalance(current);

                    return created;
                } else {
                    current = current->right;
                    continue;
                }
            }
        }
    }

    Node* findKey(const key_type& key) const {
        if(size == 0) return nullptr;
        Node* current = root;

        while(current != nullptr) {
            if(current->value.first > key)
                current = current->left;
            else if(current->value.first < key)
                current = current->right;
            else break;
        }

        return current;
    }

    void clear() {
        if(size == 0) deleteNode(root);
        else deleteNode(root->parent);

        size = 0;
        root = nullptr;
    }

    std::size_t getSize() const {
        return size;
    }

    Node* getFirstNode() const {
        Node *tmp = root;
        while(tmp->left != nullptr)
            tmp = tmp->left;

        return tmp;
    }

    Node* getLastNode() const {
        if(size == 0) return root;
        return root->parent;
    }

    void initAVLTree() {
        root = new Node(key_type(), mapped_type()); //set guard
        size = 0;
    }

    void deleteKey(const key_type& key) {
        Node *delNode = findKey(key);
        if(delNode == nullptr)
            throw std::out_of_range("No such an element");

        if(root->value.first == key && size == 1) {
            root = root->parent;
            delete root->left;
            root->left = nullptr;
            --size;
        } else if(delNode->left != nullptr && delNode->right != nullptr) {
            Node *successor = delNode->right;
            while(successor->left != nullptr)
                successor = successor->left;

            Node *tmp = delNode;
            delNode = new Node(successor->value.first, successor->value.second);
                delNode->parent = tmp->parent;
                delNode->left = tmp->left;
                delNode->right = tmp->right;
                if(root == tmp) root = delNode;

            if(tmp->parent->left == tmp)
                tmp->parent->left = delNode;
            else tmp->parent->right = delNode;
            delete delNode;

            if(successor->right != nullptr)
                successor->right->parent = successor->parent;

            if(successor->parent->left == successor)
                successor->parent->left = successor->right;
            else successor->parent->right = successor->right;

            if(successor->parent->parent != nullptr) rebalance(successor->parent);
            delete successor;
            --size;
        } else if(delNode->left != nullptr && delNode->right == nullptr) {
            delNode->left->parent = delNode->parent;

            if(delNode->parent->left == delNode)
                delNode->parent->left = delNode->left;
            else delNode->parent->right = delNode->left;

            if(delNode == root) root = delNode->left;
            if(delNode->parent->parent != nullptr) rebalance(delNode->parent);
            delete delNode;
            --size;
        } else if(delNode->left == nullptr && delNode->right != nullptr) {
            delNode->right->parent = delNode->parent;

            if(delNode->parent->left == delNode)
                delNode->parent->left = delNode->right;
            else delNode->parent->right = delNode->right;

            if(delNode == root) root = delNode->right;
            if(delNode->parent->parent != nullptr)  rebalance(delNode->parent);
            delete delNode;
            --size;
        } else { //leaf
             if(delNode->parent->left == delNode)
                delNode->parent->left = nullptr;
            else delNode->parent->right = nullptr;

            if(delNode->parent->parent != nullptr) rebalance(delNode->parent);
            delete delNode;
            --size;
        }

    }

    ~AVLTree() {
        clear();
    }

private:
    int height(Node* node) {
        if(node == nullptr) return 0;
        return 1 + std::max(height(node->left), height(node->right));
    }

    Node* rightRotation(Node* node) {
        Node* tmp = node->left;
        tmp->parent = node->parent;
        node->left = tmp->right;

        if(tmp->right != nullptr)
            tmp->right->parent = node;

        tmp->right = node;

        if(tmp->parent != nullptr) {
            if(tmp->parent->right == node)
                tmp->parent->right = tmp;
            else tmp->parent->left = tmp;
        }

        return tmp;
    }

    Node* leftRotation(Node* node) {
        Node* tmp = node->right;
        tmp->parent = node->parent;
        node->right = tmp->left;

        if(tmp->left != nullptr)
            tmp->left->parent = node;

        tmp->left = node;

        if(tmp->parent != nullptr) {
            if(tmp->parent->right == node)
                tmp->parent->right = tmp;
            else tmp->parent->left = tmp;
        }

        return tmp;
    }

    Node* leftRightRotation(Node* node) {
        node->left = leftRotation(node->left);
        return rightRotation(node);
    }

    Node* rightLeftRotation(Node* node) {
        node->right = rightRotation(node->right);
        return leftRotation(node);
    }

    void rebalance(Node* node) {
        if(node == nullptr) return;
        int balance = height(node->left) - height(node->right);

        if(balance == 2) {
            if(height(node->left->left) >= height(node->left->right))
                node = rightRotation(node);
            else node = leftRightRotation(node);

        } else if(balance == -2) {
            if(height(node->right->right) >= height(node->right->left))
                node = leftRotation(node);
            else node = rightLeftRotation(node);
        }

        if(node->parent->parent != nullptr) rebalance(node->parent);
        else root = node;
    }

    void deleteNode(Node* node) {
        if(node == nullptr) return;
        deleteNode(node->left);
        deleteNode(node->right);
        delete node;
    }

};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
    Node *current, *begin;

public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;

  explicit ConstIterator(){
    this->current = nullptr;
    this->begin = nullptr;
  }

  ConstIterator(Node *current, Node *begin) {
    this->current = current;
    this->begin = begin;
  }

  ConstIterator(const ConstIterator& other)
  {
    this->current = other.current;
    this->begin = other.begin;
  }

  ConstIterator& operator++()
  {
    if(current == nullptr || begin == nullptr)
        throw std::out_of_range("No access");

    if(current->parent == nullptr)
        throw std::out_of_range("No access");

    if(current->right != nullptr) {
        current = current->right;
        while(current->left != nullptr)
            current = current->left;

    } else {
        Node *parent = current->parent;
        while(parent->left != current) {
            current = parent;
            parent = current->parent;
        }
        current = parent;
    }

    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator it(*this);
    ++(*this);
    return it;
  }

  ConstIterator& operator--()
  {
    if(current == nullptr || begin == nullptr)
        throw std::out_of_range("No access");

    if(current == begin)
        throw std::out_of_range("No access");

    if(current->left != nullptr) {
        current = current->left;
        while(current->right != nullptr)
            current = current->right;
    } else {
        Node *parent = current->parent;
        while(parent->right != current) {
            current = parent;
            parent = current->parent;
        }
        current = parent;
    }

    return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator it(*this);
    --(*this);
    return it;
  }

  reference operator*() const
  {
    if(current == nullptr || begin == nullptr)
        throw std::out_of_range("No access");
    if(current->parent == nullptr)
        throw std::out_of_range("No access"); //end
    return current->value;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return (current == other.current && begin == other.begin);
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator()
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_MAP_H */
