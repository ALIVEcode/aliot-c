#include "AliotTimer.h"

AliotTimer::AliotTimer() {
    _delayTime = 1000;
    _currentTime = 0;
    _prevTime = 0;
}

void AliotTimer::setDelayTime(uint32_t delayTime) {
    _delayTime = delayTime;
}

void AliotTimer::setSleepTime(uint32_t sleepTime) {
    _sleepTime = sleepTime;
}

bool AliotTimer::waitDelayTime() {
    _currentTime = millis();
    if (!(_currentTime % _delayTime) && _currentTime != _prevTime) {
        _prevTime = _currentTime;
        return true;
    } return false;
}

void AliotTimer::deepSleep() {
    Serial.println("[AliotTimer] Going to sleep...");
    delay(DELAY_BEFORE_SLEEP);
    ESP.deepSleep(_sleepTime * 1000);
}
