#pragma once
#include <unordered_map>
#include <map>
#include <vector>
#include<iostream>
#include <functional>

class TimeTask
{
    friend class Timer;
public:
    TimeTask(int dt, std::function<void()>cb, bool isRepeat = false) : delayTime_(dt),
                                                                       isRepeat_(isRepeat),
                                                                       callback_(cb) {
        isRun_ = true;
        //time_slot.resize(8);
    }
    void callBack();
    int getDelayTime() {return delayTime_;}
    bool isRepeat() {return isRepeat_;}
private:
    bool isRun_;                     //这个定时器是否生效
    int delayTime_;                  //延迟时间
    bool isRepeat_;                  //是否循环
    std::function<void()> callback_; //回调函数 
    std::vector<int> time_slot;      //创建的延迟时间转化为当前定时器被放到每层时间轮时所在格子索引
};

class TimeWheel
{
    friend class Timer;
public:
    TimeWheel(int slotNum) : slotNum_(slotNum) { curSlot_ = 0; }
private:
    std::map<int, std::vector<TimeTask*>> tasks; //每个格子包含的定时器
    int curSlot_;	//目前定时器几个格子
    int slotNum_;	//该时间轮多少格子
};

//当前该定时器内时间精度再1ms - 1000ms 要可以被1000ms整除
class Timer
{
public:
    ~Timer();
    TimeTask* addOnce(int delayTime, std::function<void()> callback);
    TimeTask* addRepeat(int delayTime, std::function<void()> callback, bool immediately = true);
    bool del(TimeTask* time);           //删除定时任务 只是不执行该定时器任务
    bool init(int maxMs = 1000);               //初始化定时器
    void tick(int ms);
private:
    bool isCanAdd(int delayTime);
    bool add(TimeTask* timer);
private:
    int curMs_ = 1000;		// 每隔多长时间轮动一次，槽间隔为1s
    int maxMs_ = 1000;		// 时间精度，也是最上层时间轮每个格子的时间
    std::vector<TimeWheel> timeWheelList_;//时间轮层数
};