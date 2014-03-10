#include "page_table.H"
#include "console.H"

#define SHIFT_RIGHT(addr, num_of_bits) ((addr) >> (num_of_bits))
#define PAGE_DIR_SHIFT   22
#define PAGE_TABLE_SHIFT 12

/* Definitios of static class members */
PageTable* PageTable::current_page_table;
FramePool* PageTable::kernel_mem_pool;    
FramePool* PageTable::process_mem_pool;
unsigned long PageTable::shared_size;        
unsigned int PageTable::paging_enabled;

PageTable::PageTable()
{
    /* Any 4k-aligned address can be used for the page_directory */
    page_directory = reinterpret_cast <unsigned long*> (process_mem_pool->get_frame() * PageTable::PAGE_SIZE);

    /* The page table follows the page_directory at the next 4k-aligned address */
    page_table     = reinterpret_cast <unsigned long*> (process_mem_pool->get_frame() * PageTable::PAGE_SIZE);

    unsigned long address = 0;

    /* The following is to map the first 4 MB of memory */

    for (int i = 0; i < PageTable::ENTRIES_PER_PAGE; i++)
    {
        /* The last 3 bits stand for superviser/user, read/write and present/non-present status respectively.
         * We set the these attribute bits to superviser, read/write and present */
        page_table[i] = address | 3;
        address       = address + PageTable::PAGE_SIZE;
    }

    /* The page table initialized above is stored in the page directory */
    page_directory[0] = (unsigned long) page_table;

    /* We then set the attribute bits to superviser, read/write and present */
    page_directory[0] |= 3;

    /* The rest of the entries in the page_directory have to be filled now.
     * Since the page tables in these entries have not been created yet, we just store zeros here. 
     * Also, we mark the attribute sets' present status to 0.
     * Hence we do a bitwise OR by 2
     */
    for (int i = 1; i < PageTable::ENTRIES_PER_PAGE; i++)
    {
        page_directory[i] = 0 | 2;
    }

    page_directory[1023] = (unsigned long) page_directory;
    page_directory[1023] |= 3;
}

void PageTable::init_paging(FramePool * _kernel_mem_pool,
                            FramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
    kernel_mem_pool  = _kernel_mem_pool;
    process_mem_pool = _process_mem_pool;
    shared_size      = _shared_size;
}

void PageTable::load()
{
    /*
     * Page table loaded into processor context
     * by loading the page directory address into 
     * the CR3 register
     */
    unsigned long *page_dir_addr = (unsigned long*) page_directory;
    current_page_table = this;
    write_cr3((long unsigned int)page_dir_addr);
}

void PageTable::enable_paging()
{
    /*
     * Write 1 to the 31st bit of CR0 register
     * to enable paging
     */
    paging_enabled = 1;
    write_cr0(read_cr0() | 0x80000000);
}

void PageTable::handle_fault(REGS * _r)
{
    /* Page fault handler */
   
    unsigned long addr = 0, page_dir_index = 0, page_table_index = 0, page_offset = 0;
    unsigned long page_fault_dir = read_cr2();

    /* This follows from the recursive model. The page directory base address in the virtual 
     * address space will always be 0xFFFFF000.
     */
    unsigned long *page_directory_virtual_base_addr = (unsigned long *) 0xFFFFF000;
    

    /* The page tables will start from the address 0xFFC00000 in the virtual address space. */
    unsigned long *page_table_virtual_base_addr = (unsigned long *) 0xFFC00000;

    /* Page directory index is stored in the first 10 bits of the virtual address */ 
    page_dir_index   = SHIFT_RIGHT (page_fault_dir, PAGE_DIR_SHIFT) & 0x3FF;

    /* Page table index is stored in the second 10 bits of the virtual address */ 
    page_table_index = SHIFT_RIGHT (page_fault_dir, PAGE_TABLE_SHIFT) & 0x3FF;

    /* Page offset is stored in the first 12 bits of the virtual address */
    page_offset = page_fault_dir & 0xFFF;

    if((page_directory_virtual_base_addr[page_dir_index] & 0x1) == 0)
    /* The page table is not present in the page directory. 
     * So allocate a frame from the process frame pool and mark the page as present.
     */
    {
        page_directory_virtual_base_addr[page_dir_index] = ((PageTable::process_mem_pool->get_frame()) * PageTable::PAGE_SIZE ) | 3;
    }
    unsigned long *page_table = (unsigned long*) ((unsigned long)page_table_virtual_base_addr + page_dir_index * 0x1000);
    page_table[page_table_index] = ((PageTable::process_mem_pool->get_frame()) * PageTable::PAGE_SIZE ) | 3;
}

void PageTable::free_page(unsigned long _page_no)
{

}

void PageTable::register_vmpool(VMPool *_pool)
{
}
