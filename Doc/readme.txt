多任务：
1、起始任务，运行时创建LED1、LED2和LED3三个任务，然后删除自身。
2、LED1任务，每隔1s切换一次LED1亮灭状态。
3、LED2任务，每隔5s切换一次LED2亮灭状态。
4、LED3任务，每隔10s切换一次LED3亮灭状态。


ucos任务文章
https://blog.csdn.net/swee111/article/details/73285997
普通的	
OSTaskCreate()：创建人物
OSTaskDel（）：删除任务
OSTaskChangePrio（）：修改任务优先级
OSTaskRegSet（）：任务寄存器设置
OSTaskRegGet（）：获取当前寄存器的值
OSTaskSuspend（）：取消任务
OSTaskResume（）：恢复任务
OSTaskTimeQuantaSet（）：更改任务时间片
OS_TaskInit（）：任务初始化
OS_TaskInitTCB（）：初始化堆栈的默认值
--------------------- 
标记任务	
OSTaskSemPend（）：等待接收任务信号量
OSTaskSemPost（）：表示等待任务的信号
OSTaskSemPendAbort（）：终止等待任务信号
OSTaskSemSet（）：设置清除信号计数器
OSTaskStkChk（）：被用来检测堆栈后剩余内存量
--------------------- 
给任务发送消息	
OSTaskQPend（）：等待接收一个消息
OSTaskQPost（）：发送信息给任务
OSTaskQPendAbort（）：中止等待消息
OSTaskQFlush（）：刷新内部任务消息队列
OS_TaskResume():恢复一个已经移除的任务

--------------------- 


