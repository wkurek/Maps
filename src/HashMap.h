#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>
#include <iterator>
#include <functional>

#define BUCKETS_NUMBER 64000

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
    std::list<value_type> hashTable[BUCKETS_NUMBER];
    size_type size;

public:
  HashMap(){
    size = 0;
  }

  HashMap(std::initializer_list<value_type> list)
  {
    this->size = 0;
    for(auto pair : list) {
        (*this)[pair.first] = pair.second;
    }
  }

  HashMap(const HashMap& other)
  {
    this->size = 0;
    ConstIterator it = other.cbegin();
    for(;it != other.cend(); ++it) {
        (*this)[it->first] = it->second;
    }
  }

  HashMap(HashMap&& other)
  {
    this->size = other.size;
    for(size_type i = 0; i < BUCKETS_NUMBER; ++i) {
        this->hashTable[i] = std::move(other.hashTable[i]);
    }
  }

  HashMap& operator=(const HashMap& other)
  {
    if(*this == other) return *this;

    this->clear();

    this->size = 0;
    ConstIterator it = other.cbegin();
    for(;it != other.cend(); ++it) {
        (*this)[it->first] = it->second;
    }

    return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if(*this == other) return *this;

    this->size = other.size;
    for(size_type i = 0; i < BUCKETS_NUMBER; ++i) {
        this->hashTable[i] = std::move(other.hashTable[i]);
    }

    return *this;
  }

  bool isEmpty() const
  {
    return size == 0;
  }

  mapped_type& operator[](const key_type& key)
  {
    size_type index = std::hash<key_type>{}(key) % BUCKETS_NUMBER;

    typename std::list<value_type>::iterator it = hashTable[index].begin();
    for(; it != hashTable[index].end(); ++it) {
        if((*it).first == key) return (*it).second;
    }

    hashTable[index].push_back(std::make_pair(key, mapped_type()));
    ++size;
    return (*(--(hashTable[index].end()))).second;

  }

  const mapped_type& valueOf(const key_type& key) const
  {
    if(isEmpty()) throw std::out_of_range("No access");

    size_type index = std::hash<key_type>()(key) % BUCKETS_NUMBER;
    if(hashTable[index].empty())
        throw std::out_of_range("No access");

    typename std::list<value_type>::const_iterator it = hashTable[index].begin();
    for(; it != hashTable[index].end(); ++it) {
        if((*it).first == key) return (*it).second;
    }

    throw std::out_of_range("No access");
  }

  mapped_type& valueOf(const key_type& key)
  {
    if(isEmpty()) throw std::out_of_range("No access");

    size_type index = std::hash<key_type>()(key) % BUCKETS_NUMBER;
    if(hashTable[index].empty())
        throw std::out_of_range("No access");

    typename std::list<value_type>::iterator it = hashTable[index].begin();
    for(; it != hashTable[index].end(); ++it) {
        if(it->first == key) return it->second;
    }

    throw std::out_of_range("No access");
  }

  const_iterator find(const key_type& key) const
  {
    if(isEmpty()) return cend();

    size_type index = std::hash<KeyType>()(key) % BUCKETS_NUMBER;
    typename std::list<value_type>::const_iterator it = hashTable[index].begin();
    for(;it != hashTable[index].end(); ++it) {
        if((*it).first == key)
            return ConstIterator(index, it, hashTable);
    }

    return cend();
  }

  iterator find(const key_type& key)
  {
    if(isEmpty()) return end();

    size_type index = std::hash<key_type>()(key) % BUCKETS_NUMBER;
    typename std::list<value_type>::const_iterator it = hashTable[index].begin();
    for(;it != hashTable[index].end(); ++it) {
        if((*it).first == key)
            return Iterator(ConstIterator(index, it, hashTable));
    }

    return end();
  }

  void remove(const key_type& key)
  {
    ConstIterator it = find(key);
    if(it == cend())
        throw std::out_of_range("Cannot remove not existing element");

    hashTable[it.getIndex()].remove(*it);
    --size;
  }

  void remove(const const_iterator& it)
  {
    if(it == cend())
        throw std::out_of_range("Cannot remove not existing element");

    hashTable[it.getIndex()].remove(*it);
    --size;
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const HashMap& other) const
  {
    if(this->getSize() != other.getSize()) return false;
    ConstIterator it = other.cbegin();
    for(;it != other.cend(); ++it) {
        ConstIterator cit = this->find(it->first);
        if(cit == this->cend() || cit->second != it->second) return false;
    }

    return true;
  }

  bool operator!=(const HashMap& other) const
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
    if(isEmpty()) return ConstIterator(0, hashTable[0].begin(), hashTable);
    size_t i = 0;
    while(i < BUCKETS_NUMBER) {
        if(!hashTable[i].empty()) {
            return ConstIterator(i, hashTable[i].begin(), hashTable);
        }
        ++i;
    }
    return ConstIterator(0, hashTable[0].begin(), hashTable);
  }

  const_iterator cend() const
  {
    if(isEmpty()) return ConstIterator(0, hashTable[0].end(), hashTable);
    size_t i = BUCKETS_NUMBER - 1;
    while(i >= 0) {
        if(!hashTable[i].empty()) {
            return ConstIterator(i, hashTable[i].end(), hashTable);
        }
        --i;
    }
    return ConstIterator(0, hashTable[0].begin(), hashTable);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }

private:
    void clear() {
        for(size_type i = 0; i < BUCKETS_NUMBER; ++i) {
            if(!hashTable[i].empty()) {
                size_type number = hashTable[i].size();
                for(;number > 0; --number) {
                    hashTable[i].pop_back();
                }
            }
        }
    }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;

  std::size_t index;
  const std::list<value_type> *hashTable;
  typename std::list<value_type>::const_iterator it;

  explicit ConstIterator()
  {}

  ConstIterator(std::size_t index,typename std::list<value_type>::const_iterator it,
                const std::list<value_type> *hashTable) : index(index), hashTable(hashTable), it(it) {}

  ConstIterator(const ConstIterator& other)
  {
    this->index = other.index;
    this->it = other.it;
    this->hashTable = other.hashTable;
  }

  ConstIterator& operator++()
  {
    if(it == hashTable[index].end())
        throw std::out_of_range("No access");

    ++it;
    if(it == hashTable[index].end()) {
        std::size_t i = index + 1;
        while(i < BUCKETS_NUMBER) {
            if(!hashTable[i].empty()) {
                it = hashTable[i].begin();
                index = i;
                return *this;
            }

            ++i;
        }
    }

    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator cit(*this);
    ++(*this);
    return cit;
  }

  ConstIterator& operator--()
  {
    if(it != hashTable[index].begin()) {
        --it;
        return *this;
    }

    if(index == 0) throw std::out_of_range("No access");

    std::size_t i = index - 1;
    while(i >= 0) {
        if(!hashTable[i].empty()) {
            it = --(hashTable[i].end());
            index = i;
            return *this;
        }

        --i;
    }

    throw std::out_of_range("No access");
  }

  ConstIterator operator--(int)
  {
    ConstIterator cit(*this);
    --(*this);
    return cit;
  }

  reference operator*() const
  {
    if(it == hashTable[index].end())
        throw std::out_of_range("No access to last element");
    return *it;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return (this->index == other.index && this->it == other.it);
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }

  std::size_t getIndex() const { return index; }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

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

#endif /* AISDI_MAPS_HASHMAP_H */
