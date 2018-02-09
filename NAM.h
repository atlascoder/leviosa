#ifndef NAM_H
#define NAM_H

#include <QNetworkAccessManager>
#include <QThread>

class NAM : private QThread
{
    void run() override {
        mNam = new QNetworkAccessManager();
        exec();
    }
public:
    static NAM& instance() {
        static NAM sInstance;
        return sInstance;
    }
    QNetworkAccessManager* nam() { return mNam; }
private:
    QNetworkAccessManager* mNam;
    NAM() {
        this->start();
    }
    ~NAM() {
        this->quit();
        this->wait();
        delete mNam;
    }
};

#endif // NAM_H
