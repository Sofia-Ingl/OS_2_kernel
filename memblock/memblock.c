#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/memblock.h>
#include <linux/uaccess.h>
#include <linux/string.h>

struct memblock_region_view {
 u64 base;
 u64 size;
 enum memblock_flags flags;
};

struct memblock_type_view {
 unsigned long cnt;
 unsigned long max;
 u64 total_size;
 struct memblock_region_view * regions;
 char* name;
};

struct memblock_view {
 int bottom_up;
 u64 current_limit;
 struct memblock_type_view memory;
 struct memblock_type_view reserved;

};

SYSCALL_DEFINE4(memblock, void*, dest_arr, int, dest_arr_sz, void*, 
buff_arr, int, buff_arr_sz)
{
 printk(KERN_INFO "memblock copy, sz - %lu\n", sizeof(struct memblock_view));
 int extra_sz_0 = strlen(memblock.memory.name) + strlen(memblock.reserved.name) + 2;
 int extra_sz_1 = sizeof(struct memblock_region)*(memblock.memory.cnt + memblock.reserved.cnt); 
 if (sizeof(struct memblock_view) > dest_arr_sz) {
	printk(KERN_INFO "fail, dest arr is not enough \n");
	return -1;
 }
 int return_val = -1;
 if ((extra_sz_0 + extra_sz_1) <= buff_arr_sz) {
	 
	 return_val = 0; // can take regions & strs into buffer
	 
 } else {
	if (extra_sz_0 <= buff_arr_sz) {
		return_val = 1;  // can take strs into buffer
	}
 } 
 
 if (return_val == -1) {
	printk(KERN_INFO "fail, buff arr is not enough \n");
	return -1;
 }
 
 int buf_pos = 0;
 int arr_pos = 0;
 
 
 struct memblock_type_view memo;
 memo.cnt = memblock.memory.cnt;
 memo.max = memblock.memory.max;
 memo.total_size = memblock.memory.total_size;
 if (return_val == 0) {
	 // can take regions
	 copy_to_user((char*)buff_arr + buf_pos, memblock.memory.regions, sizeof(struct memblock_region)* memblock.memory.cnt);
	 memo.regions = (void*)( (char*)buff_arr + buf_pos);
	 buf_pos = buf_pos + sizeof(struct memblock_region)* memblock.memory.cnt;
 } else {
	 memo.regions = NULL;
 }
 copy_to_user((char*)buff_arr + buf_pos, memblock.memory.name, strlen(memblock.memory.name) + 1);
 memo.name = (char*)buff_arr + buf_pos;
 buf_pos = buf_pos + strlen(memblock.memory.name) + 1;
 
 struct memblock_view mbl_v;
 mbl_v.bottom_up = (int)memblock.bottom_up;
 mbl_v.current_limit = memblock.current_limit;
 
 
 
 struct memblock_type_view reserved;
 reserved.cnt = memblock.reserved.cnt;
 reserved.max = memblock.reserved.max;
 reserved.total_size = memblock.reserved.total_size;
 if (return_val == 0) {
	 // can take regions
	 copy_to_user((char*)buff_arr + buf_pos, memblock.reserved.regions, sizeof(struct memblock_region)* memblock.reserved.cnt);
	 reserved.regions = (void*)((char*)buff_arr + buf_pos);
	 buf_pos = buf_pos + sizeof(struct memblock_region)* memblock.reserved.cnt;
 } else {
	 reserved.regions = NULL;
 }
 copy_to_user((char*)buff_arr + buf_pos, memblock.reserved.name, strlen(memblock.reserved.name) + 1);
 reserved.name = (char*)buff_arr + buf_pos;
 buf_pos = buf_pos + strlen(memblock.reserved.name) + 1;
 
 mbl_v.memory = memo;
 mbl_v.reserved = reserved;
 
 
 
 
 copy_to_user(dest_arr, &mbl_v, sizeof(struct memblock_view));
 

 printk(KERN_INFO "memblock \n");
 printk(KERN_INFO "bottom up: %s \n", ((int)memblock.bottom_up== 0)?"no":"yes");
 printk(KERN_INFO "current limit: 0x%llx \n", (u64)memblock.current_limit);
 printk(KERN_INFO "memblock types: \n");
 printk(KERN_INFO "1.\n");
 printk(KERN_INFO "memory name: %s \n", memblock.memory.name);
 printk(KERN_INFO "total_sz: 0x%llx \n", (u64)memblock.memory.total_size);
 printk(KERN_INFO "count: %lu\n", memblock.memory.cnt);
 printk(KERN_INFO "2.\n");
 printk(KERN_INFO "memory name: %s \n", memblock.reserved.name);
 printk(KERN_INFO "total_sz: 0x%llx \n", (u64)memblock.reserved.total_size);
 printk(KERN_INFO "count: %lu \n", memblock.reserved.cnt);
 return 0;
}

