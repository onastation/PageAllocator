#include <iostream>
#include "Allocator.h"

using namespace std;

void alloc_one_test()
{
    Allocator allocator = Allocator(4096 * 8);
    allocator.mem_dump();
    cout << "One page allocation test:\n";
    for (int i = 0; i < 4; ++i)
    {
        allocator.mem_alloc(2048);
    }
    allocator.mem_dump();
}

void alloc_multi_test()
{
    Allocator allocator = Allocator(4096 * 8);
    allocator.mem_dump();
    cout << "Multipage allocation test:\n";
    allocator.mem_alloc(4096);
    allocator.mem_alloc(4096 * 3);
    allocator.mem_dump();
}

void realloc_test() {
    Allocator allocator = Allocator(4096 * 8);
    cout << "Reallocation test:\n";
    auto* loc = allocator.mem_alloc(1024);
    loc = allocator.mem_realloc(loc, 2048);

    allocator.mem_dump();
}

void free_one_test()
{
    Allocator allocator = Allocator(4096 * 8);
    cout << "One page freeing test:\n";
    auto* t1 = allocator.mem_alloc(512);
    allocator.mem_alloc(512);
    allocator.mem_alloc(512);
    auto* t2 = allocator.mem_alloc(512);

    allocator.mem_free(t1);
    allocator.mem_free(t2);

    allocator.mem_dump();
}

void free_multi_test()
{
    Allocator allocator = Allocator(4096 * 8);
    cout << "Multipage freeing test:\n";
    auto* t1 = allocator.mem_alloc(4096);
    allocator.mem_alloc(2 * 4 * 1024);
    auto* t2 = allocator.mem_alloc(4096);

    allocator.mem_free(t1);
    allocator.mem_free(t2);

    allocator.mem_dump();
}

void free_all_test() {
    Allocator allocator = Allocator(4096 * 8);
    cout << "Freeing everything test:\n";
    allocator.mem_alloc(2 * 4 * 1024);
    for (int i = 0; i < 4; ++i)
    {
        allocator.mem_alloc(1024);
    }

    allocator.mem_free();
    allocator.mem_dump();
}


int main() {
    try
    {
        alloc_one_test();
        alloc_multi_test();
        realloc_test();
        free_one_test();
        free_multi_test();
        free_all_test();
    }
    catch (exception ex)
    {
        cerr << ex.what() << '\n';
        return 1;
    }
    return 0;
}