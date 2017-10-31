/* ========================================================================== */
/* REFERENCED.H -- 参考头文件
 *
 * REFERENCE: 
 *
 * COPYRIGHT 2017 Optixera.
 * -------------------------------------------------------------------------- */
/* $REVISION: 1.00$ (20171001 Chaohong Liu) */
/* HISTORY: 20171001(v1.00) Initial Version;
 * ========================================================================== */


#ifndef _REFERENCED_H
#define _REFERENCED_H

#ifdef WIN32
#include <windows.h>
#endif
#pragma pack(4)//设定为4字节对齐 
class Referenced
{
    public:
        Referenced():_refCount(0){}
        Referenced(const Referenced&):_refCount(0){}
        inline Referenced& operator = (const Referenced&) { return *this; }

        /** Increment the reference count by one, indicating that 
            this object has another pointer which is referencing it.*/
        void ref() const
        {
#ifdef WIN32
            InterlockedIncrement(&_refCount);
#else
            _refCount++;
#endif
        }
        
        /** Decrement the reference count by one, indicating that 
            a pointer to this object is referencing it.  If the
            reference count goes to zero, it is assumed that this object
            is no longer referenced and is automatically deleted.*/
        inline void unref() const
        {
            bool needDelete = false;
#ifdef WIN32
            InterlockedDecrement(&_refCount);
#else
            _refCount--;
#endif
            needDelete = _refCount <= 0;

            if (needDelete)
                delete this;
        }
        
        /** Return the number pointers currently referencing this object. */
        inline int referenceCount() const { return _refCount; }

       
    protected:
        virtual ~Referenced(){}

private:
        mutable long  _refCount;
};


/** Smart pointer for handling referenced counted objects.*/
template<class T>
class ref_ptr
{
    public:
        typedef T element_type;

        ref_ptr() : _ptr(0) {}
        ref_ptr(T* ptr) : _ptr(ptr) { if (_ptr) _ptr->ref(); }
        ref_ptr(const ref_ptr& rp) : _ptr(rp._ptr) { if (_ptr) _ptr->ref(); }
        template<class Other> ref_ptr(const ref_ptr<Other>& rp) : _ptr(rp._ptr) { if (_ptr) _ptr->ref(); }

        ~ref_ptr() { if (_ptr) _ptr->unref();  _ptr = 0; }

        ref_ptr& operator = (const ref_ptr& rp)
        {
            assign(rp);
            return *this;
        }

        template<class Other> ref_ptr& operator = (const ref_ptr<Other>& rp)
        {
            assign(rp);
            return *this;
        }

        inline ref_ptr& operator = (T* ptr)
        {
            if (_ptr==ptr) return *this;
            T* tmp_ptr = _ptr;
            _ptr = ptr;
            if (_ptr) _ptr->ref();
            // unref second to prevent any deletion of any object which might
            // be referenced by the other object. i.e rp is child of the
            // original _ptr.
            if (tmp_ptr) tmp_ptr->unref();
            return *this;
        }

        // comparison operators for ref_ptr.
        bool operator == (const ref_ptr& rp) const { return (_ptr==rp._ptr); }
        bool operator == (const T* ptr) const { return (_ptr==ptr); }
        friend bool operator == (const T* ptr, const ref_ptr& rp) { return (ptr==rp._ptr); }

        bool operator != (const ref_ptr& rp) const { return (_ptr!=rp._ptr); }
        bool operator != (const T* ptr) const { return (_ptr!=ptr); }
        friend bool operator != (const T* ptr, const ref_ptr& rp) { return (ptr!=rp._ptr); }
        bool operator < (const ref_ptr& rp) const { return (_ptr<rp._ptr); }

    public:
        T& operator*() const { return *_ptr; }
        T* operator->() const { return _ptr; }
        T* get() const { return _ptr; }

        bool operator!() const   { return _ptr==0; } // not required
        bool valid() const       { return _ptr!=0; }

        T* release() { T* tmp=_ptr; if (_ptr) _ptr->unref(); _ptr=0; return tmp; }

        void swap(ref_ptr& rp) { T* tmp=_ptr; _ptr=rp._ptr; rp._ptr=tmp; }

    private:

        template<class Other> void assign(const ref_ptr<Other>& rp)
        {
            if (_ptr==rp._ptr) return;
            T* tmp_ptr = _ptr;
            _ptr = rp._ptr;
            if (_ptr) _ptr->ref();
            // unref second to prevent any deletion of any object which might
            // be referenced by the other object. i.e rp is child of the
            // original _ptr.
            if (tmp_ptr) tmp_ptr->unref();
        }

        template<class Other> friend class ref_ptr;

        T* _ptr;
};
#pragma pack()//恢复对齐状态

template<class T> inline
void swap(ref_ptr<T>& rp1, ref_ptr<T>& rp2) { rp1.swap(rp2); }

template<class T> inline
T* get_pointer(const ref_ptr<T>& rp) { return rp.get(); }

template<class T, class Y> inline
ref_ptr<T> static_pointer_cast(const ref_ptr<Y>& rp) { return static_cast<T*>(rp.get()); }

template<class T, class Y> inline
ref_ptr<T> dynamic_pointer_cast(const ref_ptr<Y>& rp) { return dynamic_cast<T*>(rp.get()); }

template<class T, class Y> inline
ref_ptr<T> const_pointer_cast(const ref_ptr<Y>& rp) { return const_cast<T*>(rp.get()); }

#endif

