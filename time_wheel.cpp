#include "time_wheel.h"
#include<chrono>
#include<ctime>
#include <ratio> 
void TimeTask::callBack() {
    if (callback_ != nullptr && isRun_) {
        callback_();
    }
}

Timer::~Timer() {
    for (auto& timeWheel : timeWheelList_) {
        for (auto* task : timeWheel.tasks[timeWheel.curSlot_]) {
            if (task) {
                delete task;
                task = nullptr;
            }
        }
        timeWheel.tasks.clear();
    }
    timeWheelList_.clear();
}

void Timer::tick(int ms) {
    using namespace std;
    using namespace std::chrono;
    while (curMs_ >= maxMs_) {
        auto begin = chrono::duration_cast<chrono::milliseconds>(steady_clock::now().time_since_epoch());

        curMs_ = curMs_ - maxMs_;

        bool isAddCurSlot = false;
        bool needSyncPreTimeWheel = false;

        TimeWheel* preTimeWheel = nullptr;

        for (int i = 0; i < timeWheelList_.size(); ++i){
            auto& timeWheel = timeWheelList_[i];
            //执行最上层当前时间轮
            if (0 == i) {
                for (auto* task : timeWheel.tasks[timeWheel.curSlot_]) {
                    if (task) {
                        task->callBack();
                        if (task->isRepeat()) {
                            add(task);
                        }
                        else {
                            delete task;
                            task = nullptr;
                        }
                    }
                }
                isAddCurSlot = true;
            }//下层时间轮同步给上层时间轮
            else if (needSyncPreTimeWheel) {
                needSyncPreTimeWheel = false;
                auto& tasks = timeWheel.tasks[timeWheel.curSlot_];
                for (auto* task : tasks) {
                    int slot = task->time_slot[i - 1];
                    if (preTimeWheel) {
                        preTimeWheel->tasks[slot].emplace_back(task);
                    }
                }
                isAddCurSlot = true;
            }
            //进入下一个格子
            if (isAddCurSlot) {
                isAddCurSlot = false;
                timeWheel.tasks[timeWheel.curSlot_].clear();
                ++timeWheel.curSlot_;

                if (timeWheel.curSlot_ == timeWheel.slotNum_) {
                    timeWheel.curSlot_ = 0;
                    needSyncPreTimeWheel = true;
                    preTimeWheel = &timeWheel;
                }
            }
        
        }
        auto end = chrono::duration_cast<chrono::milliseconds>(steady_clock::now().time_since_epoch());
        auto diff = (end - begin).count();
        cout << "diff=" << diff << std::endl;
    }
    curMs_ += ms;
}

bool Timer::isCanAdd(int delayTime) {
    int maxDelayTime = maxMs_;
    for (auto& timeWheel : timeWheelList_) {
        maxDelayTime *= timeWheel.slotNum_;
    }
    if (delayTime >= maxDelayTime) {
        std::cout << delayTime << " > "<< maxDelayTime << std::endl;
        return false;
    }
    return true;
}

TimeTask* Timer::addOnce(int delayTime, std::function<void()> callback) {
    if (!isCanAdd(delayTime)) {
        return nullptr;
    }
    TimeTask* timer = new TimeTask(delayTime, callback);
    if (add(timer)) {
        return timer;
    }
    else {
        delete timer;
        return nullptr;
    }
}

TimeTask* Timer::addRepeat(int delayTime, std::function<void()> callback, bool immediately) {
    if (!isCanAdd(delayTime)) {
        return nullptr;
    }
    int time = 0;
    if (!immediately) {
        time = delayTime;
    }
    TimeTask* timer = new TimeTask(time, callback, true);
    if (add(timer)) {
        timer->delayTime_ = delayTime;
        return timer;
    }
    else {
        delete timer;
        return nullptr;
    }
}

bool Timer::add(TimeTask* task) {
    if (!task) {
        return false;
    }
    if (!isCanAdd(task->getDelayTime())) {
        return false;
    }

    task->time_slot.clear();

    int realSlot = task->getDelayTime() / maxMs_;

    for (auto& timeWheel : timeWheelList_) {
        realSlot += timeWheel.curSlot_;

        if (&timeWheel == &timeWheelList_[timeWheelList_.size() -1]) {
            realSlot %= timeWheel.slotNum_;
        }

        if (realSlot / timeWheel.slotNum_ > 0) {
            task->time_slot.emplace_back(realSlot % timeWheel.slotNum_);
            realSlot = realSlot / timeWheel.slotNum_ - 1;
        }
        else {
            timeWheel.tasks[realSlot].emplace_back(task);
            return true;
        }
    }
    return false;
}

bool Timer::del(TimeTask* timer)
{
    if(!timer) {
        return false;
    }
    timer->isRun_ = false;
    return true;
}

bool Timer::init(int maxMs)
{
    maxMs_ = maxMs; //精度ms
    curMs_ = maxMs_;
    if (maxMs_ < 1000) {
        while (1000 % maxMs_ != 0) {
            ++maxMs_;
        }
        if (maxMs_ < 1000) {
            timeWheelList_.emplace_back(1000 / maxMs_);//毫秒
        }
        else {
            maxMs_ = 1000;
        }
    }
    else {
        maxMs_ = 1000;
    }
    timeWheelList_.emplace_back(5); //秒
    timeWheelList_.emplace_back(5); //分钟
    timeWheelList_.emplace_back(5); //小时
    return true;
}
