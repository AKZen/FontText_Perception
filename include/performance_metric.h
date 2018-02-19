#ifndef PERFORMANCEMETRIC_H
#define PERFORMANCEMETRIC_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <windows.h>
#include <conio.h>
#include <chrono>
#include <vector>
#include <QString>
#include "IEmoStateDLL.h"
#include "Iedk.h"
#include "IedkErrorCode.h"
#include "IEmoStatePerformanceMetric.h"
#include "EmotivLicense.h"

class performance_metric {
private:
    EmoEngineEventHandle eEvent;
    EmoStateHandle eState;
    unsigned int userID;
    int state;

    double normalize(double rawScore, double maxScale, double minScale);
    double mini_log(double rawScore, double maxScale, double minScale);
    double log_data(EmoStateHandle eState);

public:
    performance_metric();
    ~performance_metric();

    QString run();
    std::vector<double> get_emo_states();
};

#endif // PERFORMANCEMETRIC_H
