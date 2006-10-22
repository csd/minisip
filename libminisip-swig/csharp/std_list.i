// Downloaded from sourceforge http://sourceforge.net/tracker/index.php?func=detail&aid=1375891&group_id=1645&atid=301645
// Warning: Use the typemaps here in the expectation that the macros they are in will change name.

/*
 * SWIG typemaps for std::list
 * C# implementation
 * The C# wrapper is made to look and feel like a typesafe C# System.Collections.ArrayList
 * All the methods in IList are defined, but we don't derive from IList as this is a typesafe collection.
 * Warning: heavy macro usage in this file. Use swig -E to get a sane view on the real file contents!
 */

%include <std_common.i>

// MACRO for use within the std::list class body
// CSTYPE and CTYPE respectively correspond to the types in the cstype and ctype typemaps
%define SWIG_STD_LIST_MINIMUM(CSTYPE, CTYPE...)
%typemap(csinterfaces) std::list<CTYPE > "IDisposable, System.Collections.IEnumerable";
%typemap(cscode) std::list<CTYPE > %{
  public $csclassname(System.Collections.ICollection c) : this() {
    if (c == null)
      throw new ArgumentNullException("c");
    foreach (CSTYPE element in c) {
      this.Add(element);
    }
  }

  public bool IsFixedSize {
    get {
      return false;
    }
  }

  public bool IsReadOnly {
    get {
      return false;
    }
  }

  public CSTYPE this[int index]  {
    get {
      return getitem(index);
    }
  }

  public int Count {
    get {
      return (int)size();
    }
  }

  public bool IsSynchronized {
    get {
      return false;
    }
  }

  public void CopyTo(System.Array array) {
    CopyTo(0, array, 0, this.Count);
  }

  public void CopyTo(System.Array array, int arrayIndex) {
    CopyTo(0, array, arrayIndex, this.Count);
  }

  public void CopyTo(int index, System.Array array, int arrayIndex, int count) {
    if (array == null)
      throw new ArgumentNullException("array");
    if (index < 0)
      throw new ArgumentOutOfRangeException("index", "Value is less than zero");
    if (arrayIndex < 0)
      throw new ArgumentOutOfRangeException("arrayIndex", "Value is less than zero");
    if (count < 0)
      throw new ArgumentOutOfRangeException("count", "Value is less than zero");
    if (array.Rank > 1)
      throw new ArgumentException("Multi dimensional array.");
    if (index+count > this.Count || arrayIndex+count > array.Length)
      throw new ArgumentException("Number of elements to copy is too large.");
    for (int i=0; i<count; i++)
      array.SetValue(getitemcopy(index+i), arrayIndex+i);
  }

  // Type-safe version of IEnumerable.GetEnumerator
  System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() {
    return new $csclassnameEnumerator(this);
  }

  public $csclassnameEnumerator GetEnumerator() {
    return new $csclassnameEnumerator(this);
  }

  // Type-safe enumerator
  /// Note that the IEnumerator documentation requires an InvalidOperationException to be thrown
  /// whenever the collection is modified. This has been done for changes in the size of the
  /// collection but not when one of the elements of the collection is modified as it is a bit
  /// tricky to detect unmanaged code that modifies the collection under our feet.
  public sealed class $csclassnameEnumerator : System.Collections.IEnumerator {
    private $csclassname collectionRef;
    private int currentIndex;
    private object currentObject;
    private int currentSize;

    public $csclassnameEnumerator($csclassname collection) {
      collectionRef = collection;
      currentIndex = -1;
      currentObject = null;
      currentSize = collectionRef.Count;
    }

    // Type-safe iterator Current
    public CSTYPE Current {
      get {
        if (currentIndex == -1)
          throw new InvalidOperationException("Enumeration not started.");
        if (currentIndex > currentSize - 1)
          throw new InvalidOperationException("Enumeration finished.");
        if (currentObject == null)
          throw new InvalidOperationException("Collection modified.");
        return (CSTYPE)currentObject;
      }
    }

    // Type-unsafe IEnumerator.Current
    object System.Collections.IEnumerator.Current {
      get {
        return Current;
      }
    }

    public bool MoveNext() {
      int size = collectionRef.Count;
      bool moveOkay = (currentIndex+1 < size) && (size == currentSize);
      if (moveOkay) {
        currentIndex++;
        currentObject = collectionRef[currentIndex];
      } else {
        currentObject = null;
      }
      return moveOkay;
    }

    public void Reset() {
      currentIndex = -1;
      currentObject = null;
      if (collectionRef.Count != currentSize) {
        throw new InvalidOperationException("Collection modified.");
      }
    }
  }
%}

  public:
    typedef size_t size_type;
    %rename(Clear) clear;
    void clear();
    %rename(Add) push_back;
    void push_back(CTYPE value);
    size_type size() const;
    %newobject GetRange(int index, int count);
    %newobject Repeat(CTYPE value, int count);
    list();
    %extend {
      list() throw (std::out_of_range) {
        std::list<CTYPE >* pv = 0;
        pv = new std::list<CTYPE >();
       return pv;
      }
      CTYPE getitemcopy(int index) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size()) {
          std::list<CTYPE >::iterator it = (*self).begin();
          std::advance(it,index);
          return  (*it);
        }
        else
          throw std::out_of_range("index");
      }
      CTYPE getitem(int index) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size()) {
          std::list<CTYPE >::iterator it = (*self).begin();
          std::advance(it,index);
          return  (*it);
        }
        else
          throw std::out_of_range("index");
      }
      // Takes a deep copy of the elements unlike ArrayList.AddRange
      void AddRange(const std::list<CTYPE >& values) {
        self->insert(self->end(), values.begin(), values.end());
      }
      // Takes a deep copy of the elements unlike ArrayList.GetRange
      std::list<CTYPE > *GetRange(int index, int count) throw (std::out_of_range, std::invalid_argument) {
        if (index < 0)
          throw std::out_of_range("index");
        if (count < 0)
          throw std::out_of_range("count");
        if (index >= (int)self->size()+1 || index+count > (int)self->size())
          throw std::invalid_argument("invalid range");
        std::list<CTYPE >::iterator it=(*self).begin(), itEnd=(*self).begin();
        std::advance(it,index);
        std::advance(itEnd,index+count);
        return new std::list<CTYPE >(it, itEnd);
      }
      void Insert(int index, CTYPE value) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size()+1) {
          std::list<CTYPE >::iterator it = (*self).begin();
          std::advance(it,index);
          self->insert(it, value);
        }
        else
          throw std::out_of_range("index");
      }
      // Takes a deep copy of the elements unlike ArrayList.InsertRange
      void InsertRange(int index, const std::list<CTYPE >& values) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size()+1) {
          std::list<CTYPE >::iterator it = (*self).begin();
          std::advance(it,index);
          self->insert(it, values.begin(), values.end());
        }
        else
          throw std::out_of_range("index");
      }
      void RemoveAt(int index) throw (std::out_of_range) {
        if (index>=0 && index<(int)self->size()) {
          std::list<CTYPE >::iterator it = (*self).begin();
          std::advance(it,index);
          self->erase(it);
        }
        else
          throw std::out_of_range("index");
      }
      void RemoveRange(int index, int count) throw (std::out_of_range, std::invalid_argument) {
        if (index < 0)
          throw std::out_of_range("index");
        if (count < 0)
          throw std::out_of_range("count");
        if (index >= (int)self->size()+1 || index+count > (int)self->size())
          throw std::invalid_argument("invalid range");
        std::list<CTYPE >::iterator it=(*self).begin(), itEnd=(*self).begin();
        std::advance(it,index);
        std::advance(itEnd,index+count);
        self->erase(it, itEnd);
      }
      static std::list<CTYPE > *Repeat(CTYPE value, int count) throw (std::out_of_range) {
        if (count < 0)
          throw std::out_of_range("count");
        return new std::list<CTYPE >(count, value);
      }
      void Reverse() {
        self->reverse();
      }
      void Reverse(int index, int count) throw (std::out_of_range, std::invalid_argument) {
        if (index < 0)
          throw std::out_of_range("index");
        if (count < 0)
          throw std::out_of_range("count");
        if (index >= (int)self->size()+1 || index+count > (int)self->size())
          throw std::invalid_argument("invalid range");
        std::list<CTYPE >::iterator it=(*self).begin(), itEnd=(*self).begin();
        std::advance(it,index);
        std::advance(itEnd,index+count);
        std::reverse(it, itEnd);
      }
      // Takes a deep copy of the elements unlike ArrayList.SetRange
      void SetRange(int index, const std::list<CTYPE >& values) throw (std::out_of_range) {
        if (index < 0)
          throw std::out_of_range("index");
        if (index+values.size() > self->size())
          throw std::out_of_range("index");
        std::list<CTYPE >::iterator it = (*self).begin();
        std::advance(it,index);
        std::copy(values.begin(), values.end(), it);
      }
    }
%enddef

// Extra methods added to the collection class if operator== is defined for the class being wrapped
// CSTYPE and CTYPE respectively correspond to the types in the cstype and ctype typemaps
%define SWIG_STD_LIST_EXTRA_OP_EQUALS_EQUALS(CSTYPE, CTYPE...)
    %extend {
      bool Contains(CTYPE value) {
        return std::find(self->begin(), self->end(), value) != self->end();
      }
      int IndexOf(CTYPE value) {
        int index = -1;
        std::list<CTYPE >::iterator it = std::find(self->begin(), self->end(), value);
        if (it != self->end())
          index = std::distance( self->begin(), it );
        return index;
      }
      int LastIndexOf(CTYPE value) {
        int index = -1;
        std::list<CTYPE >::reverse_iterator rit = std::find(self->rbegin(), self->rend(), value);
        if (rit != self->rend())
          index = std::distance( self->rbegin(), rit );
        return index;
      }
      void Remove(CTYPE value) {
        std::list<CTYPE >::iterator it = std::find(self->begin(), self->end(), value);
        if (it != self->end())
          self->erase(it);
      }
    }
%enddef

// Macros for std::list class specializations
// CSTYPE and CTYPE respectively correspond to the types in the cstype and ctype typemaps
%define SWIG_STD_LIST_SPECIALIZE(CSTYPE, CTYPE...)
namespace std {
  template<> class list<CTYPE > {
    SWIG_STD_LIST_MINIMUM(CSTYPE, CTYPE)
    SWIG_STD_LIST_EXTRA_OP_EQUALS_EQUALS(CSTYPE, CTYPE)
  };
}
%enddef

%define SWIG_STD_LIST_SPECIALIZE_MINIMUM(CSTYPE, CTYPE...)
namespace std {
  template<> class list<CTYPE > {
    SWIG_STD_LIST_MINIMUM(CSTYPE, CTYPE)
  };
}
%enddef


%{
#include <list>
#include <algorithm>
#include <stdexcept>
%}

%csmethodmodifiers std::list::getitemcopy "private"
%csmethodmodifiers std::list::getitem "private"
%csmethodmodifiers std::list::size "private"

namespace std {
  // primary (unspecialized) class template for std::list
  // does not require operator== to be defined
  template<class T> class list {
    SWIG_STD_LIST_MINIMUM(T, T)
  };
}

// template specializations for std::list
// these provide extra collections methods as operator== is defined
SWIG_STD_LIST_SPECIALIZE(bool, bool)
SWIG_STD_LIST_SPECIALIZE(char, char)
SWIG_STD_LIST_SPECIALIZE(sbyte, signed char)
SWIG_STD_LIST_SPECIALIZE(byte, unsigned char)
SWIG_STD_LIST_SPECIALIZE(short, short)
SWIG_STD_LIST_SPECIALIZE(ushort, unsigned short)
SWIG_STD_LIST_SPECIALIZE(int, int)
SWIG_STD_LIST_SPECIALIZE(uint, unsigned int)
SWIG_STD_LIST_SPECIALIZE(int, long)
SWIG_STD_LIST_SPECIALIZE(uint, unsigned long)
SWIG_STD_LIST_SPECIALIZE(long, long long)
SWIG_STD_LIST_SPECIALIZE(ulong, unsigned long long)
SWIG_STD_LIST_SPECIALIZE(float, float)
SWIG_STD_LIST_SPECIALIZE(double, double)
SWIG_STD_LIST_SPECIALIZE(string, std::string) // also requires a %include "std_string.i"


