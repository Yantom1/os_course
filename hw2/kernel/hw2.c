#include <linux/kernel.h>
#include<linux/sched.h>
#include <linux/list.h>
#include <linux/module.h>

long sys_get_leaf_children_sum_aux(struct task_struct* task);

asmlinkage long sys_hello(void) {
        printk("Hello, World!\n");
        return 0;
} 

asmlinkage long sys_set_weight(int weight){
        if(weight < 0)
                return -EINVAL;
        current->weight = weight;
        return 0;
}

asmlinkage long sys_get_weight(void){
        return current->weight;
}

asmlinkage long sys_get_leaf_children_sum(void){
        if(list_empty(&current->children)){
                return -ECHILD;
        }
        return sys_get_leaf_children_sum_aux(current);

}

long sys_get_leaf_children_sum_aux(struct task_struct* task){
        long total_weight = 0;
  	struct list_head* node;
        struct task_struct* current_task;
        if(list_empty(&task->children)){
                return task->weight;
        }
        list_for_each(node, &task->children){
                current_task = list_entry(node, struct task_struct, sibling);
                total_weight += sys_get_leaf_children_sum_aux(current_task);
        }

        return total_weight;
}

asmlinkage long sys_get_heaviest_ancestor(void){
        long max_weight = current->weight;
        long max_pid = current->pid;

        struct task_struct* current_task = current;
        while(current_task->pid != 1){
                current_task = current_task->real_parent;

                if(current_task->weight > max_weight){
                        max_weight = current_task->weight;
                        max_pid = current_task->pid;
                }
                
        }

        return max_pid;
}