#include <linux/kernel.h>
#include <linux/pid.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/string.h>
 
 
 struct vm_area_struct_view {
	 
	unsigned long vm_start;
	unsigned long vm_end;
	unsigned long vm_flags;
	unsigned long vm_pgoff;
	char* filename;
 };
 
SYSCALL_DEFINE6(vm_area_struct, int*, pid_and_first_struct_num, void*, dest_arr, int, dest_arr_sz, void*, buff_arr, int, buff_arr_sz, int*, all_areas_flag)
{
 struct pid* pid_val;
 struct task_struct* process;
 struct mm_struct* mm;
 struct vm_area_struct * vm_area;
 int pfsn[2];

 copy_from_user(pfsn, pid_and_first_struct_num, sizeof(int)*2);
 int pid = pfsn[0];
 int first_struct_num = pfsn[1];

 pid_val = find_get_pid(pid);
 process = get_pid_task(pid_val, PIDTYPE_PID);
 if (process == NULL) {
	 printk(KERN_INFO "no task with gived pid \n");
	 return -1;
 }
 mm = process->mm;
 if (mm == NULL) {
	 printk(KERN_INFO "no memory mappings \n");
	 return 0;
 }
 vm_area = mm->mmap;
 
 int dest_arr_pos = 0;
 int buff_arr_pos = 0;
 int areas_count = 0;
 int all_areas = 0;
 
 
 while (vm_area != NULL) {

	if (areas_count >= first_struct_num) {
	 
	struct vm_area_struct_view vm_area_view;
	if (dest_arr_sz < (dest_arr_pos + sizeof(struct vm_area_struct_view)) ) {
		
		 printk(KERN_INFO "not enough space in dest arr \n");
		/* not enough space in dest arr */
		copy_to_user(all_areas_flag, &all_areas, sizeof(int));
		return areas_count;
		
	}
	 
	if (vm_area->vm_file != NULL) {
		char* filename = vm_area->vm_file->f_path.dentry->d_name.name;
		int len = strlen(filename);
		if (buff_arr_sz < (buff_arr_pos + len + 1)) {
			printk(KERN_INFO "not enough space in buff (str) arr\n");
			/* not enough space in buff (str) arr */
			copy_to_user(all_areas_flag, &all_areas, sizeof(int));
			return areas_count;
		}
		
		copy_to_user((char*)buff_arr + buff_arr_pos, filename, len + 1);
		vm_area_view.filename = (char*)buff_arr + buff_arr_pos;
		vm_area_view.vm_pgoff = vm_area->vm_pgoff;
		buff_arr_pos = buff_arr_pos + len + 1;
		
		
	} else {
		vm_area_view.filename =  NULL;
		vm_area_view.vm_pgoff = 0;
	}
	
	vm_area_view.vm_start = vm_area->vm_start;
	vm_area_view.vm_end = vm_area->vm_end;
	vm_area_view.vm_flags = vm_area->vm_flags;
	
	copy_to_user((char*)dest_arr + dest_arr_pos, &vm_area_view, sizeof(struct vm_area_struct_view));
	dest_arr_pos = dest_arr_pos + sizeof(struct vm_area_struct_view);
	

	}
	areas_count++;
	 
	 
	 
	 
	printk(KERN_INFO "VM area struct \n");
	printk(KERN_INFO "vm area start: 0x%lx \n", vm_area->vm_start);
	printk(KERN_INFO "vm area end: 0x%lx \n", vm_area->vm_end);
	printk(KERN_INFO "vm area flags: 0x%lx\n", vm_area->vm_flags);
	printk(KERN_INFO "some basic characteristics: \n");
	if ((vm_area->vm_flags & 1) == 1) {
		printk(KERN_INFO "+ VM_READ\n");
	} else {
		printk(KERN_INFO "- VM_READ\n");
	}
	if ((vm_area->vm_flags & 2) == 2) {
		printk(KERN_INFO "+ VM_WRITE\n");
	 }
	 else {
		 printk(KERN_INFO "- VM_WRITE\n");
	 }
	 if ((vm_area->vm_flags & 4) == 4) {
		 printk(KERN_INFO "+ VM_EXEC\n");
	 } else {
		 printk(KERN_INFO "- VM_EXEC\n");
	 }
	 if ((vm_area->vm_flags & 8) == 8) {
		 printk(KERN_INFO "+ VM_SHARED\n");
	 } else {
		 printk(KERN_INFO "- VM_SHARED\n");
	 }
	 printk(KERN_INFO "file-backed: %s \n", (vm_area->vm_file == NULL)? "no":"yes");
	 if (vm_area->vm_file != NULL) {
		 printk(KERN_INFO "file name: %s \n", vm_area->vm_file->f_path.dentry->d_name.name);
		 printk(KERN_INFO "file offset (in page sz units): 0x%lx \n", vm_area->vm_pgoff); 
	 }
	 
	 
	 
	 
	vm_area = vm_area->vm_next;
	
		 
 }

 all_areas = 1;
 copy_to_user(all_areas_flag, &all_areas, sizeof(int));
 return areas_count;

}

