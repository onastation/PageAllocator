# PageAllocator
**Theory:**

All memory is divided into pages. The page size does not have to be the same as the virtual page size. For example, one virtual one can contain several pages of the allocator. All pages are aligned, so when accessing data at any address within a page, we refer to only one virtual page.

All memory blocks are divided into two groups. The first group includes blocks with sizes less than or equal to half the page, the second includes all the rest. The blocks of the first group are divided into classes. Blocks of the same class have the same size. For example, this size could be 2x (x ≥ 4). If an application requests a block of memory of some size less than or equal to half a page, then the allocator assigns the closest class to this block. The blocks of the second group are blocks of one or more pages. If an application requests a block of memory of some size greater than half a page, then the allocator rounds this size to the nearest integer number of pages.

Each page can be in one of three states: the page is free, the page is divided into blocks of one class, or the page is occupied by a multi-page block. The allocator contains a list of free pages. This list can be created as soon as the memory area is received or as busy pages are freed. If the page is divided into blocks of the same class, then this page can only contain blocks of this class and all blocks have the same size. If a page is occupied by a multi-page block, then this page may be followed by zero or more pages of one memory block.

For each page there is a page descriptor that uniquely defines its state. The location of the handle can be determined by the pointer to the handle. All pointers to page descriptors can be placed in a one-dimensional array (the length of the array is equal to the number of pages in the allocated memory area). For a page divided into small blocks, the page descriptor can be stored in the page itself. For a page divided into large blocks, the page descriptor can be stored in a smaller block (to get this block, you must recursively call the memory allocator). For many page blocks, page descriptors are not required, all information about the number of pages can be encoded in pointers to the page descriptors of the multipage block.

For a page divided into blocks, the page descriptor has a pointer to the first free block in the page, as well as a counter for the number of free blocks in that page. All free blocks in one page are linked into a list, the fields for linking into a list are in the free blocks themselves. When a block is released, the allocator calculates the page number of the block at the block address, determines the pointer to the page descriptor by the page number, adds the freed block to the free block list and increases the free block counter. If all blocks are free, then this page is released.

**Implementation:**

mem\_alloc(size\_t size) - allocates a memory chunk sized size

mem\_free(void\* addr) - frees memory at the specified address

mem\_free() - frees all memory

mem\_realloc(void\* addr, size\_t size) reallocates memory at the specified address with the specified size

**Examples:**

Testing functions:

![](https://user-images.githubusercontent.com/43548404/103768759-c262c200-502b-11eb-894d-5f9a555e1646.png)

One page allocation:

![](https://user-images.githubusercontent.com/43548404/103768760-c2fb5880-502b-11eb-9873-b6e2c9e9c1b5.png)

Multipage allocation:

![](https://user-images.githubusercontent.com/43548404/103768761-c393ef00-502b-11eb-99d4-dc1e13e98b1d.png)

Reallocation:

![](https://user-images.githubusercontent.com/43548404/103768762-c393ef00-502b-11eb-91dd-e568f5eec1df.png)
