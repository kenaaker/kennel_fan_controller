#include "kennel_fan_sslsock.h"
#include <QSslKey>
#include <QFile>

#define CACERTIFICATES_FILE "kennel_fan_ca.pem"
#define LOCALCERTIFICATE_FILE "kennel_fan_system_certificate.pem"
#define PRIVATEKEY_FILE "kennel_fan_system_certificate.key"

void kennel_fan_sslsock::incomingConnection(qintptr socketDescriptor) {

    qDebug("kennel_fan_sslsock::incomingConnection(%d)", socketDescriptor);
    socket = new QSslSocket();
    if (!socket) {
        qWarning("not enough memory to create new QSslSocket");
    } else {
        socket->setProtocol(QSsl::AnyProtocol);
        connectSocketSignals();
        if (!socket->setSocketDescriptor(socketDescriptor)) {
            qWarning("couldn't set socket descriptor");
            delete socket;
        } else {
            addPendingConnection(socket);
            startServerEncryption();
        } /* endif */
    } /* endif */
} /* incomingConnection */

void kennel_fan_sslsock::send_line(QString &a_line) {
    socket->write(a_line.toLatin1(), a_line.size());
}

QString kennel_fan_sslsock::recv_line() {
    QByteArray in_buff(256, ' ');
    socket->readLine(in_buff.size());
    return QString(in_buff);
}

void kennel_fan_sslsock::startServerEncryption() {
    bool b;
    QFile file(PRIVATEKEY_FILE);

    file.open(QIODevice::ReadOnly);
    if (!file.isOpen()) {
        qWarning("couldn't open %s", PRIVATEKEY_FILE);
        socket->disconnectFromHost();
    } else {
        QSslKey key(&file, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, "");
        qDebug() << "Set up key";
        if (key.isNull()) {
            qDebug() << "Key isn't good.";
            qWarning("key is null");
            socket->disconnectFromHost();
            return;
        } else {
            b = socket->addCaCertificates(CACERTIFICATES_FILE);
            qDebug() << "CA setup";
            if (!b) {
                qWarning("Couldn't add CA certificates (\"%s\")", CACERTIFICATES_FILE);
            } else {
                qDebug() << "CA added.";
                socket->setLocalCertificate(LOCALCERTIFICATE_FILE);
                socket->setPrivateKey(key);
                socket->setPeerVerifyMode(QSslSocket::VerifyPeer);
                socket->startServerEncryption();
                qDebug() << "Started Encryption.";
            }
        }
    }
}

void kennel_fan_sslsock::connectSocketSignals() {
    connect(socket, SIGNAL(encrypted()), this, SLOT(slot_encrypted()));
    connect(socket, SIGNAL(encryptedBytesWritten(qint64)),
            this, SLOT(slot_encryptedBytesWritten(qint64)));
    connect(socket, SIGNAL(modeChanged(QSslSocket::SslMode)),
            this, SLOT(slot_modeChanged(QSslSocket::SslMode)));
    connect(socket, SIGNAL(peerVerifyError(const QSslError &)),
            this, SLOT(slot_peerVerifyError (const QSslError &)));
    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)),
            this, SLOT(slot_sslErrors(const QList<QSslError> &)));
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(slot_readyRead()));
    connect(socket, SIGNAL(connected()),
            this, SLOT(slot_connected()));
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(slot_disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slot_error(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(hostFound()),
            this, SLOT(slot_hostFound()));
    connect(socket, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)),
            this, SLOT(slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(slot_stateChanged(QAbstractSocket::SocketState)));
//    connect_to_db();
}

void kennel_fan_sslsock::slot_encrypted() {
    qDebug("kennel_fan_sslsock::slot_encrypted");
}

void kennel_fan_sslsock::slot_encryptedBytesWritten(qint64 written) {
    qDebug("kennel_fan_sslsock::slot_encryptedBytesWritten(%ld)", (long) written);
}

void kennel_fan_sslsock::slot_modeChanged(QSslSocket::SslMode mode) {
    qDebug("kennel_fan_sslsock::slot_modeChanged(%d)", mode);
}

void kennel_fan_sslsock::slot_peerVerifyError(const QSslError &) {
    qDebug("kennel_fan_sslsock::slot_peerVerifyError");
}

void kennel_fan_sslsock::slot_sslErrors(const QList<QSslError> &) {
    qDebug("kennel_fan_sslsock::slot_sslErrors");
}

void kennel_fan_sslsock::slot_readyRead() {
    QByteArray rem_command;

    qDebug("kennel_fan_sslsock::slot_readyRead");

    rem_command = socket->readLine(256);
    if (rem_command.size() < 2) {
        abort();
    } else {
        current_command = QString(rem_command);
        emit command_received(current_command);
    } /* endif */
}

void kennel_fan_sslsock::slot_connected() {
    qDebug("kennel_fan_sslsock::slot_connected");
}

void kennel_fan_sslsock::slot_disconnected() {
    qDebug("kennel_fan_sslsock::slot_disconnected");
}

void kennel_fan_sslsock::slot_error(QAbstractSocket::SocketError err) {
    qDebug() << "kennel_fan_sslsock::slot_error(" << err << ")";
    qDebug() << socket->errorString();
}

void kennel_fan_sslsock::slot_hostFound() {
    qDebug("kennel_fan_sslsock::slot_hostFound");
}

void kennel_fan_sslsock::slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *) {
    qDebug("kennel_fan_sslsock::slot_proxyAuthenticationRequired");
}

void kennel_fan_sslsock::slot_stateChanged(QAbstractSocket::SocketState state) {
    qDebug() << "kennel_fan_sslsock::slot_stateChanged(" << state << ")";
}
