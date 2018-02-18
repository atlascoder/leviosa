#ifndef CONTROLLERSMGR_H
#define CONTROLLERSMGR_H

#include <QObject>

#include "ControllerCfg.h"
#include <vector>
#include <memory>

class ControllersMgr : private QObject
{
    Q_OBJECT
public:
    static ControllersMgr& instance(){
        static ControllersMgr sInstance;
        return sInstance;
    }

private:
    std::unique_ptr<std::vector<std::unique_ptr<ControllerCfg>>> mControllers;
    ControllersMgr() : QObject(nullptr), mControllers(new std::vector<std::unique_ptr<ControllerCfg>>) {}
};

#endif // CONTROLLERSMGR_H
