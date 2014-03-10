#include "vm_pool.H"

VMPool::VMPool(unsigned long _base_address,
          unsigned long _size,
          FramePool *_frame_pool,
          PageTable *_page_table)
{
    this->vm_start_address = _base_address;
    this->pool_size        = _size;
    this->frame_pool       = _frame_pool;
    this->page_table       = _page_table;
    this->number_of_vm_regions = 0;

    /* The management information for the VM regions structure is stored in a frame.
     * A drawback with this implementation is that only a limited number of 
     * virtual memory regions are supported. The number of regions supported is equal to
     * the frame size divided by the size of vm_pool_info structure.
     */
    vm_region_info = reinterpret_cast <struct vm_region_info_t*> (frame_pool->get_frame());
    memset(vm_region_info, 0, FramePool::FRAME_SIZE); 
    this->page_table->register_vmpool(this);
}

unsigned long VMPool::allocate(unsigned long _size)
{
    unsigned long number_of_pages_to_allocate;
    unsigned long virtual_addr = 0;
    unsigned long region_size;

    /* The following logic ensures that we always allocate 
     * in multiples of page size
     */
    if (_size % PageTable::PAGE_SIZE == 0)
        number_of_pages_to_allocate = _size / PageTable::PAGE_SIZE;
    else
        number_of_pages_to_allocate = (_size / PageTable::PAGE_SIZE) + 1;

    region_size = number_of_pages_to_allocate * PageTable::PAGE_SIZE;

    if (number_of_vm_regions == 0)
    {
        vm_region_info[0].vm_region_start_addr = this->vm_start_address;
        vm_region_info[0].allocated_size       = region_size;
        virtual_addr = vm_region_info[0].vm_region_start_addr;
        number_of_vm_regions++;
    }
    else
    {
        for ( int i = 0; i < ( PageTable::PAGE_SIZE / sizeof(vm_region_info_t) ); i++)
        {
            if ((vm_region_info[i].vm_region_start_addr == 0))
            /* Two possibilities here. The start address of the region could be 0 
             * if it is a region which had been deallocated or if it is a 
             * region which has not been allocated before.
             */
            {
                /* Check for possibility #1 here, ie if the region had been deallocated before.
                 * If this is the case, the allocated_size field of the struct would not be 0.
                 */
                if ( vm_region_info[i].allocated_size != 0 )
                {
                    /*
                     * Handle corner case here. (for i == 0)
                     */
                    if( i == 0 )
                    {
                        if (region_size < vm_region_info[i].allocated_size)
                        {
                            vm_region_info[i].vm_region_start_addr = this->vm_start_address;
                            vm_region_info[i].allocated_size       = region_size;
                            virtual_addr                           = vm_region_info[i].vm_region_start_addr;
                            number_of_vm_regions++;
                            break;
                        }
                    }
                    else
                    {
                        if (region_size < vm_region_info[i].allocated_size)
                        {
                            int j;
                            for ( j = i-1; j >= 0; j-- )
                            {
                                if (vm_region_info[j].vm_region_start_addr != 0)
                                    break;
                            }
                            vm_region_info[i].vm_region_start_addr = vm_region_info[j].vm_region_start_addr + vm_region_info[j].allocated_size;
                            vm_region_info[i].allocated_size       = region_size;
                            virtual_addr                           = vm_region_info[i].vm_region_start_addr;
                            number_of_vm_regions++;
                            break;

                        }
                    }
                }

                /* Check for possibility #2 here, ie if the region has not been allocated before.
                 * If this is the case, the allocated_size field of the struct would be 0.
                 */
                if( (vm_region_info[i].allocated_size == 0) )
                {
                    /*
                     * Handling corner case here. (for i = 0)
                     */
                    int j;
                    for (j = i-1; j >= 0; j--)
                    {
                        if (vm_region_info[j].vm_region_start_addr != 0)
                            break;
                    }
                    vm_region_info[i].vm_region_start_addr = vm_region_info[j].vm_region_start_addr + vm_region_info[j].allocated_size;
                    vm_region_info[i].allocated_size       = region_size;
                    virtual_addr                           = vm_region_info[i].vm_region_start_addr;
                    number_of_vm_regions++;
                    break;

                }
            }
        }

    }

    return virtual_addr;

}

void VMPool::release(unsigned long _start_address)
{
    for (int i = 0; i < (PageTable::PAGE_SIZE / sizeof(vm_region_info_t)); i++)
    {
        if(vm_region_info[i].vm_region_start_addr == _start_address)
        {
            vm_region_info[i].vm_region_start_addr = 0;
            number_of_vm_regions--;
        }
    }
}

BOOLEAN VMPool::is_legitimate(unsigned long _address)
{
    for (int i = 0; i < (PageTable::PAGE_SIZE / sizeof(vm_region_info_t)); i++)
    {
        if (_address < (vm_region_info[i].vm_region_start_addr + vm_region_info[i].allocated_size))
            return TRUE;
    }
    return FALSE;
}
