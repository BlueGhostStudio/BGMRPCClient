#include "bgmrpcclient.h"

#include <QDebug>
#include <QTimer>

using namespace NS_BGMRPCClient;

/*Calling::Calling(const QString& mID, QObject* parent)
    : QObject(parent), m_mID(mID) {}

void
Calling::waitForReturn(CallChain* callChain, const BGMRPCClient* client) {
    QObject::connect(
        client, &BGMRPCClient::onReturn, [=](const QJsonDocument& jsonDoc) {
            if (jsonDoc["mID"].toString() == m_mID) {
                QObject::disconnect(client, &BGMRPCClient::onReturn, 0, 0);
                QObject::disconnect(client, &BGMRPCClient::onError, 0, 0);
                callChain->resolve(
                    jsonDoc["values"].toVariant());
                deleteLater();
            }
        });
    QObject::connect(
        client, &BGMRPCClient::onError, [=](const QJsonDocument& jsonDoc) {
            QObject::disconnect(client, &BGMRPCClient::onReturn, 0, 0);
            QObject::disconnect(client, &BGMRPCClient::onError, 0, 0);
            callChain->reject(jsonDoc["error"].toVariant());
            deleteLater();
        });
}

// ===============

quint64 BGMRPCClient::m_totalMID = 0;
BGMRPCClient::BGMRPCClient(QObject* parent) : QObject(parent) {
    QObject::connect(
        &m_socket, &QWebSocket::stateChanged,
        [=](QAbstractSocket::SocketState state) {
            emit isConnectedChanged(state == QAbstractSocket::ConnectedState);
        });
    QObject::connect(&m_socket, &QWebSocket::connected, this,
                     &BGMRPCClient::connected);
    QObject::connect(&m_socket, &QWebSocket::disconnected, this,
                     &BGMRPCClient::disconnected);
    QObject::connect(&m_socket, &QWebSocket::textMessageReceived,
                     [=](const QString& message) {
                         QJsonDocument jsonDoc =
                             QJsonDocument::fromJson(message.toUtf8());
                         if (jsonDoc["type"].toString() == "return")
                             onReturn(jsonDoc);
                         else if (jsonDoc["type"].toString() == "signal")
                             onRemoteSignal(jsonDoc["object"].toString(),
                                            jsonDoc["signal"].toString(),
                                            jsonDoc["args"].toArray());
                         else if (jsonDoc["type"].toString() == "error")
                             onError(jsonDoc);
                     });
}

bool
BGMRPCClient::isConnected() {
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

void
BGMRPCClient::connectToHost(const QUrl& url) {
    m_socket.open(url);
}

void
BGMRPCClient::disconnectFromHost() {
    m_socket.close();
}

void
BGMRPCClient::callMethod(CallChain* callChain, const QString& object,
                         const QString& method, const QVariantList& args) {
    m_mID = QString("#%1").arg(m_totalMID);
    m_totalMID++;
    QVariantMap callVariant;
    callVariant["object"] = object;
    callVariant["method"] = method;
    callVariant["args"] = args;
    callVariant["mID"] = m_mID;
    (new Calling(m_mID, this))->waitForReturn(callChain, this);
    m_socket.sendTextMessage(QJsonDocument::fromVariant(callVariant).toJson());
}*/

/*Calling::Calling(BGMRPCClient* client, const QString& mID, QObject* parent)
    : QObject(parent), m_client(client), m_mID(mID) {
    QObject::connect(
        m_client, &BGMRPCClient::returned, this,
        [=](const QJsonDocument& jsonDoc) {
            if (jsonDoc["mID"].toString() == m_mID) {
                QObject::disconnect(m_client, &BGMRPCClient::returned, this, 0);
                QObject::disconnect(m_client, &BGMRPCClient::error, this, 0);
                m_returnCallback(jsonDoc["values"].toVariant());
                deleteLater();
            }
        });
    QObject::connect(
        m_client, &BGMRPCClient::error, this,
        [=](const QJsonDocument& jsonDoc) {
            if (jsonDoc["mID"].toString() == m_mID) {
                QObject::disconnect(m_client, &BGMRPCClient::returned, this, 0);
                QObject::disconnect(m_client, &BGMRPCClient::error, this, 0);
                if (m_errorCallback) m_errorCallback(jsonDoc.toVariant());
                deleteLater();
            }
        });
}

void
Calling::then(std::function<void(const QVariant&)> ret,
              std::function<void(const QVariant&)> err) {
    m_returnCallback = ret;
    m_errorCallback = err;
}*/

quint64 BGMRPCClient::m_totalMID = 0;

BGMRPCClient::BGMRPCClient(QObject* parent) : QObject(parent) {
    QObject::connect(
        &m_socket, &QWebSocket::stateChanged, this,
        [=](QAbstractSocket::SocketState state) {
            emit isConnectedChanged(state == QAbstractSocket::ConnectedState);
            if (state == QAbstractSocket::ConnectedState &&
                m_aliveInterval > 0) {
                m_socket.ping();
                emit ping();
            }
        });
    QObject::connect(&m_socket, &QWebSocket::stateChanged, this,
                     &BGMRPCClient::stateChanged);
    QObject::connect(&m_socket, &QWebSocket::connected, this,
                     &BGMRPCClient::connected);
    QObject::connect(&m_socket, &QWebSocket::disconnected, this,
                     &BGMRPCClient::disconnected);

    QObject::connect(&m_socket, &QWebSocket::textMessageReceived, this,
                     [=](const QString& message) {
                         QJsonDocument jsonDoc =
                             QJsonDocument::fromJson(message.toUtf8());
                         QString type = jsonDoc["type"].toString();
                         if (type == "return")
                             emit returned(jsonDoc);
                         else if (type == "error")
                             emit error(jsonDoc);
                         else if (type == "signal")
                             emit remoteSignal(jsonDoc["object"].toString(),
                                               jsonDoc["signal"].toString(),
                                               jsonDoc["args"].toArray());
                     });

    QObject::connect(
        &m_socket, &QWebSocket::pong, this, [=](quint64 elapsedTime) {
            emit pong();
            if (m_aliveInterval > 0) {
                QTimer::singleShot(qMax<int>(m_aliveInterval - elapsedTime, 0),
                                   [=]() {
                                       m_socket.ping();
                                       emit ping();
                                   });
            }
        });
}

bool
BGMRPCClient::isConnected() {
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

int
BGMRPCClient::alive() const {
    return m_aliveInterval;
}

void
BGMRPCClient::setAlive(int interval) {
    m_aliveInterval = interval;

    emit aliveChanged();
}

void
BGMRPCClient::connectToHost(const QUrl& url) {
    if (url.scheme() == "wss") {
        QSslConfiguration conf;
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        // conf.setProtocol(QSsl::TlsV1SslV3);
        m_socket.setSslConfiguration(conf);
    }

    m_socket.open(url);
}

void
BGMRPCClient::disconnectFromHost() {
    m_socket.close();
}

Calling*
BGMRPCClient::callMethod(const QString& object, const QString& method,
                         const QVariantList& args) {
    QString mID = QString("#%1").arg(m_totalMID);
    m_totalMID++;
    QVariantMap callVariant;
    callVariant["object"] = object;
    callVariant["method"] = method;
    callVariant["args"] = args;
    callVariant["mID"] = mID;

    Calling* newCalling = new Calling(this, mID);
    m_socket.sendTextMessage(QJsonDocument::fromVariant(callVariant).toJson());

    emit calling(mID, object, method);

    return newCalling;
}
