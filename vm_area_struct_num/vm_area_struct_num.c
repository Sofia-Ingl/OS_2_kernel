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
 
SYSCALL_DEFINE3(vm_area_struct_num, int, pid, int*, main_arr_sz, int*, buff_arr_sz)
{
 struct pid* pid_val;
 struct task_struct* process;
 struct mm_struct* mm;
 struct vm_area_struct * vm_area;

 pid_val = find_get_pid(pid);
 process = get_pid_task(pid_val, PIDTYPE_PID);
 if (process == NULL) {
	 printk(KERN_INFO "no task with gived pid \n");
	 return -1;
 }
 mm = process->mm;
 if (mm == NULL) {
	 printk(KERN_INFO "no memory mappings \n");
	 return 1;
 }
 vm_area = mm->mmap;
 
 int main_sz = 0;
 int buff_sz = 0;
 
 
 while (vm_area != NULL) {


	main_sz = main_sz + sizeof(struct vm_area_struct_view);

	if (vm_area->vm_file != NULL) {
		char* filename = vm_area->vm_file->f_path.dentry->d_name.name;
		int len = strlen(filename);
		buff_sz  = buff_sz + len + 1;
				
	} 
	
	vm_area = vm_area->vm_next;
	
		 
 }

 copy_to_user(main_arr_sz, &main_sz, sizeof(int));
 copy_to_user(buff_arr_sz, &buff_sz, sizeof(int));
 return 0;

}
