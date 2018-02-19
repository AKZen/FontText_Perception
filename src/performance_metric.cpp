#include "performance_metric.h"

#include <QThread>
#include <QDebug>

performance_metric::performance_metric() {
    eEvent	= IEE_EmoEngineEventCreate();
    eState = IEE_EmoStateCreate();
    unsigned int userID	= 0;
    int state  = 0;
}

performance_metric::~performance_metric() {
    //IEE_EngineDisconnect();
    IEE_EmoStateFree(eState);
    IEE_EmoEngineEventFree(eEvent);
}

QString performance_metric::run() {
    if (IEE_EngineConnect() == EDK_OK) {
        return "connection OK";
    } else {
        return "connection FALLED";
    }
}

double performance_metric::normalize(double rawScore, double maxScale, double minScale) {
    if (rawScore < minScale) {
        return 0;
    } else if (rawScore > maxScale)	{
        return 1;
    } else {
        return (rawScore - minScale)/(maxScale - minScale);
    }
}

std::vector<double> performance_metric::calculate() {
    std::vector<double> out;
    out.reserve(60000);
    QThread::currentThread()->usleep(10000);

    for (int i = 0; i < 60000; i++) {
        state = IEE_EngineGetNextEvent(eEvent);
        if (state == EDK_OK) {
            IEE_Event_t eventType = IEE_EmoEngineEventGetType(eEvent);
            IEE_EmoEngineEventGetUserId(eEvent, &userID);
            if (eventType == IEE_EmoStateUpdated) {
                IEE_EmoEngineEventGetEmoState(eEvent, eState);
                out.push_back(log_data(eState));
            }
        }
        QThread::currentThread()->usleep(1);
    }
    qDebug() << "IN THREAD " << out.size();
    return out;
}

double performance_metric::mini_log(double rawScore, double maxScale, double minScale) {
    if (minScale == maxScale) {
        return -1;
    } else {
        return normalize(rawScore, maxScale, minScale);
    }
}

double performance_metric::log_data(EmoStateHandle eState) {
    double rawScore = 0;
    double minScale = 0;
    double maxScale = 0;

    /* Interest */
    IS_PerformanceMetricGetInterestModelParams(eState, &rawScore, &minScale, &maxScale);
    return mini_log(rawScore, maxScale, minScale);
}
