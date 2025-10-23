#ifndef BINARYSEARCHTREE_HPP
#define BINARYSEARCHTREE_HPP
#include "BiTreeIterator.hpp"
#include "BiTreeNode.hpp"
#include <stdexcept>
#include <utility>
#include <queue>

namespace nikonov
{
  template< typename Key, typename Value, typename Compare = std::less< Key > >
  class BinarySearchTree
  {
  public:
    BinarySearchTree() noexcept;
    BinarySearchTree(const BinarySearchTree& rhs);
    BinarySearchTree(BinarySearchTree&& rhs) noexcept;
    ~BinarySearchTree();

    ConstBiTreeIterator< Key, Value > cbegin() const noexcept;
    ConstBiTreeIterator< Key, Value > cend() const noexcept;

    BinarySearchTree& operator=(const BinarySearchTree& other);
    BinarySearchTree& operator=(BinarySearchTree&& other) noexcept;

    size_t size() const noexcept;
    bool empty() const noexcept;

    Value& operator[](const Key& k);
    Value& at(const Key& k);
    const Value& at(const Key& k) const;

    void swap(BinarySearchTree& rhs) noexcept;
    void clear() noexcept;

    ConstBiTreeIterator< Key, Value > find(const Key& k) const;

    template < typename F >
    F traverse_lnr(F f) const;

    template < typename F >
    F traverse_rnl(F f) const;

    template < typename F >
    F traverse_breadth(F f) const;

  private:
    using value_type = std::pair< Key, Value >;
    detail::BiTreeNode< Key, Value >* root_;
    size_t size_;
    Compare cmp_;

    void clearNode(detail::BiTreeNode< Key, Value >* node);
    detail::BiTreeNode< Key, Value >* insertNode(detail::BiTreeNode< Key, Value >* node, const Key& key,
      const Value& value, detail::BiTreeNode< Key, Value >* parent, size_t& size);
    detail::BiTreeNode< Key, Value >* findNode(detail::BiTreeNode< Key, Value >* node, const Key& key) const;
    void copyTree(detail::BiTreeNode< Key, Value >*& node, detail::BiTreeNode< Key, Value >* otherNode, detail::BiTreeNode< Key, Value >* parent);

    detail::BiTreeNode< Key, Value >* findMin(detail::BiTreeNode< Key, Value >* node) const;

    template< typename F >
    F traverseLNRRecursive(detail::BiTreeNode< Key, Value >* node, F f) const;

    template< typename F >
    F traverseRNLRecursive(detail::BiTreeNode< Key, Value >* node, F f) const;
  };

  template< typename Key, typename Value, typename Compare >
  nikonov::BinarySearchTree< Key, Value, Compare >::BinarySearchTree() noexcept:
    root_(nullptr),
    size_(0),
    cmp_(Compare())
  {}

  template< typename Key, typename Value, typename Compare >
  nikonov::BinarySearchTree< Key, Value, Compare >::BinarySearchTree(const BinarySearchTree& other):
    root_(nullptr),
    size_(0),
    cmp_(other.cmp_)
  {
    try
    {
      copyTree(root_, other.root_, nullptr);
    }
    catch (std::bad_alloc&)
    {
      clear();
      throw;
    }
  }

  template< typename Key, typename Value, typename Compare >
  nikonov::BinarySearchTree< Key, Value, Compare >::BinarySearchTree(BinarySearchTree&& other) noexcept:
    root_(other.root_),
    size_(other.size_),
    cmp_(std::move(other.cmp_))
  {
    other.root_ = nullptr;
    other.size_ = 0;
  }

  template< typename Key, typename Value, typename Compare >
  nikonov::BinarySearchTree< Key, Value, Compare >::~BinarySearchTree()
  {
    clear();
  }

  template< typename Key, typename Value, typename Compare >
  nikonov::BinarySearchTree< Key, Value, Compare >& nikonov::BinarySearchTree< Key, Value, Compare >::operator=(const BinarySearchTree& other)
  {
    if (this == &other)
    {
      return *this;
    }
    BinarySearchTree temp(other);
    swap(temp);
    return *this;
  }

  template< typename Key, typename Value, typename Compare >
  nikonov::BinarySearchTree< Key, Value, Compare >& nikonov::BinarySearchTree< Key, Value, Compare >::operator=(BinarySearchTree&& other) noexcept
  {
    if (this == &other)
    {
      return *this;
    }
    clear();
    root_ = other.root_;
    size_ = other.size_;
    cmp_ = std::move(other.cmp_);
    other.root_ = nullptr;
    other.size_ = 0;
    return *this;
  }

  template< typename Key, typename Value, typename Compare >
  nikonov::ConstBiTreeIterator< Key, Value > nikonov::BinarySearchTree< Key, Value, Compare >::cbegin() const noexcept
  {
    return ConstBiTreeIterator< Key, Value >(findMin(root_));
  }

  template< typename Key, typename Value, typename Compare >
  nikonov::ConstBiTreeIterator< Key, Value > nikonov::BinarySearchTree< Key, Value, Compare >::cend() const noexcept
  {
    return ConstBiTreeIterator< Key, Value >(nullptr);
  }

  template< typename Key, typename Value, typename Compare >
  size_t nikonov::BinarySearchTree< Key, Value, Compare >::size() const noexcept
  {
    return size_;
  }

  template< typename Key, typename Value, typename Compare >
  bool nikonov::BinarySearchTree< Key, Value, Compare >::empty() const noexcept
  {
    return !size_;
  }

  template< typename Key, typename Value, typename Compare >
  Value& nikonov::BinarySearchTree< Key, Value, Compare >::operator[](const Key& k)
  {
    detail::BiTreeNode< Key, Value >* node = findNode(root_, k);
    if (!node)
    {
      root_ = insertNode(root_, k, Value(), nullptr, size_);
      node = findNode(root_, k);
    }
    return node->data.second;
  }

  template< typename Key, typename Value, typename Compare >
  Value& nikonov::BinarySearchTree< Key, Value, Compare >::at(const Key& k)
  {
    detail::BiTreeNode< Key, Value >* node = findNode(root_, k);
    if (!node)
    {
      throw std::out_of_range("Key not found");
    }
    return node->data.second;
  }

  template< typename Key, typename Value, typename Compare >
  const Value& nikonov::BinarySearchTree< Key, Value, Compare >::at(const Key& k) const
  {
    detail::BiTreeNode< Key, Value >* node = findNode(root_, k);
    if (!node)
    {
      throw std::out_of_range("Key not found");
    }
    return node->data.second;
  }

  template< typename Key, typename Value, typename Compare >
  void nikonov::BinarySearchTree< Key, Value, Compare >::swap(BinarySearchTree& rhs) noexcept
  {
    std::swap(root_, rhs.root_);
    std::swap(size_, rhs.size_);
    std::swap(cmp_, rhs.cmp_);
  }

  template< typename Key, typename Value, typename Compare >
  void nikonov::BinarySearchTree< Key, Value, Compare >::clear() noexcept
  {
    clearNode(root_);
    root_ = nullptr;
    size_ = 0;
  }

  template< typename Key, typename Value, typename Compare >
  nikonov::ConstBiTreeIterator< Key, Value > nikonov::BinarySearchTree< Key, Value, Compare >::find(const Key& k) const
  {
    detail::BiTreeNode< Key, Value >* node = findNode(root_, k);
    if (node == nullptr)
    {
      return cend();
    }
    return ConstBiTreeIterator< Key, Value >(node);
  }

  template< typename Key, typename Value, typename Compare >
  detail::BiTreeNode< Key, Value >* nikonov::BinarySearchTree< Key, Value, Compare >::findMin(detail::BiTreeNode< Key, Value >* node) const
  {
    if (node == nullptr)
    {
      return nullptr;
    }
    while (node->left != nullptr)
    {
      node = node->left;
    }
    return node;
  }

  template< typename Key, typename Value, typename Compare >
  void nikonov::BinarySearchTree< Key, Value, Compare >::clearNode(detail::BiTreeNode< Key, Value >* node)
  {
    if (!node)
    {
      return;
    }
    clearNode(node->left);
    clearNode(node->right);
    delete node;
  }

  template< typename Key, typename Value, typename Compare >
  detail::BiTreeNode< Key, Value >* nikonov::BinarySearchTree< Key, Value, Compare >::insertNode(
    detail::BiTreeNode< Key, Value >* node, const Key& key, const Value& value,
    detail::BiTreeNode< Key, Value >* parent, size_t& size)
  {
    if (node == nullptr)
    {
      ++size;
      detail::BiTreeNode< Key, Value >* newNode = new detail::BiTreeNode< Key, Value >(key, value);
      newNode->parent = parent;
      return newNode;
    }
    
    if (cmp_(key, node->data.first))
    {
      node->left = insertNode(node->left, key, value, node, size);
    }
    else if (cmp_(node->data.first, key))
    {
      node->right = insertNode(node->right, key, value, node, size);
    }
    return node;
  }

  template< typename Key, typename Value, typename Compare >
  detail::BiTreeNode< Key, Value >* nikonov::BinarySearchTree< Key, Value, Compare >::findNode(
    detail::BiTreeNode< Key, Value >* node, const Key& key) const
  {
    if (node == nullptr)
    {
      return nullptr;
    }
    
    if (cmp_(key, node->data.first))
    {
      return findNode(node->left, key);
    }
    else if (cmp_(node->data.first, key))
    {
      return findNode(node->right, key);
    }
    else
    {
      return node;
    }
  }

  template< typename Key, typename Value, typename Compare >
  void nikonov::BinarySearchTree< Key, Value, Compare >::copyTree(
    detail::BiTreeNode< Key, Value >*& node, detail::BiTreeNode< Key, Value >* otherNode,
    detail::BiTreeNode< Key, Value >* parent)
  {
    if (!otherNode)
    {
      node = nullptr;
      return;
    }
    node = new detail::BiTreeNode< Key, Value >(otherNode->data.first, otherNode->data.second);
    node->parent = parent;
    ++size_;
    try
    {
      copyTree(node->left, otherNode->left, node);
    }
    catch (std::bad_alloc&)
    {
      delete node;
      node = nullptr;
      --size_;
      throw;
    }
    try
    {
      copyTree(node->right, otherNode->right, node);
    }
    catch (std::bad_alloc&)
    {
      clearNode(node->left);
      delete node;
      node = nullptr;
      --size_;
      throw;
    }
  }
  template< typename Key, typename Value, typename Compare >
  template< typename F >
  F BinarySearchTree< Key, Value, Compare >::traverseLNRRecursive(detail::BiTreeNode< Key, Value >* node, F f) const
  {
    if (node == nullptr)
    {
      return f;
    }
    f = traverseLNRRecursive(node->left, f);
    f(node->data);
    f = traverseLNRRecursive(node->right, f);
    return f;
  }

  template< typename Key, typename Value, typename Compare >
  template< typename F >
  F BinarySearchTree< Key, Value, Compare >::traverse_lnr(F f) const
  {
    return traverseLNRRecursive(root_, f);
  }

  template< typename Key, typename Value, typename Compare >
  template< typename F >
  F BinarySearchTree< Key, Value, Compare >::traverse_rnl(F f) const
  {
    return traverseRNLRecursive(root_, f);
  }

  template< typename Key, typename Value, typename Compare >
  template< typename F >
  F BinarySearchTree< Key, Value, Compare >::traverse_breadth(F f) const
  {
    if (root_ == nullptr)
    {
      return f;
    }
    std::queue< detail::BiTreeNode< Key, Value >* > nodeQueue;
    nodeQueue.push(root_);
    while (!nodeQueue.empty())
    {
      detail::BiTreeNode< Key, Value >* current = nodeQueue.front();
      nodeQueue.pop();
      f(current->data);
      if (current->left != nullptr)
      {
        nodeQueue.push(current->left);
      }
      if (current->right != nullptr)
      {
        nodeQueue.push(current->right);
      }
    }
    return f;
  }

  template< typename Key, typename Value, typename Compare >
  template< typename F >
  F BinarySearchTree< Key, Value, Compare >::traverseRNLRecursive(detail::BiTreeNode< Key, Value >* node, F f) const
  {
    if (node == nullptr)
    {
      return f;
    }
    f = traverseRNLRecursive(node->right, f);
    f(node->data);
    f = traverseRNLRecursive(node->left, f);
    return f;
  }
}

#endif
