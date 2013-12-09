/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <QMap>
#include <QWidget>
#include <QString>

// Forward declarations
QT_BEGIN_NAMESPACE
class QLineEdit;
class QTextEdit;
class QPushButton;
class QComboBox;
QT_END_NAMESPACE
class TiXmlElement;
class TiXmlDocument;

//! [class definition]
class AddressBook : public QWidget
{
    Q_OBJECT

    // Possible editor modes
    enum Mode {
    	NavigationMode,
    	AddingMode,
    	EditingMode
    };

public:
    // Valid XML tags, for contacts lists files.
    static const char* xmlRootName;
    static const char* xmlListName;
    static const char* xmlListItem;
    static const char* xmlListItemKey;
    static const char* xmlListItemVal;
//    // Name-Address separator string for saved/loaded files
//    static const QString fieldSep;

public:
    AddressBook(QWidget *parent = 0);

public slots:
	void shutdown();
	// Contacts editing commands
    void addContact();
    void submitContact();
    void cancel();
    void editContact();
    void removeContact();
    // Contacts navigation commands
    void next();
    void prev();
    void direct(const QString& name);
    // Contacts saving/loading in disk
    void load();
    void save();

private:
    // Mode switch
    void updateInterface(Mode mode);
    // Contacts saving/loading in disk (internals)
    bool parseContactsList(TiXmlElement* XMLRoot);
    void createEmptyXmlFile(const QString& fname);

private:
    QLineEdit* nameLine;
    QTextEdit* addressText;
    // Buttons for the slots
    QPushButton* addButton;
    QPushButton* submitButton;
    QPushButton* cancelButton;
    QPushButton* editButton;
    QPushButton* removeButton;
    QPushButton* nextButton;
    QPushButton* prevButton;
    QPushButton* loadButton;
    QPushButton* saveButton;
    QPushButton* exitButton;
    // Container for contacts list-like display
    QMap<QString,QString> contacts;
    QComboBox* contactsList;
    // Current mode
    Mode currentMode;
    // Temp fields, to restore values if user cancels operation
    QString oldName;
    QString oldAddress;
    bool modified;
};
//! [class definition]

#endif
