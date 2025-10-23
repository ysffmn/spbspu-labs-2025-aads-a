#ifndef HASHTABLEITERATOR_HPP
#define HASHTABLEITERATOR_HPP
#include <iterator>

namespace nikonov
{
  template< typename Key, typename Value, typename Hash, typename Equal >
  class HashTable;

  template< typename Key, typename Value, typename Hash, typename Equal >
  class HashTableIterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair< Key, Value >;
    using difference_type = std::ptrdiff_t;
    using pointer = std::pair< Key, Value >*;
    using reference = std::pair< Key, Value >&;
    using table_type = HashTable< Key, Value, Hash, Equal >;

    HashTableIterator();
    HashTableIterator(table_type* table, size_t index);
    HashTableIterator(const HashTableIterator& other) = default;
    ~HashTableIterator() = default;

    HashTableIterator& operator=(const HashTableIterator& other) = default;

    reference operator*() const;
    pointer operator->() const;

    HashTableIterator& operator++();
    HashTableIterator operator++(int);

    bool operator==(const HashTableIterator& other) const;
    bool operator!=(const HashTableIterator& other) const;

  private:
    table_type* table_;
    size_t index_;

    void advanceToNextOccupied();
  };

  template< typename Key, typename Value, typename Hash, typename Equal >
  HashTableIterator< Key, Value, Hash, Equal >::HashTableIterator() :
    table_(nullptr),
    index_(0)
  {}

  template< typename Key, typename Value, typename Hash, typename Equal >
  HashTableIterator< Key, Value, Hash, Equal >::HashTableIterator(table_type* table, size_t index) :
    table_(table),
    index_(index)
  {}

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTableIterator< Key, Value, Hash, Equal >::reference
  HashTableIterator< Key, Value, Hash, Equal >::operator*() const
  {
    return table_->table_[index_].data;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTableIterator< Key, Value, Hash, Equal >::pointer
  HashTableIterator< Key, Value, Hash, Equal >::operator->() const
  {
    return &table_->table_[index_].data;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  HashTableIterator< Key, Value, Hash, Equal >&
  HashTableIterator< Key, Value, Hash, Equal >::operator++()
  {
    if (table_ && index_ < table_->table_.size())
    {
      ++index_;
      advanceToNextOccupied();
    }
    return *this;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  HashTableIterator< Key, Value, Hash, Equal >
  HashTableIterator< Key, Value, Hash, Equal >::operator++(int)
  {
    HashTableIterator temp = *this;
    ++(*this);
    return temp;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  bool HashTableIterator< Key, Value, Hash, Equal >::operator==(const HashTableIterator& other) const
  {
    return table_ == other.table_ && index_ == other.index_;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  bool HashTableIterator< Key, Value, Hash, Equal >::operator!=(const HashTableIterator& other) const
  {
    return !(*this == other);
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  void HashTableIterator< Key, Value, Hash, Equal >::advanceToNextOccupied()
  {
    if (!table_) return;

    while (index_ < table_->table_.size() && !table_->table_[index_].occupied)
    {
      ++index_;
    }
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  class HashTableConstIterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const std::pair< Key, Value >;
    using difference_type = std::ptrdiff_t;
    using pointer = const std::pair< Key, Value >*;
    using reference = const std::pair< Key, Value >&;
    using table_type = const HashTable< Key, Value, Hash, Equal >;

    HashTableConstIterator();
    HashTableConstIterator(table_type* table, size_t index);
    HashTableConstIterator(const HashTableConstIterator& other) = default;
    ~HashTableConstIterator() = default;

    HashTableConstIterator& operator=(const HashTableConstIterator& other) = default;

    reference operator*() const;
    pointer operator->() const;

    HashTableConstIterator& operator++();
    HashTableConstIterator operator++(int);

    bool operator==(const HashTableConstIterator& other) const;
    bool operator!=(const HashTableConstIterator& other) const;

  private:
    table_type* table_;
    size_t index_;

    void advanceToNextOccupied();
  };

  template< typename Key, typename Value, typename Hash, typename Equal >
  HashTableConstIterator< Key, Value, Hash, Equal >::HashTableConstIterator() :
    table_(nullptr),
    index_(0)
  {}

  template< typename Key, typename Value, typename Hash, typename Equal >
  HashTableConstIterator< Key, Value, Hash, Equal >::HashTableConstIterator(table_type* table, size_t index) :
    table_(table),
    index_(index)
  {}

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTableConstIterator< Key, Value, Hash, Equal >::reference
  HashTableConstIterator< Key, Value, Hash, Equal >::operator*() const
  {
    return table_->table_[index_].data;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTableConstIterator< Key, Value, Hash, Equal >::pointer
  HashTableConstIterator< Key, Value, Hash, Equal >::operator->() const
  {
    return &table_->table_[index_].data;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  HashTableConstIterator< Key, Value, Hash, Equal >&
  HashTableConstIterator< Key, Value, Hash, Equal >::operator++()
  {
    if (table_ && index_ < table_->table_.size())
    {
      ++index_;
      advanceToNextOccupied();
    }
    return *this;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  HashTableConstIterator< Key, Value, Hash, Equal >
  HashTableConstIterator< Key, Value, Hash, Equal >::operator++(int)
  {
    HashTableConstIterator temp = *this;
    ++(*this);
    return temp;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  bool HashTableConstIterator< Key, Value, Hash, Equal >::operator==(const HashTableConstIterator& other) const
  {
    return table_ == other.table_ && index_ == other.index_;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  bool HashTableConstIterator< Key, Value, Hash, Equal >::operator!=(const HashTableConstIterator& other) const
  {
    return !(*this == other);
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  void HashTableConstIterator< Key, Value, Hash, Equal >::advanceToNextOccupied()
  {
    if (!table_) return;

    while (index_ < table_->table_.size() && !table_->table_[index_].occupied)
    {
      ++index_;
    }
  }
}

#endif
