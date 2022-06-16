SYSCALL_DEFINE5(mysetnice,pid_t,pid,int,flag,int,nicevalue,void __user *,prio,void __user *,nice)
{
	struct pid * p;
	struct task_struct * task;
	int oldnice;   //保存进程原始nice值
	int newnice;   
	int oldprio;   //保存进程原始prio值
	int newprio;
	p = find_get_pid(pid);  //获取进程的struct pid
	task = pid_task(p,PIDTYPE_PID); //获取进程的struct task_struct
	oldnice = task_nice(task);  //获取当前nice值
	oldprio = task_prio(task);  //获取当前prio值
	if(flag == 1)
	{
		set_user_nice(task,nicevalue);  //设置进程nice值为nicevalue
		newnice = task_nice(task);      //获取当前nice值
		newprio = task_prio(task);      //获取当前prio值
		if(copy_to_user((int*)nice,&newnice,sizeof(newnice))) 
			return EFAULT;
		if(copy_to_user((int*)prio,&newprio,sizeof(newprio))) 
			return EFAULT;
	}
	else if(flag == 0)
	{
		if(copy_to_user((int*)nice,&oldnice,sizeof(oldnice))) 
			return EFAULT;
		if(copy_to_user((int*)prio,&oldprio,sizeof(oldprio))) 
			return EFAULT;
	}
	else
		return EFAULT;
	return 0;
}