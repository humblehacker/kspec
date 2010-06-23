#ifndef __SHARED_PTR_H__
#define __SHARED_PTR_H__

// TODO: write test driver shared_ptr

#include <typeinfo>

class ReferenceCounted
{
public:
  void increment() { ++refcount; }
  void decrement() { if (--refcount == 0) delete this; }

protected:
  ReferenceCounted() : refcount(0) {}

private:
  unsigned refcount;
};

template <typename T>
class shared_ptr
{
public:
  shared_ptr(T *source=NULL) : pointee(source)
    { increment(); }
  shared_ptr(const shared_ptr &source)
    : pointee(const_cast<T*>(source.pointee))
    { increment(); }
  ~shared_ptr() { decrement(); }

  shared_ptr &operator=(const shared_ptr &source)
  {
    decrement();
    pointee = const_cast<T*>(source.pointee);
    increment();
    return *this;
  }

  void reset(T *source)         { *this = source; }
  T *release()                  { decrement(); T *r = pointee;
                                  pointee = NULL; return r; }
  T *get()                      { return  pointee; }
  const T *get() const          { return  pointee; }
  T *operator->() const         { return  pointee; }
  T &operator*()  const         { return *pointee; }
  bool is_null()         const  { return !pointee; }
  bool is_correct_type() const
    { return typeid(*pointee) == typeid(T); }

private:
  void increment() { if (pointee) pointee->increment(); }
  void decrement() { if (pointee) pointee->decrement(); }

  T *pointee;
};

#endif // __SHARED_PTR_H__
