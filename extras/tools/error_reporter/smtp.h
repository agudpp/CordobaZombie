/*
 Copyright (c) 2013 Raivis Strogonovs
 
 http://morf.lv
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
  Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SMTP_H
#define SMTP_H

#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslSocket>
#include <QtCore/QStringList>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>

class Smtp : public QObject
{
    Q_OBJECT

public:

    // Reporting status error
    //
    enum StatusType {
        STATUS_ERROR = 0,
        STATUS_UPDATE,
        STATUS_DONE,
        STATUS_DISCONNECTED,
    };


public:
    Smtp(const QString &user,
         const QString &pass,
         const QString &host,
         int port = 465,
         int timeout = 30000);
    ~Smtp();

    bool
    sendMail(const QString &from,
             const QString &to,
             const QString &subject,
             const QString &body,
             QStringList files = QStringList());

    // @brief check if the smtp is working
    //
    bool
    working(void) const;

public slots:
    void
    stateChanged(QAbstractSocket::SocketState socketState);
    void
    errorReceived(QAbstractSocket::SocketError socketError);
    void
    disconnected();
    void
    connected();
    void
    readyRead();

signals:
    void
    status(Smtp::StatusType statusType, const QString &msg);

private:
    int timeout;
    QString message;
    QTextStream *mTextStream;
    QSslSocket *socket;
    QString from;
    QString rcpt;
    QString response;
    QString user;
    QString pass;
    QString host;
    int port;
    enum states
    {
        Tls,
        HandShake,
        Auth,
        User,
        Pass,
        Rcpt,
        Mail,
        Data,
        Init,
        Body,
        Quit,
        Close
    };
    int state;

};
#endif
