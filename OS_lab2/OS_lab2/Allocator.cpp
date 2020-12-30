#include "Allocator.h"
#include <string>
#include <cassert>
#include <iostream>

Allocator::Allocator(size_t size)
{
    pageNumber = ceil((double)size / pageSize);
    size = pageNumber * pageSize;
    p = malloc(size);
    for (int i = 0; i < pageNumber; ++i)
    {
        void* page = (void*)((char*)(p)+i * pageSize);
        freePages.push_back(page);
        Header header = Header();
        header.state = PageState::Free;
        headers.insert(pair<void*, Header>(page, header));
    }
    for (size_t i = minSize; i <= pageSize / 2; i *= 2)
    {
        freeClassPages.insert(pair<size_t, vector<void*>>(i, vector<void*>()));
    }
}

void* Allocator::mem_alloc(size_t size)
{
    if (fits_in_one(size))
    {
        auto classSize = get_class(size);
        auto page = find_class_page(classSize);
        if (page == nullptr) page = divide_free_page(classSize);
        return alloc_block(page);
    }
    return alloc_multipage(size);
}

void* Allocator::find_class_page(size_t classSize)
{
    auto pages = freeClassPages[classSize];
    if (pages.empty()) return nullptr;
    return pages.front();
}

void* Allocator::divide_free_page(size_t classSize)
{
    assert(!freePages.empty() && "Error. No free pages");
    auto page = freePages.back();
    freePages.pop_back();

    auto header = headers[page];
    header.size = classSize;
    header.p = page;
    header.state = PageState::Divided;
    header.blocks = pageSize / classSize;
    headers[page] = header;

    for (int i = 0; i < header.blocks - 1; ++i)
    {
        void* block = (int*)((char*)page + i * header.size);
        auto blockHeader = (BlockHeader*)block;
        void* nextBlock = (int*)((char*)page + (i + 1) * header.size);
        blockHeader->next = nextBlock;
    }

    freeClassPages[classSize].push_back(page);
    return page;
}

void* Allocator::alloc_block(void* page)
{
    auto header = headers[page];
    auto addr = header.p;
    auto next = ((BlockHeader*)addr)->next;
    header.p = next;
    header.blocks--;
    if (header.blocks == 0)
    {
        auto pages = freeClassPages[header.size];
        pages.erase(remove(pages.begin(), pages.end(), page), pages.end());
    }
    headers[page] = header;
    return addr;
}

void* Allocator::alloc_multipage(size_t size)
{
    size_t amount = ceil((double)size / pageSize);
    assert(amount <= freePages.size() && "Error. Not enough space");
    std::vector<void*> pages;
    for (int i = 0; i < pageNumber; ++i)
    {
        void* page = (void*)((char*)(p)+i * pageSize);
        auto header = headers[page];
        if (header.state == PageState::Free)
        {
            pages.push_back(page);
            if (pages.size() == amount) break;
        }
        else
        {
            pages.clear();
        }
    }
    assert(pages.size() == amount && "Error. Not enough consecutive pages");
    for (auto const& page : pages)
    {
        auto header = headers[page];
        header.state = PageState::MultiplePage;
        header.blocks = amount;
        header.size = amount * pageSize;
        headers[page] = header;
        freePages.erase(remove(freePages.begin(), freePages.end(), page), freePages.end());
    }
    return pages.front();
}

void* Allocator::mem_realloc(void* addr, size_t size)
{
    auto newAddr = mem_alloc(size);
    if (newAddr == nullptr) return nullptr;

    move_mem(addr, newAddr);
    mem_free(addr);
    return newAddr;
}

void Allocator::move_mem(void* addr, void* newAddr)
{
    auto page = get_page(addr);
    auto newPage = get_page(newAddr);

    auto size = headers[page].size;
    auto newSize = headers[newPage].size;

    memcpy(addr, newAddr, min(size, newSize));
}

void Allocator::mem_free(void* addr)
{
    auto page = get_page(addr);
    auto header = headers[page];
    if (header.state == PageState::Divided) free_block(page, addr);
    else free_multipage(page, header.blocks);
}

void Allocator::free_block(void* page, void* addr)
{
    auto header = headers[page];

    auto next = header.p;
    header.p = addr;
    ((BlockHeader*)addr)->next = next;

    header.blocks++;
    if (header.blocks == pageSize / header.size)
    {
        header.state = PageState::Free;
        freePages.push_back(page);
        auto pages = freeClassPages[header.size];
        pages.erase(remove(pages.begin(), pages.end(), page), pages.end());
    }
    headers[page] = header;
}

void Allocator::free_multipage(void* firstPage, size_t amount)
{
    for (int i = 0; i < amount; ++i)
    {
        void* page = ((char*)(firstPage)+i * pageSize);
        auto header = headers[page];
        header.state = PageState::Free;
        headers[page] = header;
        freePages.push_back(page);
    }
}

void Allocator::mem_free()
{
    freePages.clear();
    for (int i = 0; i < pageNumber; ++i)
    {
        void* page = (void*)((char*)(p)+i * pageSize);
        auto header = headers[page];
        header.state = PageState::Free;
        headers[page] = header;
        freePages.push_back(page);
    }
    for (auto& classes : freeClassPages)
    {
        classes.second.clear();
    }
}

void Allocator::mem_dump()
{
    cout << "No of pages: " << pageNumber << endl;
    cout << "Page size: " << pageSize << endl;
    cout << "No of free pages: " << freePages.size() << endl;
    for (int i = 0; i < pageNumber; ++i)
    {
        void* page = (void*)((char*)(p)+i * pageSize);
        auto header = headers[page];
        auto state = header.state == PageState::Free ? "Free" :
            header.state == PageState::Divided ? "Divided" : "Multiple";
        auto classSize = header.state == PageState::Divided ? " - Class size: " + to_string(header.size) : "";
        auto freePlace = header.state == PageState::Divided ? " - Free blocks: " + to_string(header.blocks) : "";
        cout << "Page " << page << " " << state << classSize << freePlace << endl;
    }
    cout << endl;
}

Allocator::~Allocator()
{
    free(p);
}