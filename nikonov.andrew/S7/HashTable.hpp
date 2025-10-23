#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>
#include "HashTableIterator.hpp"

namespace nikonov
{
  template< typename Key, typename Value, typename Hash = std::hash< Key >, typename Equal = std::equal_to< Key > >
  class HashTable
  {
    friend class HashTableIterator< Key, Value, Hash, Equal >;
    friend class ConstHashTableIterator< Key, Value, Hash, Equal >;

  private:
    struct Slot
    {
      std::pair< Key, Value > data;
      bool occupied = false;
      bool deleted = false;

      Slot() = default;
      Slot(const Key& k, const Value& v) : data(k, v), occupied(true) {}
    };

    std::vector< Slot > table_;
    size_t size_;
    Hash hasher_;
    Equal equal_;
    float max_load_factor_ = 0.7f;

  public:
    using value_type = std::pair< Key, Value >;
    using iterator = HashTableIterator< Key, Value, Hash, Equal >;
    using const_iterator = ConstHashTableIterator< Key, Value, Hash, Equal >;

    HashTable();
    HashTable(const HashTable& other) = default;
    HashTable(HashTable&& other) noexcept = default;
    ~HashTable() = default;

    HashTable& operator=(const HashTable& other) = default;
    HashTable& operator=(HashTable&& other) noexcept = default;
 
    size_t size() const noexcept;
    bool empty() const noexcept;
    void clear() noexcept;
    void swap(HashTable& other) noexcept;

    Value& operator[](const Key& key);
    Value& at(const Key& key);
    const Value& at(const Key& key) const;

    bool insert(const Key& key, const Value& value);
    bool erase(const Key& key) noexcept;
    bool contains(const Key& key) const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    iterator find(const Key& key);
    const_iterator find(const Key& key) const;

    void rehash(size_t new_capacity);

  private:
    size_t findIndex(const Key& key) const;
    size_t findInsertPosition(const Key& key) const;
    void checkLoadFactor();
    size_t findNextOccupied(size_t start) const;  // Добавлен недостающий метод
  };

  template< typename Key, typename Value, typename Hash, typename Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable() :
    table_(16),
    size_(0)
  {}

  template< typename Key, typename Value, typename Hash, typename Equal >
  size_t HashTable< Key, Value, Hash, Equal >::size() const noexcept
  {
    return size_;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  bool HashTable< Key, Value, Hash, Equal >::empty() const noexcept
  {
    return size_ == 0;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  void HashTable< Key, Value, Hash, Equal >::clear() noexcept
  {
    table_.clear();
    table_.resize(16);
    size_ = 0;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  void HashTable< Key, Value, Hash, Equal >::swap(HashTable& other) noexcept
  {
    std::swap(table_, other.table_);
    std::swap(size_, other.size_);
    std::swap(hasher_, other.hasher_);
    std::swap(equal_, other.equal_);
    std::swap(max_load_factor_, other.max_load_factor_);
  }

  // Добавлен недостающий метод для итераторов
  template< typename Key, typename Value, typename Hash, typename Equal >
  size_t HashTable< Key, Value, Hash, Equal >::findNextOccupied(size_t start) const
  {
    for (size_t i = start; i < table_.size(); ++i)
    {
      if (table_[i].occupied)
      {
        return i;
      }
    }
    return table_.size();
  }

  // Методы итераторов
  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTable< Key, Value, Hash, Equal >::iterator HashTable< Key, Value, Hash, Equal >::begin()
  {
    return iterator(this, findNextOccupied(0));
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTable< Key, Value, Hash, Equal >::iterator HashTable< Key, Value, Hash, Equal >::end()
  {
    return iterator(this, table_.size());
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator HashTable< Key, Value, Hash, Equal >::begin() const
  {
    return const_iterator(this, findNextOccupied(0));
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator HashTable< Key, Value, Hash, Equal >::end() const
  {
    return const_iterator(this, table_.size());
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator HashTable< Key, Value, Hash, Equal >::cbegin() const
  {
    return const_iterator(this, findNextOccupied(0));
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator HashTable< Key, Value, Hash, Equal >::cend() const
  {
    return const_iterator(this, table_.size());
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTable< Key, Value, Hash, Equal >::iterator HashTable< Key, Value, Hash, Equal >::find(const Key& key)
  {
    size_t index = findIndex(key);
    return iterator(this, index);
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator HashTable< Key, Value, Hash, Equal >::find(const Key& key) const
  {
    size_t index = findIndex(key);
    return const_iterator(this, index);
  }

  // Остальные методы без изменений
  template< typename Key, typename Value, typename Hash, typename Equal >
  size_t HashTable< Key, Value, Hash, Equal >::findIndex(const Key& key) const
  {
    if (table_.empty()) return table_.size();

    size_t base_slot = hasher_(key) % table_.size();
    size_t current_slot = base_slot;
    size_t i = 1;

    while (i <= table_.size())
    {
      if (table_[current_slot].occupied && equal_(table_[current_slot].data.first, key))
      {
        return current_slot;
      }
      if (!table_[current_slot].occupied && !table_[current_slot].deleted)
      {
        break;
      }
      current_slot = (base_slot + i * i) % table_.size();
      ++i;
    }
    return table_.size();
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  size_t HashTable< Key, Value, Hash, Equal >::findInsertPosition(const Key& key) const
  {
    size_t base_slot = hasher_(key) % table_.size();
    size_t current_slot = base_slot;
    size_t i = 1;
    size_t first_deleted = table_.size();

    while (i <= table_.size())
    {
      if (table_[current_slot].occupied && equal_(table_[current_slot].data.first, key))
      {
        return current_slot;
      }
      if (!table_[current_slot].occupied)
      {
        if (table_[current_slot].deleted && first_deleted == table_.size())
        {
          first_deleted = current_slot;
        }
        else if (!table_[current_slot].deleted)
        {
          return current_slot;
        }
      }
      current_slot = (base_slot + i * i) % table_.size();
      ++i;
    }

    return (first_deleted != table_.size()) ? first_deleted : table_.size();
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  void HashTable< Key, Value, Hash, Equal >::checkLoadFactor()
  {
    if (static_cast<float>(size_) / table_.size() >= max_load_factor_)
    {
      rehash(table_.size() * 2);
    }
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  bool HashTable< Key, Value, Hash, Equal >::insert(const Key& key, const Value& value)
  {
    checkLoadFactor();

    size_t position = findInsertPosition(key);
    if (position == table_.size())
    {
      return false;
    }

    if (table_[position].occupied && equal_(table_[position].data.first, key))
    {
      return false;
    }

    table_[position].data = std::make_pair(key, value);
    table_[position].occupied = true;
    table_[position].deleted = false;
    ++size_;
    return true;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  Value& HashTable< Key, Value, Hash, Equal >::operator[](const Key& key)
  {
    size_t position = findInsertPosition(key);
    
    if (position != table_.size() && table_[position].occupied)
    {
      return table_[position].data.second;
    }

    checkLoadFactor();
    position = findInsertPosition(key);
    
    if (position == table_.size())
    {
      throw std::runtime_error("HashTable is full");
    }

    table_[position].data = std::make_pair(key, Value{});
    table_[position].occupied = true;
    table_[position].deleted = false;
    ++size_;
    return table_[position].data.second;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  Value& HashTable< Key, Value, Hash, Equal >::at(const Key& key)
  {
    size_t position = findIndex(key);
    if (position == table_.size())
    {
      throw std::out_of_range("Key not found");
    }
    return table_[position].data.second;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  const Value& HashTable< Key, Value, Hash, Equal >::at(const Key& key) const
  {
    size_t position = findIndex(key);
    if (position == table_.size())
    {
      throw std::out_of_range("Key not found");
    }
    return table_[position].data.second;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  bool HashTable< Key, Value, Hash, Equal >::erase(const Key& key) noexcept
  {
    size_t position = findIndex(key);
    if (position == table_.size())
    {
      return false;
    }

    table_[position].occupied = false;
    table_[position].deleted = true;
    --size_;
    return true;
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  bool HashTable< Key, Value, Hash, Equal >::contains(const Key& key) const
  {
    return findIndex(key) != table_.size();
  }

  template< typename Key, typename Value, typename Hash, typename Equal >
  void HashTable< Key, Value, Hash, Equal >::rehash(size_t new_capacity)
  {
    if (new_capacity < size_)
    {
      new_capacity = size_ * 2;
    }

    std::vector< Slot > new_table(new_capacity);

    for (const auto& slot: table_)
    {
      if (slot.occupied)
      {
        size_t new_position = findInsertPosition(slot.data.first);
        if (new_position != new_table.size())
        {
          new_table[new_position].data = slot.data;
          new_table[new_position].occupied = true;
          new_table[new_position].deleted = false;
        }
      }
    }
    table_.swap(new_table);
  }
}

#endif
