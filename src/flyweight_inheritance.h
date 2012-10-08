/*
 * This file is part of the Sequoia MSO Solver.
 * 
 * Copyright 2012 Alexander Langer, Theoretical Computer Science,
 *                                  RWTH Aachen University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @author Alexander Langer
 */
#ifndef SEQUOIA_FLYWEIGHT_INHERITANCE_H
#define SEQUOIA_FLYWEIGHT_INHERITANCE_H

/*
 * Enable USE_BOOST to use boost's flyweight implementation.
 * Beware!  Will most definitely crash on program's exit (but the solving
 * works fine).
 */
//#define USE_BOOST

#include "hashing.h"

#ifndef USE_BOOST
#include "flyweight.h"
#else
#include <boost/flyweight/flyweight.hpp>
#include <boost/flyweight/hashed_factory.hpp>
#include <boost/flyweight/static_holder.hpp>
#include <boost/flyweight/intermodule_holder.hpp> // test for static
#include <boost/flyweight/simple_locking.hpp>
#include <boost/flyweight/no_locking.hpp>
#include <boost/flyweight/refcounted.hpp>
#include <boost/shared_ptr.hpp>
#endif

namespace sequoia {
    
template <typename Base> class FlyweightBase {
public:
    virtual ~FlyweightBase<Base>() { }
    virtual const Base* get() const = 0;
    virtual const FlyweightBase<Base>* clone() const = 0;
};

template <typename Base, typename Derived> class FlyweightDerived : public FlyweightBase<Base> {
public:
    FlyweightDerived<Base, Derived>(const Derived* entry) : _flyweight(entry) { }
    FlyweightDerived<Base, Derived>(const FlyweightDerived<Base, Derived>& other) :
        _flyweight(other._flyweight) { }
    virtual ~FlyweightDerived<Base, Derived>() { }
    virtual const FlyweightDerived<Base, Derived>* clone() const {
        return new FlyweightDerived<Base, Derived>(*this);
    }

#ifndef USE_BOOST
private:
    virtual const Base* get() const { return _flyweight.get(); }
    Flyweight<Derived> _flyweight;
#else
    virtual const Base* get() const { return _flyweight.get().get(); }
    /*
     * We need to use an (shared_ptr) holder object, because the pointer to
     * Derived must be detroyed when the refcount becomes 0.
     */
    typedef boost::shared_ptr<const Derived> ptr_holder;
    struct my_equals {
        size_t operator()(const ptr_holder& p1, const ptr_holder& p2) const {
            if (p1.get() == p2.get()) return true;
            if (p1.get() == NULL || p2.get() == NULL) return false;
            return *p1 == *p2;
        }
    };
    struct my_hasher {
        size_t operator()(const ptr_holder& in) const {
            ptr_deep_hasher<Derived> h;
            return h(in.get());
        }
    };
private:
    typedef boost::flyweight<ptr_holder,
        boost::flyweights::hashed_factory<my_hasher, my_equals>,
        boost::flyweights::intermodule_holder,
#if 1
        boost::flyweights::simple_locking,
#else
	boost::flyweights::no_locking,
#endif
        boost::flyweights::refcounted> FlyweightImpl;
    FlyweightImpl _flyweight;
#endif
};


/**
 * An implementation of the flyweight pattern that allows for inheritance.
 * This is achieved by using a wrapper class (FlyweightInheritance)
 * that only stores pointers to the implementation flyweights.
 * The actual implementation extends FlyweightBase in a concrete
 * implementation FlyweightDerived.
 */
template <typename Entry> class FlyweightInheritance {
public:
    FlyweightInheritance() : _wrapper(NULL) { }
    FlyweightInheritance(const FlyweightInheritance& other) : _wrapper(NULL) {
        if (other._wrapper != NULL)
            _wrapper = other._wrapper->clone();
    }
    FlyweightInheritance(const FlyweightBase<Entry>* wrapper) : _wrapper(wrapper) { }
    ~FlyweightInheritance() {
        delete _wrapper;
    }
    const Entry* get() const {
        assert(_wrapper != NULL);
        return _wrapper->get();
    }
    bool operator==(const FlyweightInheritance& other) const {
        if (_wrapper == other._wrapper) return true;
        if (_wrapper == NULL || other._wrapper == NULL) return false;
        /* The flyweight pattern guarantees that the underlying objects
         * are equal if and only if their pointer addresses are equal */
        return _wrapper->get() == other._wrapper->get();
    }
    bool operator!=(const FlyweightInheritance& other) const {
        return !(*this == other);
    }
    FlyweightInheritance& operator=(const FlyweightInheritance& other) {
        assert(other._wrapper != NULL);
        assert(_wrapper == NULL); // if this fails, run delete on wrapper.
        if (this != &other) 
            _wrapper = other._wrapper->clone();
        return *this;
    }
    const FlyweightInheritance<Entry>* clone() const {
        return new FlyweightInheritance<Entry>(*this);
    }
private:
    /* Use a direct pointer.  Using boost::shared_ptr will decrease performance
     * by roughly 10% */
    const FlyweightBase<Entry>* _wrapper;
};

template <typename T, typename H = hash_func_hasher<T> >
struct flyweight_inheritance_deep_hasher {
    typedef FlyweightInheritance<T> InFly;
    size_t operator()(const InFly& in) const {
        H h;
        return h(*in.get());
    }
};

template <typename T> struct FlyweightInheritanceFactory {
    template<typename D> static FlyweightInheritance<T> make_static(const D* obj) {
        //FlyweightDerived<T, D>::FlyweightImpl::init();
        return FlyweightInheritance<T>(new FlyweightDerived<T, D>(obj));
    }
    template<typename D> static const FlyweightInheritance<T>* make(const D* obj) {
        return new FlyweightInheritance<T>(new FlyweightDerived<T, D>(obj));
    }
};

} // namespace

#endif // SEQUOIA_FLYWEIGHT_INHERITANCE_H