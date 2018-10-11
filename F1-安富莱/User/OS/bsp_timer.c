/*
*********************************************************************************************************
*
*	ģ������ : ��ʱ��ģ��
*	�ļ����� : bsp_timer.c
*	��    �� : V1.0
*	˵    �� : ����systick��ʱ����Ϊϵͳ�δ�ʱ����ȱʡ��ʱ����Ϊ1ms��
*
*				ʵ���˶�������ʱ����������ʹ��(����1ms)�� ����ͨ���޸� TMR_COUNT ������ʱ������
*				ʵ����ms�����ӳٺ���������1ms�� ��us���ӳٺ���
*				ʵ����ϵͳ����ʱ�亯����1ms��λ��
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*		V1.1    2013-06-21 armfly  ����us���ӳٺ��� bsp_DelayUS
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp_timer.h"
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

#define SCH_MAX_TASKS   5
#define SystemCoreClock 72000000



sTask SCH_task_G[SCH_MAX_TASKS];  //������������

/* ��2��ȫ�ֱ���ת���� bsp_DelayMS() ���� */
static volatile uint32_t s_uiDelayCount = 0;
static volatile uint8_t s_ucTimeOutFlag = 0;

/* ���������ʱ���ṹ����� */
static SOFT_TMR s_tTmr[TMR_COUNT];


/*
	ȫ������ʱ�䣬��λ1ms
	����Ա�ʾ 24.85�죬�����Ĳ�Ʒ��������ʱ�䳬�������������뿼���������
*/
__IO int32_t g_iRunTime = 0;


/*
*******************************************************************************************
* �� �� ��: bsp_InitTimer
* ����˵��: ���� systick �жϣ�����ʼ�������ʱ������
* �� ��: ��
* �� �� ֵ: ��
*******************************************************************************************
*/
void bsp_InitTimer(void)
{
	uint8_t i;
	
	//�������е������ʱ��
	for(i = 0;i < TMR_COUNT; i++)
	{
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = TMR_ONCE_MODE;
		
	}
	
	//����systick�ж�����Ϊ1ms,������systick�ж�
	SysTick_Config(SystemCoreClock/1000);
}

/*
*********************************************************************************************************
*	�� �� ��: SCH_Update(void)
*	����˵��: ��������ˢ�º�����ÿ��ʱ���ж�ִ��һ�Ρ�����શ�ʱ���ж�����ִ�С�
*			  ��ˢ�º���ȷ��ĳ������Ҫִ�е�ʱ�򣬽�RunMe��1��Ҫע�����ˢ������
*			  ��ִ���κκ�������Ҫ���е������е��Ⱥ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SCH_Update(void)
{
	tByte index;
	
	/*ע�������λ��ʱ�꣬���Ǻ���*/
	for(index = 0; index < SCH_MAX_TASKS;index++)
	{
		//��������Ƿ�������
		if(SCH_task_G[index].pTask)
		{
			//�����������ʱʱ�䵽�ˣ�RunMe��1��־�������
			if(SCH_task_G[index].Delay == 0)
			{
				/*������Ҫ���� ��RunMe��1*/
				SCH_task_G[index].RunMe += 1;
				
				if(SCH_task_G[index].Period)
				{
					/*���������Ե������ٴ�ִ��*/
					SCH_task_G[index].Delay = SCH_task_G[index].Period;
				}
			}
			else
			{
				/*����׼��������*/
				SCH_task_G[index].Delay -= 1;
			}
		}
	}
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_RunPer1ms
*	����˵��: �ú���ÿ��1ms��Systick�жϵ���1�Ρ���� bsp_timer.c�Ķ�ʱ�жϷ������һЩ��Ҫ�����Դ����
*			������Է��ڴ˺��������磺��������ɨ�衣
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_RunPer1ms(void)
{
	SCH_Update();
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_RunPer10ms
*	����˵��: �ú���ÿ��10ms��Systick�жϵ���1�Ρ���� bsp_timer.c�Ķ�ʱ�жϷ������һЩ��Ҫ�����Դ���
*			��������Է��ڴ˺��������磺����ɨ�衢���������п��Ƶȡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_RunPer10ms(void)
{
	
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SoftTimerDec
*	����˵��: ÿ��1ms�����ж�ʱ��������1�����뱻SysTick_ISR�����Ե��á�
*	��    ��:  _tmr : ��ʱ������ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->Count > 0)
	{
		/* �����ʱ����������1�����ö�ʱ�������־ */
		if (--_tmr->Count == 0)
		{
			_tmr->Flag = 1;

			/* ������Զ�ģʽ�����Զ���װ������ */
			if(_tmr->Mode == TMR_AUTO_MODE)
			{
				_tmr->Count = _tmr->PreLoad;
			}
		}
	}
}
/*
*********************************************************************************************************
*	�� �� ��: SysTick_ISR
*	����˵��: SysTick�жϷ������ÿ��1ms����1��
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SysTick_ISR(void)
{
	static uint8_t s_count = 0;
	uint8_t i;
	
	/* ÿ��1ms����1�� �������� bsp_DelayMS�� */
	if(s_uiDelayCount > 0) //��������ʱ�����ı�־λ
	{
		if(--s_uiDelayCount == 0)
		{
			s_ucTimeOutFlag = 1;  //��־��ʱʱ�䵽
		}
	}
	
	/* ÿ��1ms���������ʱ���ļ��������м�һ���� */
	//TMR_COUNT��ʾ�ж��ٸ������ʱ��
	for(i = 0; i < TMR_COUNT; i++)
	{
		bsp_SoftTimerDec(&s_tTmr[i]);  //�����ʱ����1ms
	}
	
	/* ȫ������ʱ��ÿ1ms��1 */
	g_iRunTime++;
	if(g_iRunTime == 0x7FFFFFFF) /* ��������� int32_t ���ͣ������Ϊ 0x7FFFFFFF */
	{
		g_iRunTime = 0;
	}
	
	bsp_RunPer1ms();		/* ÿ��1ms����һ�δ˺���������SCH_Update */
	
	if(++s_count >= 10)
	{
		s_count = 0;
		bsp_RunPer10ms();  /* ÿ��10ms����һ�δ˺��� */
	}
	
}


/*
*********************************************************************************************************
*	�� �� ��: SCH_Add_Task
*	����˵��: �������
*	��    �Σ�void (*pFuntion)(void); tWord DELAY; tWord PERIOD
*	�� �� ֵ: ���������ID��
*   ʹ��˵����
*	��1��SCH_Add_Task(DOTASK,1000,0) DOTASK�Ǻ��������е�ַ��1000��1000��ʱ���Ժ�ʼ���У�ֻ����һ�Σ�
*	��2��Task_ID = SCH_Add_Task(DOTASK,1000,0); �������ʾ������ �Ա��Ժ�ɾ������ 
*	��3��SCH_Add_Task(DOTASK,0,1000); ÿ��1000��ʱ�������Ե�����һ�Σ�
*********************************************************************************************************
*/
#define  RETURN_ERROR                 0x00;
#define  RETURN_NORMOL                0x01;
#define  ERROR_SCH_CANOT_DELETE_TASK  0x02;
#define  ERROR_SCH_TOO_MANY_TASKS	  0x03;

tByte Error_code_G; //������Ϣ

tByte SCH_Add_Task(void (*pFuntion)(void),
					tWord DELAY,
					tWord PERIOD)
{
	tByte index = 0; /*�����ڶ������ҵ�һ����϶��������еĻ���*/
	//pTask��û��ֵ��ʾ����������񵽸ó�Ա
	while((SCH_task_G[index].pTask != 0) && (index < SCH_MAX_TASKS))
	{
		index++;
	}
	/*��������������Ŀ �򷵴�����Ϣ*/
	if(index == SCH_MAX_TASKS)
	{
		Error_code_G = ERROR_SCH_TOO_MANY_TASKS;/*����ȫ�ִ������*/
		return SCH_MAX_TASKS;
	}
	
	SCH_task_G[index].pTask = pFuntion;	/*���е�����˵������������ɹ�*/
	SCH_task_G[index].Delay = DELAY;
	SCH_task_G[index].Period = PERIOD;
	SCH_task_G[index].RunMe =0;
	
	return index;					   /*���������λ�ã��Ա����Ժ�ɾ��*/
	
}

/*
*********************************************************************************************************
*	�� �� ��: SCH_Task_Delete
*	����˵��: ɾ������
*	��    �Σ�tByte index
*	�� �� ֵ: �Ƿ�ɾ���ɹ�
*********************************************************************************************************
*/
tByte SCH_Task_Delete(tByte index)
{
	tByte Return_code;
	
	/*����û������*/
	if(SCH_task_G[index].pTask == 0)				   
	{
		/*����ȫ�ִ������*/
		Error_code_G = ERROR_SCH_CANOT_DELETE_TASK;
		Return_code  = RETURN_ERROR;
	}
	else
	{	
		Return_code  = RETURN_NORMOL;		
	}

	SCH_task_G[index].pTask = 0x0000;
	SCH_task_G[index].Delay = 0;
	SCH_task_G[index].Period = 0;
	SCH_task_G[index].RunMe =0;
	
	return Return_code;		/*����״̬*/
}
/*
*********************************************************************************************************
*	�� �� ��: SCH_Dispatch_Tasks
*	����˵��: ������������ִ�еĵ��Ⱥ�����˳��ִ��û�����ȼ�֮��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SCH_Dispatch_Tasks(void)
{
	tByte index;
	for(index = 0; index < SCH_MAX_TASKS; index++)
	{
		if(SCH_task_G[index].RunMe >0)
		{
			/*ִ������ */
			(*SCH_task_G[index].pTask)();    

			/* ִ��������ɺ󣬽�RunMe��һ */
			SCH_task_G[index].RunMe -= 1;

			/*����ǵ�������Ļ���������ɾ�� */	  
			if(SCH_task_G[index].Period == 0) 
			{		
				SCH_Task_Delete(index);
			}
		}
	}
}