#ifndef BGMRPCCLIENT_H
#define BGMRPCCLIENT_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QWebSocket>
#include <functional>

#include "BGMRPCClient_global.h"

namespace NS_BGMRPCClient {

class BGMRPCClient;

/*class Calling : public QObject {
    Q_OBJECT
public:
    Calling(const QString& mID, QObject* parent = nullptr);

    void waitForReturn(CallChain* callChain, const BGMRPCClient* client);

private:
    QString m_mID;
};

class BGMRPCCLIENT_EXPORT BGMRPCClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
public:
    BGMRPCClient(QObject* parent = nullptr);

    bool isConnected();
    void connectToHost(const QUrl& url);
    void disconnectFromHost();

signals:
    void isConnectedChanged(bool status);
    void connected();
    void disconnected();
    void onReturn(const QJsonDocument& jsonDoc);
    void onRemoteSignal(const QString& obj, const QString& sig,
                        const QJsonArray& args);
    void onError(const QJsonDocument& error);

public:
    void callMethod(CallChain* callChain, const QString& object,
                    const QString& method, const QVariantList& args);

protected:
    QWebSocket m_socket;
    QString m_mID;
    static quint64 m_totalMID;
};*/

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

class BGMRPCCLIENT_EXPORT BGMRPCClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
public:
    BGMRPCClient(QObject* parent = nullptr);
    virtual ~BGMRPCClient() {}

    bool isConnected();
    void connectToHost(const QUrl& url);
    void disconnectFromHost();

    Calling* callMethod(const QString& object, const QString& method,
                        const QVariantList& args);

signals:
    void isConnectedChanged(bool status);
    void stateChanged(QAbstractSocket::SocketState state);
    void connected();
    void disconnected();
    void calling(const QString& mID, const QString& obj, const QString& method);
    void returned(const QJsonDocument& jsonDoc);
    void error(const QJsonDocument& jsonDoc);
    void remoteSignal(const QString& obj, const QString& sig,
                        const QJsonArray& args);

private:
    friend Calling;
    QWebSocket m_socket;
    static quint64 m_totalMID;
};
}  // namespace NS_BGMRPCClient
#endif  // BGMRPCCLIENT_H
