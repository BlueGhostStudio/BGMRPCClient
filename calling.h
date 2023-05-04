#ifndef CALLING_H
#define CALLING_H

#include <QObject>

#include "BGMRPCClient_global.h"

namespace NS_BGMRPCClient {

class BGMRPCClient;

class BGMRPCCLIENT_EXPORT Calling : public QObject {
    Q_OBJECT
public:
    Calling(BGMRPCClient* client, const QString& mID,
            QObject* parent = nullptr);

    void then(std::function<void(const QVariant&)> ret,
              std::function<void(const QVariant&)> err = nullptr);

private:
    BGMRPCClient* m_client;
    QString m_mID;
    std::function<void(const QVariant&)> m_returnCallback;
    std::function<void(const QVariant&)> m_errorCallback;
};

};      // namespace NS_BGMRPCClient
#endif  // CALLING_H
