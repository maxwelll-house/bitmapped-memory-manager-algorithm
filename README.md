# Bitmapped-memory-manager-algorithm

Memory Allocation Pattern

//11111111 11111111 11111111

//11111110 11111111 11111111

//11111100 11111111 11111111

//if all bits for 1st section become 0 proceed to next section


//...
//00000000 11111111 11111111
//00000000 11111110 11111111
//00000000 11111100 11111111
//00000000 11111000 11111111


An refinement over the original fixed-size memory allocation scheme is the bitmapped memory manager. In this scheme, memory is requested from the operating system in relatively big chunks and memory allocation is achieved by carving out from these chunks. Deallocation is done by freeing the entire block at a single go, thus sealing off any potential leaks. Yet another advantage of this approach is that it can support array versions of the new and delete operations.
