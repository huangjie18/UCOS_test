#ifndef __TINYOS_H
#define __TINYOS_H

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct _tTask{
	tTaskStack * stack;
}tTask;


extern tTask * currentTask;  //当前任务栈指针，为什么用指针？方便变化指向下一个不同的值
extern tTask * nextTask;

void tTaskRunFirst(void);
void tTaskSwitch(void);

#endif

