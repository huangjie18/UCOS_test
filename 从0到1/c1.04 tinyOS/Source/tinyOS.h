#ifndef __TINYOS_H
#define __TINYOS_H

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct _tTask{
	tTaskStack * stack;
}tTask;


extern tTask * currentTask;  //��ǰ����ջָ�룬Ϊʲô��ָ�룿����仯ָ����һ����ͬ��ֵ
extern tTask * nextTask;

void tTaskRunFirst(void);
void tTaskSwitch(void);

#endif

