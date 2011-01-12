#ifndef NOCOPY_H
#define NOCOPY_H

/*!
 * \brief Copy preventor class.
 *
 * This class is a helper to prevent copy construction
 * when derived from. It suffices to derive from this class
 * with private visibility.
 * This design is described in detail in Effective C++ Item 6.
 * To use it just do:
 * class MyClass : private NoCopy { ... }
 */
class NoCopy
{
protected:
    NoCopy() {}
    ~NoCopy() {}
private:
    NoCopy(const NoCopy&);
    NoCopy& operator=(const NoCopy&);
};

#endif // NOCOPY_H
