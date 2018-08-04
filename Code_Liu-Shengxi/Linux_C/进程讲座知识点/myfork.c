/*************************************************************************
	> File Name: myfork.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年07月31日 星期二 10时32分31秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
struct task_struct //1.5KB
{
	//这个是进程的运行时状态，-1代表不可运行，0代表可运行，>0代表已停止。
	volatile long state;
	/* 
	flags是进程当前的状态标志，具体的如：
	0x00000002表示进程正在被创建； 
	0x00000004表示进程正准备退出； 
	0x00000040 表示此进程被fork出，但是并没有执行exec；
	0x00000400表示此进程由于其他进程发送相关信号而被杀死 。
	*/
	unsigned int flags;

	//这个是进程号
	pid_t pid;

	//这里出现了mm_struct 结构体，该结构体记录了进程内存使用的相关情况
	struct mm_struct *mm;
	void *stack;    //  指向内核栈的指针
	......
};

int fork()
{
	return sys_fork(...); //系统调用
}
int sys_fork(...)
{
	do_fork(...)
}
long do_fork(unsigned long clone_flags,
			 unsigned long stack_start,
			 unsigned long stack_size,
			 int __user *parent_tidptr,
			 int __user *child_tidptr)
{
	return _do_fork(clone_flags, stack_start, stack_size,
					parent_tidptr, child_tidptr, 0);
}
long _do_fork(unsigned long clone_flags, //参数标志来表明进程创建的方式
			  unsigned long stack_start,
			  unsigned long stack_size,
			  int __user *parent_tidptr,
			  int __user *child_tidptr,
			  unsigned long tls)
{
	1. 检查参数 
	2. strct task_struct *p;
	3. p = copy_process(clone_flags, stack_start, stack_size,
						child_tidptr, NULL, trace, tls);
	// 将进程插入运行队列，此时状态为TASK_RUNNING
	4.wake_up_new_task(p);
	5. return ? ;
}
/*/*
 * This creates a new process as a copy of the old one,
 * but does not actually start it yet.
 *
 * 根据clone_flags标志拷贝寄存器，以及其他数据结构（PCB？？）
 * It copies the registers, and all the appropriate(适当)
 * parts of the process environment (as per the clone
 * flags). The actual kick-off is left to the caller. 
 * 搞好的这个新的进程的启动由调用者完成启动 
 */
task_struct *copy_process(unsigned long clone_flags,
                                   unsigned long stack_start,
                                   struct pt_regs *regs,
                                   unsigned long stack_size,
                                   int __user *child_tidptr,
                                   struct pid *pid,
                                   int trace)
{     struct task_struct *p;
       //创建进程内核栈和进程描述符
       p = dup_task_struct(current);
       //得到的进程与父进程内容完全一致，初始化新创建进程
      retval = copy_thread_tls(clone_flags, stack_start, stack_size, p, tls);
	  //将寄存器%ax置为0，也是子进程pid返回0的原因 
      pid = alloc_pid(p->nsproxy->pid_ns_for_children); //分配新的 Pid 
       ……
       return p;
}

/*为新进程创建新的内核堆栈，thread_info和task_struct结构。 */
static struct task_struct *dup_task_struct(struct task_struct *orig)
{
       struct task_struct *tsk;
       struct thread_info *ti;
       int node = tsk_fork_get_node(orig);

       //创建进程描述符对象
       tsk = alloc_task_struct_node(node);
       //创建进程内核栈 thread_info
       ti = alloc_thread_info_node(tsk, node);
       //使子进程描述符和父进程一致
       err = arch_dup_task_struct(tsk, orig);
       //进程描述符stack指向thread_info
       tsk->stack = ti;
       //使子进程thread_info内容与父进程一致但task指向子进程task_struct
       setup_thread_stack(tsk, orig);
       return tsk;
}
int main(int argc, char *argv[])
{
	int pid;
	pid = fork();
	if (pid < 0)
	{
		fprintf(stderr, "Fork Failed!");
		exit(-1);
	}
	else if (pid == 0)
	{
		printf("This is Child Process!\n");
	}
	else
	{
		printf("This is Parent Process!\n");
		wait(NULL);
		printf("Child Complete!\n");
	}
	return 0;
}
