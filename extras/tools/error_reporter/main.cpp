#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QMessageBox>
#include "smtp.h"
#include <QTextCodec>


/*
class MsgReporter {
public:
    Q_OBJECT


    public slots:*/
    static
    void mailSent(const QString& str)
    {
        QMessageBox msgBox;
        msgBox.setText("Message: " + str);
        msgBox.exec();
    }
/*};*/


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    MsgReporter rep;
   Smtp* smtp = new Smtp("cbazombie@gmail.com", "Dosalpedo2", "smtp.gmail.com");
//   QObject::connect(smtp, &Smtp::statuss, mailSent);
 
 QStringList files;
	files << "C:\CordobaZombie\git\CordobaZombie\extras\tools\error_reporter\release\smtp.cpp";
    if( !files.isEmpty() )
        smtp->sendMail("cbazombie@gmail.com", "cbazombie@gmail.com" , "Asunto testeando", "report new error", files );
    else ;
        //smtp->sendMail(ui->uname->text(), ui->rcpt->text() , ui->subject->text(),ui->msg->toPlainText());


    return a.exec();
}
