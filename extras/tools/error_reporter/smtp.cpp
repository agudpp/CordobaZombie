/*
Copyright (c) 2013 Raivis Strogonovs
 
http://morf.lv
 
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/
 
 
 
#include "smtp.h"
 

Smtp::Smtp(const QString &user,
           const QString &pass,
           const QString &host,
           int port,
           int timeout) :
    mTextStream(0)
,   socket(0)

{    
    socket = new QSslSocket(this);
 
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected() ) );
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(errorReceived(QAbstractSocket::SocketError)));   
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(disconnected()), this,SLOT(disconnected()));
 
 
    this->user = user;
    this->pass = pass;
 
    this->host = host;
    this->port = port;
    this->timeout = timeout;

    state = Close;
}
 
bool
Smtp::sendMail(const QString &from,
               const QString &to,
               const QString &subject,
               const QString &body,
               QStringList files)
{
    message = "To: " + to + "\n";    
    message.append("From: " + from + "\n");
    message.append("Subject: " + subject + "\n");
 
    //Let's intitiate multipart MIME with cutting boundary "frontier"
    message.append("MIME-Version: 1.0\n");
    message.append("Content-Type: multipart/mixed; boundary=frontier\n\n");
 
    message.append( "--frontier\n" );
    //message.append( "Content-Type: text/html\n\n" );  //Uncomment this for HTML formating, coment the line below
    message.append( "Content-Type: text/plain\n\n" );
    message.append(body);
    message.append("\n\n");
 
    if(!files.isEmpty())
    {
        qDebug() << "Files to be sent: " << files.size();
        emit status(StatusType::STATUS_UPDATE,
                    QString("Analizando archivos a enviar: ") +
                    QString::number(files.size()));
        foreach(QString filePath, files) {
            QFile file(filePath);
            if (file.exists()) {
                if (!file.open(QIODevice::ReadOnly)) {
                    emit status(StatusType::STATUS_ERROR,
                                QString("Archivo ") + filePath
                                    + QString(" no fue "
                                              "encontrado o no se pudo abrir"));
                    qDebug("Couldn't open the file");
                    return false;
                }
                QByteArray bytes = file.readAll();
                message.append("--frontier\n");
                message.append("Content-Type: application/octet-stream\nContent-Disposition: attachment; filename="
                    + QFileInfo(file.fileName()).fileName()
                    + ";\nContent-Transfer-Encoding: base64\n\n");
                message.append(bytes.toBase64());
                message.append("\n");
            }
        }
    } else {
        qDebug() << "No attachments found";
    }
 
 
    message.append( "--frontier--\n" );
 
    message.replace( QString::fromLatin1( "\n" ), QString::fromLatin1( "\r\n" ) );
    message.replace( QString::fromLatin1( "\r\n.\r\n" ),QString::fromLatin1( "\r\n..\r\n" ) );
 
 
    this->from = from;
    rcpt = to;
    state = Init;
    socket->connectToHostEncrypted(host, port); //"smtp.gmail.com" and 465 for gmail TLS
    if (!socket->waitForConnected(timeout)) {
         qDebug() << socket->errorString();
         emit status(StatusType::STATUS_ERROR,
                     QString("Error al intentar conectarse al servidor: ") +
                             socket->errorString());
     }
 
    if (mTextStream != 0) {
        delete mTextStream;
        mTextStream = 0;
    }
    mTextStream = new QTextStream( socket );

    return true;
}

// @brief check if the smtp is working
//
bool
Smtp::working(void) const
{
    return state != Close;
}

 
Smtp::~Smtp()
{
    delete mTextStream;
    delete socket;
}
void Smtp::stateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() <<"stateChanged " << socketState;
    emit status(StatusType::STATUS_UPDATE, QString("Socket state changed to ") +
                QString::number(socketState));
}
 
void Smtp::errorReceived(QAbstractSocket::SocketError socketError)
{
    qDebug() << "error " <<socketError;

    // advise about the error
    emit status(StatusType::STATUS_UPDATE, QString("Se produjo algun error en el"
        " socket: ") + socket->errorString());
}
 
void Smtp::disconnected()
{
 
    qDebug() <<"disconneted";
    qDebug() << "error "  << socket->errorString();

    // advise about the error
    emit status(StatusType::STATUS_DISCONNECTED,
                QString("Socket desconectado: ") + socket->errorString());
    state = Close;
}
 
void Smtp::connected()
{    
    qDebug() << "Connected ";
    emit status(StatusType::STATUS_UPDATE, QString("Socket connectado"));
}
 
void Smtp::readyRead()
{
 
     qDebug() <<"readyRead";
    // SMTP is line-oriented
 
    QString responseLine;
    do
    {
        responseLine = socket->readLine();
        response += responseLine;
    }
    while ( socket->canReadLine() && responseLine[3] != ' ' );
 
    responseLine.truncate( 3 );
 
    qDebug() << "Server response code:" <<  responseLine;
    qDebug() << "Server response: " << response;

    emit status(StatusType::STATUS_UPDATE,
                QString("Server response code: ") + responseLine);
    emit status(StatusType::STATUS_UPDATE,
                QString("Server response: ") + response);
 
    if ( state == Init && responseLine == "220" )
    {
        // banner was okay, let's go on
        *mTextStream << "EHLO localhost" <<"\r\n";
        mTextStream->flush();
 
        state = HandShake;
    }
    //No need, because I'm using socket->startClienEncryption() which makes the SSL handshake for you
    /*else if (state == Tls && responseLine == "250")
    {
        // Trying AUTH
        qDebug() << "STarting Tls";
        *t << "STARTTLS" << "\r\n";
        t->flush();
        state = HandShake;
    }*/
    else if (state == HandShake && responseLine == "250")
    {
        socket->startClientEncryption();
        if(!socket->waitForEncrypted(timeout))
        {
            qDebug() << socket->errorString();
            emit status(StatusType::STATUS_UPDATE,
                        QString("Timeout error: ") +  socket->errorString());
            state = Close;
        }
 
 
        //Send EHLO once again but now encrypted
 
        *mTextStream << "EHLO localhost" << "\r\n";
        mTextStream->flush();
        state = Auth;
    }
    else if (state == Auth && responseLine == "250")
    {
        // Trying AUTH
        qDebug() << "Auth";
        emit status(StatusType::STATUS_UPDATE,
                    QString("Autorizando datos"));
        *mTextStream << "AUTH LOGIN" << "\r\n";
        mTextStream->flush();
        state = User;
    }
    else if (state == User && responseLine == "334")
    {
        //Trying User        
        //GMAIL is using XOAUTH2 protocol, which basically means that password
        // and username has to be sent in base64 coding
        //https://developers.google.com/gmail/xoauth2_protocol
        *mTextStream << QByteArray().append(user).toBase64()  << "\r\n";
        mTextStream->flush();
 
        state = Pass;
    }
    else if (state == Pass && responseLine == "334")
    {
        //Trying pass
        *mTextStream << QByteArray().append(pass).toBase64() << "\r\n";
        mTextStream->flush();
 
        state = Mail;
    }
    else if ( state == Mail && responseLine == "235" )
    {
        // HELO response was okay (well, it has to be)

        // Apperantly for Google it is mandatory to have MAIL FROM and RCPT email
        // formated the following way -> <email@gmail.com>

        emit status(StatusType::STATUS_UPDATE,
                    QString("Preparando mail"));
        *mTextStream << "MAIL FROM:<" << from << ">\r\n";
        mTextStream->flush();
        state = Rcpt;
    }
    else if ( state == Rcpt && responseLine == "250" )
    {
        //Apperantly for Google it is mandatory to have MAIL FROM and RCPT email
        // formated the following way -> <email@gmail.com>
        *mTextStream << "RCPT TO:<" << rcpt << ">\r\n"; //r
        mTextStream->flush();
        state = Data;
    }
    else if ( state == Data && responseLine == "250" )
    {
 
        *mTextStream << "DATA\r\n";
        mTextStream->flush();
        state = Body;
    }
    else if ( state == Body && responseLine == "354" )
    {
 
        *mTextStream << message << "\r\n.\r\n";
        mTextStream->flush();
        state = Quit;
    }
    else if ( state == Quit && responseLine == "250" )
    {
 
        *mTextStream << "QUIT\r\n";
        mTextStream->flush();
        // here, we just close.
        state = Close;
        emit status(StatusType::STATUS_DONE,
                    QString("Envio exitoso!"));
    }
    else if ( state == Close )
    {
        return;
    }
    else
    {
        // something broke.
        emit status(StatusType::STATUS_ERROR,
                            QString("Se produjo un error al intentar enviar los datos: ")+
                                    response);
        state = Close;
    }
    response = "";
}
