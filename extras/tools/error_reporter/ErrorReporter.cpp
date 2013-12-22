/*
 * ErrorReporter.cpp
 *
 *  Created on: Dec 15, 2013
 *      Author: agustin
 */

#include "ErrorReporter.h"

#include <time.h>
#include <stdio.h>

#include <QDir>
#include <QtGui/QFileDialog>
#include <QDateTime>
#include <QDebug>
#include <QProcess>



// Define the maximum size of files we can send
#define MAX_FILE_SIZE       1024*1024*5     // 5 MB


////////////////////////////////////////////////////////////////////////////////
bool
ErrorReporter::getFilesToSend(QStringList& aFiles)
{
    // check if we have the files we need, or get at least all the files with
    // the *.log files for now.
    //
    addStatusMsg("Obteniendo los archivos de log!");
    QDir currentPath(".");
    QStringList filters;
    filters << "*.log";
    QStringList files = currentPath.entryList(filters);

    addStatusMsg("Encontramos: " + QString::number(files.size()) + "\n"
        "Adjuntando:");

    for (QString& file : files) {
        addStatusMsg("\t" + file);
        aFiles << file;
    }

    // check now if we have the cfg files and xml files also
    filters.clear();
    filters << "*.cfg";
    filters << "*.xml";
    files = currentPath.entryList(filters);
    addStatusMsg("Buscando archivos de configuracion...");
    for (QString& file : files) {
        addStatusMsg("\t" + file);
        aFiles << file;
    }


    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::deleteFiles(const QStringList& files)
{
    addStatusMsg("Removiendo logs, limpiando un poco:");
    for (const QString& file : files) {
        addStatusMsg("\tBorrando: " + file);
        QFile::remove(file);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
ErrorReporter::sendEmail(const QStringList& files)
{
    const QString from = "cbazombie@gmail.com";
    const QString to = "cbazombie@gmail.com";
    const QString subject = "Error[" + mUI.errTypesCombo->currentText() + "]";

    // we will try to get the machine information
    QString machineInfo;
    getMachineInfo(machineInfo);

    QString dataToSend = transformFormToString();
    dataToSend.append("\nMachineInformation:\n");
    dataToSend += machineInfo;

    // get the fields
    if (!mSmtp.sendMail(from, to, subject, dataToSend, files)) {
        showMessage("Se produjo un error al intentar mandar la informacion. Por"
                    " favor dirijase a la pagina www.cordobazombie.com.ar/testing para"
                    " mayor informacion de como reportar los errores. Disculpe las molestias");
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
ErrorReporter::createFolderReport(const QStringList& files, QDir& folderCreated)
{
    // we will copy all the files into a new folder where we will generate the report
    //

    // create the new folder for report
    QDir reportFolder("./error_reports");
    if (!reportFolder.exists()) {
        addStatusMsg("Creando directorio de reporte de errores (error_reports)");
        QDir(".").mkdir("error_reports");
    }

    // create the new folder for this particular report
    time_t rawtime;
    time (&rawtime);

    // print the time into a string buff
    char str[512];
    sprintf(str, "%s", ctime(&rawtime));
    QString newFolder(str);
    addStatusMsg("Creando directorio del nuevo reporte " +
                 reportFolder.path() + "/" + newFolder);
    reportFolder.mkdir(newFolder);
    QDir folderToSave(reportFolder.path() + "/" + newFolder);

    if (!folderToSave.exists()) {
        addStatusMsg("Error generando el directorio para el reporte :(");
        return false;
    }

    // now put all the files in there
    for (const QString& file : files) {
        addStatusMsg("Copiando archivo " + file + " en " + folderToSave.path());
        if (!QFile::copy(file, folderToSave.filePath(file))) {
            addStatusMsg("Error copiando el ultimo archivo... algo salio mal");
            return false;
        }
    }

    folderCreated = folderToSave;

    addStatusMsg("Todo fue copiado correctamente.");
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::showMessage(const QString& msg, bool error) const
{
    error = error;
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
}

////////////////////////////////////////////////////////////////////////////////
QString
ErrorReporter::transformFormToString(void) const
{
    const QString subject = "Error[" + mUI.errTypesCombo->currentText() + "]";
    const QString body =
        "Reporter name: " + mUI.nameEdit->text() + "\n"+
        "Reporter mail: " + mUI.mailEdit->text() + "\n" +
        "Error type: " + mUI.errTypesCombo->currentText() + "\n" +
        "Steps:\n" + mUI.stepsEdit->toPlainText() + "\n\n\n" +
        "Comments:\n" + mUI.commentsEdit->toPlainText() + "\n\n\n";

    return body;
}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::showCriticalError(const QString& additionalInfo) const
{
    showMessage("Se produjo un error al intentar mandar la informacion. Por"
        " favor dirijase a la pagina www.cordobazombie.com.ar/testing para"
        " mayor informacion de como reportar los errores. Disculpe las molestias.\n"+
        additionalInfo);
}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::createReportBackup(void)
{
    // we couldn't send the email, we will try to create a back up?
    QDir reportFolder;
    if (!createFolderReport(mFilesSent, reportFolder)) {
        showCriticalError("Error al intentar crear el directorio de reporte.");
        return;
    }
    // else we could create the report, now we will save the information into
    // a log file (the form information)
    //
    const QString form = transformFormToString();
    QFile formLog(reportFolder.filePath("form.log"));
    if (formLog.open(QIODevice::WriteOnly)) {
        formLog.write(form.toAscii(), form.size());
        formLog.close();
        showCriticalError("Archivo form.log guardado con toda la inforamcion del "
            "formulario. Siga las instrucciones de la web para poder reportar"
            " los errores manualmente. Disculpe las molestias.");
    } else {
        showCriticalError();
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
ErrorReporter::getMachineInfo(QString& info)
{
    // What we gonna do here is create a file with the machine information we
    // need. Trying different approaches, if non succeed then will have to return
    // false for now and ask the user to add the machine information in the
    // comments.
    // This file will be created only once and will be used all the time we
    // need to send the information.

    // clear the data
    info = "";

    // check if the file exists
    addStatusMsg("Intentando obtener informacion de la maquina.");
    QFile infoFile("machine_info.info");
    if (infoFile.open(QIODevice::ReadOnly)) {
        // we read the file and return
        info = infoFile.readAll();
        addStatusMsg("Informacion de la maquina obtenida");
        return true;
    }

    // else we need to create the machine information, we will list all the possible
    // commands we will execute in priority order. Just in case some of them
    // fails, we will continue with the next one
    // we will print a little message box to advise the user that this could take
    // some seconds
    //
    showMessage("Estamos por recopilar informacion del hardware de su maquina "
        "para poder hacer un analisis mas preciso del error.\n"
        "Esta operacion puede demorar unos segundos, pero solo se hara una unica"
        " vez (solo en el primer reporte).\n"
        "Disculpe las molestias.");

    struct Command {
        QString command;
        QStringList args;
        Command(const QString& cmd, const QStringList& arg) : command(cmd), args(arg){}
    };
    QList<Command> commands;

#ifdef WIN32
    // define the commands for windows
    // dxdiag
    commands << Command("dxdiag", QStringList() << "/t" << "machine_info.info");

    // sysinfo
    commands << Command("sysinfo", QStringList() << "\"> machine_info.info\"");

#else
#error "This feature wasn't implemented yet on linux, basically we need to "\
    "set the commands correctly and we will be fine"
#endif

    // now run the commands
    bool executedOk = false;
    addStatusMsg("Ejecutando commandos para la obtencion de la informacion");
    for (QList<Command>::iterator beg = commands.begin(), end = commands.end();
        beg != end && !executedOk; ++beg) {
        QProcess proc;
        addStatusMsg(QString("Probando con: ") + beg->command);
        proc.start(beg->command, beg->args);
        if (!proc.waitForStarted()) {
            // continue with the next command
            addStatusMsg("No se pudo ejecutar el comando..");
            continue;
        }
        if (!proc.waitForFinished()) {
            // continue with the next one
            addStatusMsg("No se pudo ejecutar el commando");
            continue;
        }

        addStatusMsg("Commando ejecutado correctamente, almacenando informacion");
        executedOk = true;
    }

    if (!executedOk) {
        addStatusMsg("No pudimos obtener informacion de la maquina.");
        showMessage("No hemos logrado obtener informacion del sistema. Por favor"
            " te pedimos que nos suministres la siguiente informacion en el campo"
            " comentarios:\n"
            "\t- Procesador de la maquina.\n"
            "\t- Memoria de ram.\n"
            "\t- Placa de video (modelo).\n"
            "\t- Sistema operativo (por ejemplo, Windows 7 64bits Professional.\n"
            "\nEsa informacion nos es util a la hora de poder determinar con mayor"
            " presicion en que plataforma y con que hardware se producen los fallos.");
        info = "";
    } else {
        // read the information again
        infoFile.close();
        if (infoFile.open(QIODevice::ReadOnly)) {
            // we read the file and return
            info = infoFile.readAll();
            addStatusMsg("Informacion de la maquina obtenida");
        }
    }

    // everything fine
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::saveCurrentPersonalInfo(void)
{
    QFile tmpdata("error_reporter.tmp");
    tmpdata.open(QIODevice::WriteOnly);
    QString data = "name=" + mUI.nameEdit->text() + "\n" +
        "mail=" + mUI.mailEdit->text() + "\n";
    tmpdata.write(data.toUtf8(), data.size());
    tmpdata.close();
}
////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::restorePersonalInfo(void)
{
    QFile tmpdata("error_reporter.tmp");
    if (!tmpdata.open(QIODevice::ReadOnly)) {
        return;
    }
    // else restoring data
    QString str = tmpdata.readLine(256);
    QStringList aux = str.split('=');
    if (aux.size() == 2) {
        QString name = aux[1].trimmed();
        mUI.nameEdit->setText(name);
    }
    str = tmpdata.readLine(256);
    aux = str.split('=');
    if (aux.size() == 2) {
        QString mail = aux[1].trimmed();
        mUI.mailEdit->setText(mail);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ErrorReporter::ErrorReporter(QWidget* parent) :
    QWidget(parent)
,   mSmtp("cbazombie@gmail.com",
          "issueReporter",
          "smtp.gmail.com")
,   mSendOK(false)
{
    mUI.setupUi(this);

    // set a tool tip information by code to the webpage link
    mUI.label_7->setToolTip("www.cordobazombie.com.ar/testing");

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

    // restore previous data if we have
    restorePersonalInfo();
}

////////////////////////////////////////////////////////////////////////////////
ErrorReporter::~ErrorReporter()
{
    saveCurrentPersonalInfo();
}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::reportClicked(bool)
{
    if (mSmtp.working()) {
        showMessage("Estamos procesando el pedido, aguarde porfavor.", false);
        return;
    }

    mSendOK = false;
    mUI.statusEdit->clear();

    // analyze files
    mFilesSent.clear();
    mFilesSent << mAttachedFiles;
    if (!getFilesToSend(mFilesSent)) {
        showCriticalError();
        return;
    }

    // connect to the server using the smtp
    if (!sendEmail(mFilesSent)) {
        createReportBackup();
        return;
    }
}
void
ErrorReporter::exitClicked(bool)
{
    close();
}
void
ErrorReporter::attachClicked(bool)
{
    // check if we have to remove the current file or attach a new one
    if (!mAttachedFiles.empty()) {
        // clear the caption of the button
        mUI.attachBtn->setText("Adjuntar archivo");
        addStatusMsg("Archivo adjuntado " + mAttachedFiles.first() + " no se enviara.");
        mAttachedFiles.clear();
        return;
    }

    // get the file to be attached
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Archivo"),
                                                    ".");
    // if filename is empty we do nothing
    if (filename.isEmpty()) {
        // do nothing
        return;
    }

    QFile file(filename);
    if (file.size() > MAX_FILE_SIZE) {
        showMessage("El archivo es muy grande como para mandarlo.", true);
        return;
    }

    // else we can use this file
    mAttachedFiles << filename;
    mUI.attachBtn->setText("Remover archivo: " + filename);
    addStatusMsg("Archivo adjuntado para ser enviado: " + filename);
}

////////////////////////////////////////////////////////////////////////////////
void
ErrorReporter::statusHandling(Smtp::StatusType statusType, const QString &msg)
{
    // we always want to show the status
    mUI.statusEdit->appendPlainText(msg);

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
        // we couldn't send the email, we will try to create a back up?
        createReportBackup();
        mAttachedFiles.clear();
        mUI.attachBtn->setText("Adjuntar archivo");
        mFilesSent.clear();
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
        // everything was fine! advise the user and delete the log files
        addStatusMsg("Datos enviados con exito! Muchisimas gracias por aportar con el "
                     "proyecto!");
        showMessage("Datos enviados con exito! Muchisimas gracias por aportar con el "
                    "proyecto!",
                    false);

        // clear the text of the fields
        mUI.commentsEdit->clear();
        mUI.stepsEdit->clear();

        // removing the files
        QStringList filesToRemove;
        for (QString& file : mFilesSent) {
            // we will remove only the log files
            if (file.contains(".log")) {
                // we will remove this
                filesToRemove << file;
            }
        }
        deleteFiles(filesToRemove);
        mAttachedFiles.clear();
        mUI.attachBtn->setText("Adjuntar archivo");
        mFilesSent.clear();
    }
    break;
    }
}
