#ifndef __HASH_POLICY_H__
#define __HASH_POLICY_H__

typedef uint32_t hashcode_t;

template <typename KeyT>
struct hash_policy
{
  static
  hashcode_t
  hash(const KeyT &key)
  {
    return static_cast<long>(key)
           * std::numeric_limits<hashcode_t>::max()
           / std::numeric_limits<long>::max();
  }
  static
  int
  key_compare(const KeyT &lhs, const KeyT &rhs)
  {
    if (lhs < rhs) return -1;
    if (lhs > rhs) return 1;
    return 0;
  }
  static
  bool
  key_empty(const KeyT &key)
  {
    return key == 0;
  }
};

template <>
struct hash_policy<std::string>
{
  static
  hashcode_t
  hash(const std::string &key)
  {
    std::string::const_iterator p = key.begin();
    hashcode_t hashcode = 0;
    for (;;)
    {
      hashcode_t byte = (unsigned char) *p++;
      if ( byte == '\0' ) break;
      hashcode = (hashcode << 5) - hashcode + byte;
    }
    return hashcode;
  }
  static
  int
  key_compare(const std::string &lhs, const std::string &rhs)
  {
    return lhs.compare(rhs);
  }
  static
  bool
  key_empty(const std::string &key)
  {
    return key.empty();
  }
};


#endif // __HASH_POLICY_H__
