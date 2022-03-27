#include "rtc.h"

uint8_t Send_REQ_time=5;//发送五次通信指令参数之后发送一次控制指令参数
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
        Send_Frame_Type = 0;//刚上电发送数据
        break;
    case 1:
        Repeat_Count++;
        if (Repeat_Count%Ctrl_cycle == 0 ) //五分钟发一次req
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
        Send_Frame_Type = 0;//刚上电发送数据
        break;
    }
}



void Handle_WakeUpSource(void)
{
    if(wake_up_flag == 0)//刚刚上电运行
    {
        Send_Frame_Type = 1;  //温度 message进行发送
        HAL_PWR_EnableBkUpAccess();
        Sample_Interval=First_wake_up_time;//第一次启动唤醒时间20
        Repeat_Count=0;
    }
    else if (wake_up_flag== 1)//从低功耗模式中唤醒
    {
        wake_up_flag=0; //RTC->BKP31R这个值用来记录是否进入低功耗
        HAL_PWR_EnableBkUpAccess();
        if(One_Cycle==0)
        {
            Repeat_Count++;
            if (Repeat_Count%(Message_cycle/Wake_up_time) == 0 )  //每隔1分钟发送一次message
            {
                Send_Frame_Type = 1;  //温度 message进行发送
            }
            if (Repeat_Count%((Ctrl_cycle*Message_cycle)/Wake_up_time) == 0 )  //每隔1分钟发送一次message
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


