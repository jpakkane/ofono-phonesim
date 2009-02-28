/****************************************************************************
**
** Copyright (C) 2000-2007 TROLLTECH ASA. All rights reserved.
**
** This file is part of the Opensource Edition of the Qtopia Toolkit.
**
** This software is licensed under the terms of the GNU General Public
** License (GPL) version 2.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef INHERITEDSHAREDDATA_H
#define INHERITEDSHAREDDATA_H

#include <QSharedData>

template <class T> class InheritedSharedDataPointer
{
public:
    inline void detach() { if (d && d->ref != 1) detach_helper(); }
    inline T &operator*() { detach(); return *d; }
    inline const T &operator*() const { return *d; }
    inline T *operator->() { detach(); return d; }
    inline const T *operator->() const { return d; }
    inline operator T *() { detach(); return d; }
    inline operator const T *() const { return d; }
    inline T *data() { detach(); return d; }
    inline const T *data() const { return d; }
    inline const T *constData() const { return d; }

    inline bool operator==(const InheritedSharedDataPointer<T> &other) const { return d == other.d; }
    inline bool operator!=(const InheritedSharedDataPointer<T> &other) const { return d != other.d; }

    inline InheritedSharedDataPointer() { d = 0; }
    inline ~InheritedSharedDataPointer() { if (d && !d->ref.deref()) delete d; }

    explicit InheritedSharedDataPointer(T *data);
    inline InheritedSharedDataPointer(const InheritedSharedDataPointer<T> &o) : d(o.d) { if (d) d->ref.ref(); }
    inline InheritedSharedDataPointer<T> & operator=(const InheritedSharedDataPointer<T> &o) {
        if (o.d != d) {
#if QT_VERSION < 0x040400
            T *x = o.d;
            if (x) x->ref.ref();
            x = qAtomicSetPtr(&d, x);
            if (x && !x->ref.deref())
                delete x;
#else
            if (o.d)
                o.d->ref.ref();
            if (d && !d->ref.deref())
                delete d;
            d = o.d;
#endif
        }
        return *this;
    }
    inline InheritedSharedDataPointer &operator=(T *o) {
        if (o != d) {
#if QT_VERSION < 0x040400
            T *x = o;
            if (x) x->ref.ref();
            x = qAtomicSetPtr(&d, x);
            if (x && !x->ref.deref())
                delete x;
#else
            if (o)
                o->ref.ref();
            if (d && !d->ref.deref())
                delete d;
            d = o;
#endif
        }
        return *this;
    }

    inline bool operator!() const { return !d; }

private:
    void detach_helper();

    T *d;
};

template <class T>
Q_INLINE_TEMPLATE InheritedSharedDataPointer<T>::InheritedSharedDataPointer(T *adata) : d(adata)
{ if (d) d->ref.ref(); }

template <class T>
Q_OUTOFLINE_TEMPLATE void InheritedSharedDataPointer<T>::detach_helper()
{
    T *x = d->createCopy();
    x->ref.ref();
#if QT_VERSION < 0x040400
    x = qAtomicSetPtr(&d, x);
    if (!x->ref.deref())
        delete x;
#else
    if (!d->ref.deref())
        delete d;
    d = x;
#endif
}

#endif
