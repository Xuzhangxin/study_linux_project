#define SYS_ify(syscall_name)	(__NR_##syscall_name)


# define INTERNAL_SYSCALL_RAW(name, err, nr, args...)		\
  ({								\
       register int _a1 asm ("r0"), _nr asm ("r7");		\          //映射int类型a1变量到r0寄存器，映射__nr变量到r7寄存器
       LOAD_ARGS_##nr (args)					\                  //LOAD_ARGS_##nr 展开便是LOAD_ARGS_2(const char *) (name), (flags)
       _nr = name;						\                          //nr 变量 = name = __NR_open = #define __NR_open		 45
       asm volatile ("swi	0x0	@ syscall " #name	\              //asm volatile，插入汇编代码执行,swi 0x0是汇编的软中断指令，用来触发syscall系统调用
		     : "=r" (_a1)				\                          //=r是输出操作，使用通用寄存器存储结果，并将结果输出到_a1变量
		     : "r" (_nr) ASM_ARGS_##nr			\                  //r是输入操作，将__nr，_a1 ,_a2变量作为参数，这里也会使用通用寄存器来传递值
		     : "memory");				\                           //表示内存约束，在执行这段汇编代码之前，需要刷新所有内存数据
       _a1; })                                                      //_a1作为整个宏的返回值
#endif

/*
#define ASM_ARGS_1	ASM_ARGS_0, "r" (_a1)
#define LOAD_ARGS_2(a1, a2)			\
  int _a2tmp = (int) (a2);			\
  LOAD_ARGS_1 (a1)				\
  register int _a2 asm ("a2") = _a2tmp;
#define ASM_ARGS_2	ASM_ARGS_1, "r" (_a2)
/*