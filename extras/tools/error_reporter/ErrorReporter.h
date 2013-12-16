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
    showMessage(const QString& msg, bool error = true);

private:
    Ui::ErrorReporter mUI;
    Smtp mSmtp;
    bool mSendOK;
    QStringList mAttachedFiles;
};

#endif /* ERRORREPORTER_H_ */
