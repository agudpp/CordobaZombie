/*
 * ErrorReporter.h
 *
 *  Created on: Dec 15, 2013
 *      Author: agustin
 */

#ifndef ERRORREPORTER_H_
#define ERRORREPORTER_H_

#include <QtGui/QWidget>
#include <QtGui/QMessageBox>
#include <QtCore/QStringList>

#include "smtp.h"

#include "ui_errorreporter.h"

class ErrorReporter : public QWidget
{
    Q_OBJECT

public:
    ErrorReporter(QWidget* parent = 0);
    virtual
    ~ErrorReporter();



    public slots:

    // buttons clicks
    void reportClicked(bool);
    void exitClicked(bool);
    void attachClicked(bool);

    // smtp status handling
    void statusHandling(Smtp::StatusType statusType, const QString &msg);

private:
    // @brief Fill the string list with all the files we have to send by default
    //
    bool
    getFilesToSend(QStringList& files);

    // @brief Create a folder with the files to be reported
    //
    bool
    createFolderReport(const QStringList& files, QDir& folderCreated);

    // @brief Remove files from hd
    //
    void
    deleteFiles(const QStringList& files);

    // @brief Send email with the current values of the form + attaching the
    //        files.
    //
    bool
    sendEmail(const QStringList& files);

    // @brief Show a message box
    //
    void
    showMessage(const QString& msg, bool error = true) const;

    // @brief Transform the current form into a formated string just to be ready
    //        to be sent by mail
    //
    QString
    transformFormToString(void) const;

    // @brief Add a message in the status bar
    //
    inline void
    addStatusMsg(const QString& msg);

    // @brief show critical error with some additional information, this method
    //        should be called when the error reporter will not be able to send
    //        anything and the tester should proceed manually to report the errors
    //
    void
    showCriticalError(const QString& additionalInfo = "") const;

    // @brief create a backup from the current files into the error_reporter folder
    //
    void
    createReportBackup(void);

    // @brief Get the machine information.
    // @param info      The machine information
    // @return true on success | false otherwise
    //
    bool
    getMachineInfo(QString& info);

    // @brief Save/restore the personal information of saved by this class
    //
    void
    saveCurrentPersonalInfo(void);
    void
    restorePersonalInfo(void);

private:
    Ui::ErrorReporter mUI;
    Smtp mSmtp;
    bool mSendOK;
    QStringList mAttachedFiles;
    QStringList mFilesSent;
};





inline void
ErrorReporter::addStatusMsg(const QString& msg)
{
    mUI.statusEdit->appendPlainText(msg);
}

#endif /* ERRORREPORTER_H_ */
