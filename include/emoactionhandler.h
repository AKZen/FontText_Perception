#ifndef EMOACTIONHANDLER_H
#define EMOACTIONHANDLER_H

#include <QHash>
#include <QObject>
#include <QThread>
#include "include/Iedk.h"

// no really needed, just to formalize code-style
typedef IEE_MentalCommandAction_t MentalCommandAction;

typedef unsigned EmoUser;

class EmoActionHandler : public QObject {
    Q_OBJECT
private:
    void handleMentalCommand();
    void handleUpdate();
    void loop();

    EmoEngineEventHandle eEvent;
    EmoStateHandle eState;

    std::atomic<bool> isWorking;
public:
    enum Target {
      Headset, EmoComposer, ControlPanel
    };

    explicit EmoActionHandler(QObject *parent = nullptr);
    ~EmoActionHandler();
    bool run(Target target);
    void stop();

signals:
    void userAdded(EmoUser user);
    void userRemoved();
};


#endif // EMOACTIONHANDLER_H
