#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

static int hello_init(void)
{
    struct task_struct *p;
    p = &init_task;
    printk("程序名\tPID\t进程状态\t进程优先级\t父进程PID\t");
    for_each_process(p)
    {
        if(p->mm == NULL)
	{ 
            printk("%s\t%d\t%ld\t%d\t%d\n",p->comm,p->pid, p->state,p->prio,p->parent->pid);
        }
    }
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT"goodbye!\n");
}

module_init(hello_init);  
module_exit(hello_exit);  
MODULE_LICENSE("GPL");
