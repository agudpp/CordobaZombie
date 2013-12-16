/*
 * ErrorReporter.cpp
 *
 *  Created on: Dec 15, 2013
 *      Author: agustin
 */

#include "ErrorReporter.h"


namespace {
static const char* FILES_TO_REPORT[] = {
    "cz.log",
    "performance_stats.log",
    "Ogre.log",
    0
};
}

////////////////////////////////////////////////////////////////////////////////
bool
ErrorReporter::getFilesToSend(QStringList& files)
{
    // check if we have the files we need
//    const char

}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::deleteFiles(const QStringList& files)
{

}

////////////////////////////////////////////////////////////////////////////////
bool
ErrorReporter::sendEmail(const QStringList& files)
{
    // get the fields of the form
    const QString name = mUI.nameEdit->text();
    const QString mail = mUI.mailEdit->text();
    const QString errType = mUI.errTypesCombo->currentText();
    const QString steps = mUI.stepsEdit->text();
    const QString comments = mUI.commentsEdit->text();

    const QString from = "cbazombie@gmail.com";
    const QString to = "cbazombie@gmail.com";
    const QString subject = "Error[" + errType + "]";
    const QString body =
        "Reporter name: " + name + "\n"+
        "Reporter mail: " + mail + "\n" +
        "Error type: " + errType + "\n" +
        "Steps:\n" + steps + "\n\n\n" +
        "Comments:\n" + comments + "\n\n\n";

    // get the fields
    if (!mSmtp.sendMail(from, to, subject, body, files)) {
        showMessage("Se produjo un error al intentar mandar la informacion. Por"
                    " favor dirijase a la pagina www.cordobazombie.com.ar/testing para"
                    " mayor informacion de como reportar los errores. Disculpe las molestias");
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::showMessage(const QString& msg, bool error)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ErrorReporter::ErrorReporter(QWidget* parent) :
    QWidget(0)
,   mSmtp("cbazombie@gmail.com",
          "issueReporter",
          "smtp.google.com")
,   mSendOK(false)
{
    mUI.setupUi(this);

    // connect the signals
    // Smtp
    connect(&mSmtp,
            SIGNAL(status(Smtp::StatusType, const QString&)),
            this,
            SLOT(statusHandling(Smtp::StatusType, const QString&)));

    // buttons
    connect(mUI.cancelBtn, SIGNAL(clicked(bool)), this, SLOT(exitClicked(bool)));
    connect(mUI.reportBtn, SIGNAL(clicked(bool)), this, SLOT(reportClicked(bool)));
    connect(mUI.attachBtn, SIGNAL(clicked(bool)), this, SLOT(attachClicked(bool)));

}

////////////////////////////////////////////////////////////////////////////////
ErrorReporter::~ErrorReporter()
{

}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::reportClicked(bool)
{
    if (mSmtp.working()) {
        showMessage("Estamos procesando el pedido, aguarde porfavor.", false)
    }

    mSendOK = false;
    mUI.statusEdit->clear();

    // analyze files
    QStringList files;
    files << mAttachedFiles;
    if (!getFilesToSend(files)) {
        return;
    }


    // connect to the server using the smtp
    if (!sendEmail(files)) {
        return;
    }
    mSendOK = false;
}
void
ErrorReporter::exitClicked(bool)
{

}
void
ErrorReporter::attachClicked(bool)
{
    mAttachedFiles.clear();


}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::statusHandling(Smtp::StatusType statusType, const QString &msg)
{
    // we always want to show the status
    mUI.statusEdit->appendPlainText(msg + "\n");

    switch (statusType) {
    case Smtp::StatusType::STATUS_DISCONNECTED:
    {
        // do nothing?
        mAttachedFiles.clear();
    }
    break;
    case Smtp::StatusType::STATUS_ERROR:
    {
        mSendOK = false;
        showMessage("Se produjo un error al intentar mandar la informacion. Por"
            " favor dirijase a la pagina www.cordobazombie.com.ar/testing para"
            " mayor informacion de como reportar los errores. Disculpe las molestias");

    }
    break;
    case Smtp::StatusType::STATUS_UPDATE:
    {
        // do nothing
    }
    break;
    case Smtp::StatusType::STATUS_DONE:
    {
        mSendOK = true;
        mAttachedFiles.clear();
    }
    break;
    }
}

