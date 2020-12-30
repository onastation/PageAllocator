#include <vector>
#include <map>
#include <cmath>
using namespace std;

class Allocator
{
public:
    explicit Allocator(size_t size);
    void* mem_alloc(size_t size);
    void* mem_realloc(void* addr, size_t size);
    void mem_free(void* addr);
    void mem_free();
    void mem_dump();
    ~Allocator();

private:
    enum class PageState
    {
        Free,
        Divided,
        MultiplePage,
    };
    struct Header
    {
        PageState state;
        size_t size;
        void* p;
        unsigned short blocks;
    };
    struct BlockHeader
    {
        void* next;
    };

    size_t const pageSize = 4096;
    size_t const minSize = 16;
    size_t pageNumber;

    void* p;

    vector<void*> freePages;
    map<void*, Header> headers;
    map<size_t, vector<void*>> freeClassPages;

    inline bool fits_in_one(size_t size) const
    {
        return size < pageSize / 2;
    }

    inline size_t get_class(size_t size)
    {
        auto next = (size_t)pow(2, ceil(log(size) / log(2)));
        return max(minSize, next);
    }

    inline void* get_page(void* addr)
    {
        size_t pageNumber = ((char*)addr - (char*)p) / pageSize;
        return (char*)p + pageNumber * pageSize;
    }

    void* find_class_page(size_t classSize);
    void* divide_free_page(size_t classSize);
    void* alloc_block(void* page);
    void* alloc_multipage(size_t size);

    void free_multipage(void* firstPage, size_t amount);
    void free_block(void* page, void* addr);

    void move_mem(void* addr, void* newAddr);
};