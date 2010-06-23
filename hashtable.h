#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <set>
#include <string>
#include <memory>
#include <iostream>
#include <cassert>

#include "hash_policy.h"

template <typename KeyT, typename ValueT, typename HashT>
struct HashTableEntry;

class HashTableBase
{
protected:
  static const double max_load_factor;
};

template <typename KeyT,
          typename ValueT=KeyT,
          typename HashT=hash_policy<KeyT> >
class HashTable : public HashTableBase
{
public:
  HashTable(unsigned initial_capacity=31);
  ~HashTable();

  typedef HashTableEntry<KeyT, ValueT, HashT> entry_type;
  typedef KeyT                                key_type;
  typedef ValueT                              value_type;

  entry_type *lookup(const KeyT & key);
  entry_type *intern(const KeyT & key);
  const ValueT &operator[](const KeyT & key) const
                               { return intern(key)->value(); }
  ValueT &operator[](const KeyT & key)
                               { return intern(key)->value(); }
  unsigned capacity()    const { return table->capacity; }
  unsigned load()        const { return table->load; }
  double   load_factor() const { return (double)table->load /
                                        table->capacity; }
  bool     empty()       const { return table->load == 0; }
  void     grow();

  template <class Visitor>
  void apply(Visitor &visit) const;

private:
  struct Table;
  typedef std::auto_ptr<Table> TablePtr;
  TablePtr table;
};

template <typename KeyT, typename ValueT, typename HashT>
struct HashTableEntry
{
  typedef KeyT                                key_type;
  typedef ValueT                              value_type;

  HashTableEntry(const KeyT & key, hashcode_t hashcode)
  : key(key), hashcode(hashcode), next(NULL) {}
  bool operator == (const HashTableEntry & rhs)
    { return this == &rhs ||
      (hashcode == rhs.hashcode && key == rhs.key); }
  bool operator != (const HashTableEntry & rhs)
    { return !operator==(rhs); }

  const ValueT & value() const { return value_; }
  ValueT & value()             { return value_; }

  KeyT            key;
  ValueT          value_;
  hashcode_t      hashcode;
  HashTableEntry *next;
  friend class HashTable<KeyT, ValueT, HashT>;
};

template <typename KeyT, typename HashT>
struct HashTableEntry<KeyT, void, HashT>
{
  HashTableEntry(const KeyT & key, hashcode_t hashcode)
  : key(key), hashcode(hashcode), next(NULL) {}
  bool operator == (const HashTableEntry & rhs)
    { return this == &rhs ||
      (hashcode == rhs.hashcode && key == rhs.key); }
  bool operator != (const HashTableEntry & rhs)
    { return !operator==(rhs); }

  const KeyT & value() const { return key; }

  KeyT        key;
  hashcode_t  hashcode;
  HashTableEntry      *next;
  friend class HashTable<KeyT, void, HashT>;
};

template <typename KeyT, typename ValueT, typename HashT>
std::ostream &
operator<<(std::ostream & os,
           const typename HashTable<KeyT, ValueT, HashT>::Entry & rhs)
{
  os << "(" << rhs.key << ")";
  return os;
}

template <typename KeyT, typename ValueT, typename HashT>
struct HashTable<KeyT, ValueT, HashT>::Table
{
  Table(unsigned capacity);
  ~Table();
  entry_type *lookup_entry(const KeyT & key,
                           hashcode_t hashcode,
                           entry_type *&insert_after);
  entry_type *insert_entry(entry_type *entry,
                           entry_type *insert_after);

  entry_type **entries;
  unsigned     capacity;
  unsigned     load;
};

template <typename KeyT, typename ValueT, typename HashT>
HashTable<KeyT, ValueT, HashT>::Table::
Table(unsigned cap) : capacity(cap), load(0)
{
  entries = new entry_type*[capacity];
  memset(entries, 0, capacity*sizeof(entry_type*));
}

template <typename KeyT, typename ValueT, typename HashT>
HashTable<KeyT, ValueT, HashT>::Table::
~Table()
{
  delete [] entries;
}

template <typename KeyT, typename ValueT, typename HashT>
typename HashTable<KeyT, ValueT, HashT>::entry_type *
HashTable<KeyT, ValueT, HashT>::Table::
insert_entry(entry_type *entry, entry_type *insert_after)
{
  if ( insert_after == NULL )
  {
    unsigned index = entry->hashcode % capacity;
    entry->next = entries[index];
    entries[index] = entry;
  }
  else
  {
    entry->next = insert_after->next;
    insert_after->next = entry;
  }
  ++load;
  return entry;
}

template <typename KeyT, typename ValueT, typename HashT>
typename HashTable<KeyT, ValueT, HashT>::entry_type *
HashTable<KeyT, ValueT, HashT>::Table::
lookup_entry(const KeyT & key, hashcode_t hashcode,
             entry_type *&insert_after)
{
  // Lookup entry, return if found.
  insert_after = NULL;
  entry_type *entry = entries[hashcode % capacity];
  while ( entry != NULL )
  {
    int result = HashT::key_compare(entry->key, key);
    if ( result == 0 ) return entry;
    if ( result >  0 ) break;
    insert_after = entry;
    entry = entry->next;
  }
  return NULL;
}

template <typename KeyT, typename ValueT, typename HashT>
HashTable<KeyT, ValueT, HashT>::
HashTable(unsigned initial_capacity)
: table(new Table(initial_capacity))
{
}

template <typename KeyT, typename ValueT, typename HashT>
HashTable<KeyT, ValueT, HashT>::
~HashTable()
{
}

template <typename KeyT, typename ValueT, typename HashT>
template <typename Visitor>
void
HashTable<KeyT, ValueT, HashT>::
apply(Visitor &visit) const
{
  const entry_type *entry = NULL;
  for( unsigned i = 0; i < table->capacity; ++i )
  {
    entry = table->entries[i];
    while ( entry != NULL )
    {
      visit(*entry);
      entry = entry->next;
    }
  }
}

template <typename KeyT, typename ValueT, typename HashT>
typename HashTable<KeyT, ValueT, HashT>::entry_type *
HashTable<KeyT, ValueT, HashT>::
lookup(const KeyT &key)
{
  entry_type *insert_after = NULL;
  hashcode_t hashcode = HashT::hash(key);
  return table->lookup_entry(key, hashcode, insert_after);
}

template <typename KeyT, typename ValueT, typename HashT>
typename HashTable<KeyT, ValueT, HashT>::entry_type *
HashTable<KeyT, ValueT, HashT>::
intern(const KeyT &key)
{
  if ( HashT::key_empty(key) )
    return NULL;

  // maybe grow table
  if (load_factor() > max_load_factor)
    grow();

  hashcode_t hashcode = HashT::hash(key);
  entry_type *insert_after = NULL;
  entry_type *entry =
    table->lookup_entry(key, hashcode, insert_after);
  if ( entry ) return entry;
  return table->insert_entry(new entry_type(key, hashcode),
                             insert_after);
}

template <typename KeyT, typename ValueT, typename HashT>
void
HashTable<KeyT, ValueT, HashT>::
grow()
{
  // create new table
  TablePtr new_table(new Table(table->capacity * 2 - 1));

  // move entries from old table to new
  entry_type *insert_after = NULL;
  entry_type *entry = NULL, *next_entry = NULL;;
  for (unsigned i = 0; i < table->capacity; ++i )
  {
    entry = table->entries[i];
    while ( entry != NULL )
    {
      next_entry = new_table->lookup_entry(entry->key,
                                           entry->hashcode,
                                           insert_after);
      assert(next_entry == NULL);
      // _next pointer will be overwritten; save it.
      next_entry = entry->next;
      new_table->insert_entry(entry, insert_after);
      entry = next_entry;
    }
  }

  // replace old table with new table
  table = new_table;
}

#endif // __HASHTABLE_H__
