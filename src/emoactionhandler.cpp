#include "include/EmoActionHandler.h"
#include "include/IEmoStateDLL.h"
#include "include/IedkErrorCode.h"
#include <iostream>
#include <cassert>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

EmoActionHandler::EmoActionHandler(QObject *parent) : QObject(parent) {
    isWorking.store(false);
    eEvent = IEE_EmoEngineEventCreate();
    eState = IEE_EmoStateCreate();

    qRegisterMetaType<MentalCommandAction>("MentalCommandAction");
    qRegisterMetaType<EmoUser>("EmoUser");
}

EmoActionHandler::~EmoActionHandler() {
    stop();
    IEE_EmoStateFree(eState);
    IEE_EmoEngineEventFree(eEvent);
}

bool EmoActionHandler::run(Target target) {
    if (isWorking.load()) {
        return false;
    }

    /*Connect with Emocomposer app on port 1726*/
    /*Connect with Control Panel app on port 3008*/
    if (target == Headset) {
        if (IEE_EngineConnect() != EDK_OK)
            return false;
    } else if (target == EmoComposer) {
        if (IEE_EngineRemoteConnect("127.0.0.1", 1726) != EDK_OK)
            return false;
    } else if (target == ControlPanel) {
        if (IEE_EngineRemoteConnect("127.0.0.1", 3008) != EDK_OK)
            return false;
    }

    isWorking.store(true);
    QtConcurrent::run(this, &EmoActionHandler::loop);
    return true;
}

void EmoActionHandler::stop() {
    isWorking.store(false);
}

void EmoActionHandler::loop() {
    EmoUser userId;
    while (isWorking.load()) {
        int state = IEE_EngineGetNextEvent(eEvent);
        if (state == EDK_OK) {
            // qDebug() << "Loop thread " << QThread::currentThread();
            IEE_Event_t eventType = IEE_EmoEngineEventGetType(eEvent);
            IEE_EmoEngineEventGetUserId(eEvent, &userId);
            switch (eventType) {
                // New headset connected
                case IEE_UserAdded: {
                    emit userAdded(userId);
                    break;
                }
                // Headset disconnected
                case IEE_UserRemoved: {
                    emit userRemoved();
                    break;
                }
                // to deal with mental commands
                case IEE_EmoStateUpdated: {
                    IEE_EmoEngineEventGetEmoState(eEvent, eState);
                    handleUpdate();
                    break;
                }
                case IEE_FacialExpressionEvent: {
                    break;
                }
                // Handle MentalCommand training related event
                case IEE_MentalCommandEvent: {
                    handleMentalCommand();
                    break;
                }
                default: {
                    break;
                }
            }
        } else if (state != EDK_NO_EVENT) {
            qDebug() << "Internal error in Emotiv Engine!\n";
            break;
        }
        QThread::currentThread()->usleep(700);
    }
    isWorking.store(false);
    IEE_EngineDisconnect();
}

void EmoActionHandler::handleMentalCommand() {
    unsigned int userID = 0;
    IEE_EmoEngineEventGetUserId(eEvent, &userID);
    IEE_MentalCommandEvent_t eventType = IEE_MentalCommandEventGetType(eEvent);
    switch (eventType) {
        case IEE_MentalCommandTrainingStarted: {
            emit mentalCommandTrainingStarted();
            break;
        }
        case IEE_MentalCommandTrainingSucceeded: {
            emit mentalCommandTrainingSucceeded();
            break;
        }
        case IEE_MentalCommandTrainingFailed: {
            mentalCommandTrainingFailed();
            break;
        }
        case IEE_MentalCommandTrainingCompleted: {
            emit mentalCommandTrainingCompleted();
            break;
        }
        case IEE_MentalCommandTrainingDataErased: {
            emit mentalCommandTrainingDataErased();
            break;
        }
        case IEE_MentalCommandTrainingRejected: {
            emit mentalCommandTrainingRejected();
            break;
        }
        case IEE_MentalCommandTrainingReset: {
            emit mentalCommandTrainingReset();
            break;
        }
        case IEE_MentalCommandAutoSamplingNeutralCompleted: {
            emit mentalCommandAutoSamplingNeutralCompleted();
            break;
        }
        case IEE_MentalCommandSignatureUpdated: {
            emit mentalCommandSignatureUpdated();
            break;
        }
        case IEE_MentalCommandNoEvent:
            break;
        default: {
            //@@ unhandled case
            assert(0);
            break;
        }
    }
}

void EmoActionHandler::handleUpdate() {
    IEE_MentalCommandAction_t actionType = IS_MentalCommandGetCurrentAction(eState);
    float actionPower = IS_MentalCommandGetCurrentActionPower(eState);
    emit mentalCommandUpdate(actionType, actionPower);
    if (IS_FacialExpressionIsBlink(eState)) {
        emit facialExpressionBlink();
    }
    if (IS_FacialExpressionIsLeftWink(eState)) {
        emit facialExpressionLeftWink();
    }
    if (IS_FacialExpressionIsRightWink(eState)) {
        emit facialExpressionRightWink();
    }
}
