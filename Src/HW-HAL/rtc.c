#include "rtc.h"

uint8_t Send_REQ_time=5;//�������ͨ��ָ�����֮����һ�ο���ָ�����
void RTC_Config(void)
{
    RTCHandle.Instance = RTC;
    RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
    {
        Error_Handler();
    }
}

void Wakeup_Source_Handle (void)
{
    switch(wake_up_flag)
    {
    case 0:
        Send_Frame_Type = 0;//���ϵ緢������
        break;
    case 1:
        Repeat_Count++;
        if (Repeat_Count%Ctrl_cycle == 0 ) //����ӷ�һ��req
        {
            Repeat_Count=0;
            if(Send_REQ_time>0) Send_REQ_time--;
            Send_Frame_Type = 2;
        }
        else
        {
            Send_Frame_Type = 1;
        }
        break;
    default:
        Send_Frame_Type = 0;//���ϵ緢������
        break;
    }
}



void Handle_WakeUpSource(void)
{
    if(wake_up_flag == 0)//�ո��ϵ�����
    {
        Send_Frame_Type = 1;  //�¶� message���з���
        HAL_PWR_EnableBkUpAccess();
        Sample_Interval=First_wake_up_time;//��һ����������ʱ��20
        Repeat_Count=0;
    }
    else if (wake_up_flag== 1)//�ӵ͹���ģʽ�л���
    {
        wake_up_flag=0; //RTC->BKP31R���ֵ������¼�Ƿ����͹���
        HAL_PWR_EnableBkUpAccess();
        if(One_Cycle==0)
        {
            Repeat_Count++;
            if (Repeat_Count%(Message_cycle/Wake_up_time) == 0 )  //ÿ��1���ӷ���һ��message
            {
                Send_Frame_Type = 1;  //�¶� message���з���
            }
            if (Repeat_Count%((Ctrl_cycle*Message_cycle)/Wake_up_time) == 0 )  //ÿ��1���ӷ���һ��message
            {
                Send_Frame_Type = 2;
                Send_REQ_time--;
                Repeat_Count=0;
            }
        }
        else
        {
            One_Cycle--;
            Repeat_Count=0;
        }
    }
}

