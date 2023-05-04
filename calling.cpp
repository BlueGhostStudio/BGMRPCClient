#include "calling.h"

#include "bgmrpcclient.h"

using namespace NS_BGMRPCClient;

Calling::Calling(BGMRPCClient* client, const QString& mID, QObject* parent)
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
}
