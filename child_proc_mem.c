#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>

MODULE_LICENSE("MIT");
MODULE_AUTHOR("SAIJYOTI PANDA");
MODULE_DESCRIPTION("A kernel module that creates a parent process with multiple children and prints a memory map");

// Number of children to create to show the usecase
#define MAX_CHILDREN 5 
// mutex lock
static DEFINE_MUTEX(sync_lock);

struct proc_node {
    struct task_struct *task;
    struct proc_node *children[MAX_CHILDREN];
    int num_children;
};

static struct proc_node *parent;

// Function to be executed by the child process
static int child_func(void *data) {
    struct proc_node *node = (struct proc_node *)data;
    void *mem_block;
    
    mutex_lock(&sync_lock);
    pr_info("    ├── Child Process: %s (PID: %d)\n", node->task->comm, node->task->pid);
    mutex_unlock(&sync_lock);
    
    // Allocate memory dynamically
    mem_block = kmalloc(1024, GFP_KERNEL);
    if (!mem_block) {
        pr_err("Memory allocation failed for child PID: %d\n", node->task->pid);
        return -ENOMEM;
    }
    
    mutex_lock(&sync_lock);
    pr_info("    │   ├── Memory Allocated: 0x%p\n", mem_block);
    mutex_unlock(&sync_lock);
    
    msleep(1000);
    
    // Free allocated memory
    mutex_lock(&sync_lock);
    pr_info("    │   ├── Memory Freed: 0x%p\n", mem_block);
    mutex_unlock(&sync_lock);
    kfree(mem_block);
    
    return 0;
}

// Function to be executed by the parent process
static int parent_func(void *data) {
    struct proc_node *parent_node = (struct proc_node *)data;
    int i;
    
    mutex_lock(&sync_lock);
    pr_info("├── Parent Process: %s (PID: %d)\n", parent_node->task->comm, parent_node->task->pid);
    mutex_unlock(&sync_lock);
    
    for (i = 0; i < parent_node->num_children; i++) {
        parent_node->children[i]->task = kthread_run(child_func, parent_node->children[i], "child_%d", i);
        if (IS_ERR(parent_node->children[i]->task)) {
            pr_err("Failed to create child thread %d\n", i);
        }
        msleep(100);
    }
    return 0;
}
// Module Initialization
static int __init linux_kernel_init(void) {
    int i;
    pr_info("Kernel Module Loaded: Creating Memory Map with Child Process Tree\n");
    
    parent = kmalloc(sizeof(struct proc_node), GFP_KERNEL);
    if (!parent) {
        pr_err("Failed to allocate memory for parent\n");
        return -ENOMEM;
    }
    
    parent->num_children = MAX_CHILDREN;
    for (i = 0; i < MAX_CHILDREN; i++) {
        parent->children[i] = kmalloc(sizeof(struct proc_node), GFP_KERNEL);
        if (!parent->children[i]) {
            pr_err("Failed to allocate memory for child %d\n", i);
            while (i-- > 0) {
                kfree(parent->children[i]);
            }
            kfree(parent);
            return -ENOMEM;
        }
    }
    
    parent->task = kthread_run(parent_func, parent, "parent");
    if (IS_ERR(parent->task)) {
        pr_err("Failed to create parent thread\n");
        kfree(parent);
        return PTR_ERR(parent->task);
    }
    return 0;
}

// Module Exit
static void __exit linux_kernel_exit(void) {
    int i;
    if (parent) {
        if (parent->task) {
            kthread_stop(parent->task);
            pr_info("Parent Process stopped\n");
        }
        for (i = 0; i < parent->num_children; i++) {
            if (parent->children[i]) {
                if (parent->children[i]->task) {
                    kthread_stop(parent->children[i]->task);
                    pr_info("Child Process %d stopped\n", i);
                }
                kfree(parent->children[i]);
            }
        }
        kfree(parent);
    }
    pr_info("Kernel Module Unloaded\n");
}

module_init(linux_kernel_init);
module_exit(linux_kernel_exit);


