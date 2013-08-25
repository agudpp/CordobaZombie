/*
 * test.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#include <algorithm>
#include <vector>
#include <set>

#include <UnitTest++/UnitTest++.h>

#include <types/basics.h>
#include <types/StackVector.h>
#include <types/StackQueue.h>
#include <types/StackPriorityQueue.h>
#include <types/BoolCountingMask.h>

using namespace core;

typedef StackVector<int, 512> SV512;
typedef StackQueue<int, 512> SQ512;
typedef StackPriorityQueue<int, 512> SPQ512;


// helper method that creates a set from a StackVector
static std::set<int>
transformToSet(const SV512& v)
{
    std::set<int> result;
    for (unsigned int i = 0, size = v.size(); i < size; ++i) {
        result.insert(v[i]);
    }
    return result;
}

// Simple graph checking with one agent
//
TEST(CheckStackVector)
{
    SV512 v;

    // check clear and size and empty
    CHECK_EQUAL(512, v.max_size());
    CHECK_EQUAL(0, v.size());
    v.clear();
    CHECK_EQUAL(0, v.size());
    CHECK_EQUAL(true, v.empty());
    CHECK_EQUAL(false, v.full());
    v.push_back(1);
    CHECK_EQUAL(false, v.empty());
    CHECK_EQUAL(1, v.size());
    v.clear();
    CHECK_EQUAL(0, v.size());

    // insert some elements
    unsigned int count = 0;
    for (int i = -250; i < 250; ++i) {
        v.push_back(i);
        ++count;
        CHECK_EQUAL(count, v.size());
    }
    for (int i = -250, index = 0; i < 250; ++i, ++index) {
        CHECK_EQUAL(i, v[index]);
        CHECK_EQUAL(-250, v.front());
    }
    for (int i = 249, index = 0; i >= -250; --i, ++index) {
        CHECK_EQUAL(i, v.back());
        v.pop_back();
    }

    // check full
    v.clear();
    for (int i = 0; i < 512; ++i) {
        v.push_back(i);
    }
    CHECK(v.full());
    CHECK(!v.empty());

    // disorder insert / disordered remove
    //
    {
        std::set<int> before = transformToSet(v);
        for (unsigned int i = 0, size = v.size()/2; i < size; i += 2) {
            const int toRemove = v[i];
            before.erase(toRemove);
            v.disorder_remove(i);
        }
        std::set<int> after = transformToSet(v);
        CHECK(before == after);
    }

    {
        std::set<int> before = transformToSet(v);
        for (int i = 0, newValues = v.size()+1, size = v.size()/3; i < size;
                i += 2, ++newValues) {
            before.insert(newValues);
            v.disorder_insert(i, newValues);
        }
        std::set<int> after = transformToSet(v);
        CHECK(before == after);
    }
}

TEST(StackQueue)
{
    SQ512 q;

    CHECK_EQUAL(true, q.empty());
    CHECK_EQUAL(false, q.full());
    CHECK_EQUAL(0, q.size());
    CHECK_EQUAL(512, q.max_size());

    q.push_front(123);
    CHECK_EQUAL(false, q.empty());
    CHECK_EQUAL(false, q.full());
    CHECK_EQUAL(1, q.size());
    CHECK_EQUAL(123, q.back());
    CHECK_EQUAL(123, q.front());

    q.pop_front();
    CHECK_EQUAL(true, q.empty());
    CHECK_EQUAL(false, q.full());
    CHECK_EQUAL(0, q.size());

    // insert some elements in the begin and some elements at the end
    for (int i = 0; i < 250; ++i) {
        q.push_front(249-i);
        CHECK_EQUAL(i+1, q.size());
        CHECK_EQUAL(249-i, q.front());
    }

    for (int i = 0, start = 250; i < 250; ++i, ++start) {
        q.push_back(start);
        CHECK_EQUAL(start+1, q.size());
        CHECK_EQUAL(start, q.back());
    }

    for (int i = 0; i < 500; ++i) {
        CHECK_EQUAL(i, q[i]);
    }

    int total = 499;
    while (!q.empty()) {
        const int i = q.back();
        q.pop_back();
        CHECK_EQUAL(total--, i);
    }

    for (int i = 0; i < 512; ++i) {
        q.push_back(i);
    }
    CHECK(q.full());
    total = 0;
    while (!q.empty()) {
        const int i = q.front();
        q.pop_front();
        CHECK_EQUAL(total++, i);
    }
}

TEST(StackPriorityQueue)
{
    SPQ512 q;

    CHECK(q.empty());
    CHECK(!q.full());

    q.push(545);
    for (int i = 0; i < 100; ++i) {
        q.push(i);
        CHECK_EQUAL(545, q.top());
    }

    q.pop();
    int cont = 99;
    while (!q.empty()) {
        const int current = q.top();
        q.pop();
        CHECK_EQUAL(cont, current);
        --cont;
    }
}

int
main(void)
{
    return UnitTest::RunAllTests();
}



