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
#ifndef SEQUOIA_LIST_H
#define	SEQUOIA_LIST_H

namespace sequoia {

/**
 * A more-or-less standard double-linked list implementation,
 * which allows for fast O(1) deletion and addition and
 * moving objects to the back without invalidating iterators
 * ("handles").
 * 
 * Goal is to provide fast, handle-safe move_back() functionality
 * for the LRU-cache implemented in cache.h.  The LRU-cache stores
 * the handles in a map<key, handle> to provide fast key lookups,
 * and does rely on non-invalidating iterators.
 * 
 * As of 2012-07-15, we cannot use std::list::splice(), because
 * the C++ standard says splice() invalidates pointers, which
 * apparently is true for at least MSVC according to some threads
 * on stackoverflow.  The SGI STL version does not invalidate iterators,
 * but we'd like sequoia to be portable.
 *
 * See for some dicussion:
 * http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#250
 * http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#278
 * http://stackoverflow.com/questions/143156/splice-on-stdlist-and-iterator-invalidation
 *
 * TODO:  Remove this class and change to std::list once C++11-compliant
 * compilers are wide-spread and splice() does (hopefully) not invalidate
 * iterators.
 */
template <typename Value> class List {
    class ListNode {
    public:
        ListNode(const Value* value) : _value(value), pred(NULL), succ(NULL) { }
        const Value* value() const { return _value; }
    private:
        friend class List<Value>;
        ListNode() : _value(NULL), pred(NULL), succ(NULL) { }
        const Value *_value;
        ListNode* pred;
        ListNode* succ;
    };
public:
    typedef ListNode* Handle;
    List() {
        _sentinel = new ListNode();
        _sentinel->pred = _sentinel;
        _sentinel->succ = _sentinel;
    }
    ~List() {
        delete _sentinel;
    }
    /**
     * O(1)
     */
    Handle push_back(const Value* value) {
        ListNode* node = new ListNode(value);
        push_back(node);
        return node;
    }
    /**
     * O(1)
     */
    void move_back(const Handle& handle) {
        ListNode* node = handle;
        remove_without_free(node);
        push_back(node);
    }
    /**
     * O(1)
     */
    bool empty() const { return _sentinel->pred == _sentinel; }
    /**
     * O(1)
     */
    const Value* pop_front() {
        assert(!empty());
	ListNode* node = _sentinel->succ;
	const Value* value = node->value();
	erase(node);
	return value;
    }
    /**
     * O(1)
     */
    void erase(const Handle& handle) {
        ListNode* node = handle;
        remove_without_free(node);
        delete node;
    }
private:
    void insert_before(ListNode* before, ListNode *node) {
        // first update the new node
        node->pred = before->pred;
	node->succ = before;
	// now adjust the existing nodes
        node->pred->succ = node;
	node->succ->pred = node;
    }
    void push_back(ListNode* node) {
	insert_before(_sentinel, node);
    }
    void remove_without_free(ListNode* node) {
        node->pred->succ = node->succ;
        node->succ->pred = node->pred;
    }
    ListNode* _sentinel;
};

} // namespace

#endif	/* SEQUOIA_LIST_H */
