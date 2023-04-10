#ifndef ALIOT_TIMER_H
#define ALIOT_TIMER_H

#include <Arduino.h>

#define DELAY_BEFORE_SLEEP 500

// Callback function must return boolean value to 
typedef std::function<bool()> AliotTimerCallback;

class AliotTimer {
    public:
        AliotTimer();

        // Duration of waitDelayTime() in milliseconds
        void setDelayTime(uint32_t delayTime);

        // Duration of deepSleep() in milliseconds
        void setSleepTime(uint32_t sleepTime);

        // Wait for delayTime to pass before proceeding
        bool waitDelayTime();

        // Put ESP into deep sleep for the duration of sleepTime
        void deepSleep();

    private:
        uint32_t _currentTime;
        uint32_t _prevTime;

        uint32_t _delayTime;
        uint32_t _sleepTime;
};

#endif /* ALIOT_TIMER_H */