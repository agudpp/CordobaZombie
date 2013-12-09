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

#include <cassert>
#include <cstdio>
#include <unistd.h>  // sleep()

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QLabel>
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QInputDialog>

#include "tinyxml/tinyxml.h"
#include "addressbook.h"


const char* AddressBook::xmlRootName = "addressbook_contactslist";
const char* AddressBook::xmlListName = "nodes";
const char* AddressBook::xmlListItem = "node";
const char* AddressBook::xmlListItemKey = "key";
const char* AddressBook::xmlListItemVal = "value";
//const QString AddressBook::fieldSep  = "|||";


//! [constructor and input fields]
AddressBook::AddressBook(QWidget *parent)
    : QWidget(parent)
	, currentMode(NavigationMode)
	, modified(false)
{
	// Fixed labels and images
    QLabel* nameLabel     = new QLabel(tr("Name:"));
    QLabel* addressLabel  = new QLabel(tr("Address:"));
    QLabel* contactsLabel = new QLabel(tr("Contacts:"));
    QLabel* logoImage     = new QLabel();
    logoImage->setPixmap(QPixmap("logo-cbazb.png"));

    // Input fields and internal storage containers
    nameLine    = new QLineEdit;
    addressText = new QTextEdit;
    nameLine->setReadOnly(true);
    addressText->setReadOnly(true);
    contactsList = new QComboBox;
    assert(contacts.isEmpty());
    assert(contactsList->count()==0);
    contactsList->setEnabled(false);
//! [constructor and input fields]

//! [user interaction buttons]
    // Create the graphic buttons
    addButton = new QPushButton(tr("Add"));
    addButton->show();
    submitButton = new QPushButton(tr("Submit"));
    submitButton->hide();
    cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->hide();
    editButton = new QPushButton(tr("Edit"));
    editButton->setEnabled(false);
    removeButton = new QPushButton(tr("Remove"));
    removeButton->setEnabled(false);
    nextButton = new QPushButton(tr("Next"));
	nextButton->setEnabled(false);
	prevButton = new QPushButton(tr("Previous"));
	prevButton->setEnabled(false);
	loadButton = new QPushButton(tr("&Load"));
	loadButton->show();
	saveButton = new QPushButton(tr("&Save"));
	saveButton->setEnabled(false);
	exitButton = new QPushButton(tr("&Exit"));
	exitButton->show();
    // Connect them to the corresponing class methods
    connect(addButton,    SIGNAL(clicked()), this, SLOT(addContact()));
    connect(submitButton, SIGNAL(clicked()), this, SLOT(submitContact()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(editButton,   SIGNAL(clicked()), this, SLOT(editContact()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeContact()));
    connect(nextButton,   SIGNAL(clicked()), this, SLOT(next()));
    connect(prevButton,   SIGNAL(clicked()), this, SLOT(prev()));
    connect(contactsList, SIGNAL(currentIndexChanged(const QString&)),
    		this,  		  SLOT(direct(const QString&)));
    connect(loadButton,   SIGNAL(clicked()), this, SLOT(load()));
    connect(saveButton,   SIGNAL(clicked()), this, SLOT(save()));
    connect(exitButton,   SIGNAL(clicked()), this, SLOT(shutdown()));
//! [user interaction buttons]

//! [buttons layout]
    // Contacts editing buttons
    QVBoxLayout* buttonLayout1 = new QVBoxLayout;
    buttonLayout1->addWidget(addButton);
    buttonLayout1->addWidget(editButton);
    buttonLayout1->addWidget(removeButton);
    buttonLayout1->addWidget(submitButton);
    buttonLayout1->addWidget(cancelButton);
    buttonLayout1->addStretch();  // push all buttons upwards
    // Contacts navigation buttons
    QGridLayout* navigatorsLayout = new QGridLayout;
    navigatorsLayout->addWidget(contactsLabel, 0, 0);
    navigatorsLayout->addWidget(contactsList, 1, 0);
    navigatorsLayout->addWidget(prevButton, 1, 1);
    navigatorsLayout->addWidget(nextButton, 1, 2);
    // Contacts disk manipulations
    QHBoxLayout* buttonLayout2 = new QHBoxLayout;
    buttonLayout2->addWidget(loadButton);
    buttonLayout2->addWidget(saveButton);
    buttonLayout2->addWidget(exitButton);
//! [buttons layout]

//! [general layout]
    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addWidget(logoImage, 0, 0, 2, 3, Qt::AlignTop);
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(nameLine, 0, 1);
    mainLayout->addWidget(addressLabel, 1, 0, Qt::AlignTop);
    mainLayout->addWidget(addressText, 1, 1);
    // Embedd buttons layouts
    mainLayout->addLayout(buttonLayout1, 1, 2);
    mainLayout->addLayout(navigatorsLayout, 2, 1);
    mainLayout->addLayout(buttonLayout2, 3, 1);
//! [general layout]

//![setting the layout]
    setLayout(mainLayout);
    setWindowTitle(tr("My super address-book"));
//    setStyleSheet("background-image:url(./sangre600x400.png)");
}
//! [setting the layout]


void
AddressBook::addContact()
{
	// Free current name and address fields
	oldName = nameLine->text();
	oldAddress = addressText->toPlainText();
	nameLine->clear();
	addressText->clear();
	// Change editor mode
	updateInterface(AddingMode);
}


void
AddressBook::submitContact()
{
	// Store in temp fileds the new name line and address field
    QString name = nameLine->text();
    QString address = addressText->toPlainText();
    if (name.isEmpty() || address.isEmpty()) {
        QMessageBox::information(this, tr("Empty Field"),
            tr("Please enter a name and address."));
        return;
    }

    // Adding new contact?
    if (currentMode == AddingMode) {
		// Make sure contact is fresh
		if (contacts.contains(name)) {
			QMessageBox::information(this, tr("Add Unsuccessful"),
				tr("Sorry, \"%1\" is already in your address book.").arg(name));
			return;
		} else {
			contacts.insert(name,address);
			assert(contacts.find(name).value() == address);
			contactsList->addItem(name);
			int index = contactsList->findText(name, Qt::MatchExactly);
			assert(index>=0);
			contactsList->setCurrentIndex(index);
			QMessageBox::information(this, tr("Add Successful"),
				tr("\"%1\" has been added to your address book.").arg(name));
			modified = true;
		}

	// Changing old contact?
    } else if (currentMode == EditingMode) {
    	if (oldName != name) {
			// Make sure contact is fresh
    		if (contacts.contains(name)) {
                QMessageBox::information(this, tr("Edit Failed"),
                    tr("Sorry, \"%1\" is already in your address book.").arg(name));
    		} else {
				contacts.remove(oldName);
				contacts.insert(name, address);
				int index = contactsList->findText(name, Qt::MatchExactly);
				assert(index>=0);
				contactsList->setCurrentIndex(index);
				QMessageBox::information(this, tr("Edit Successful"),
					tr("\"%1\" has been edited in your address book.").arg(oldName));
				modified = true;
    		}
    	} else if (oldAddress != address) {
            QMessageBox::information(this, tr("Edit Successful"),
                tr("\"%1\" has been edited in your address book.").arg(name));
            contacts[name] = address;
            modified = true;
    	}
    }

	// Change editor mode
    updateInterface(NavigationMode);
}


void
AddressBook::cancel()
{
	// Restore display fields initial configuration
    nameLine->setText(oldName);
    addressText->setText(oldAddress);
    nameLine->setReadOnly(true);
    addressText->setReadOnly(true);
	// Change editor mode
    updateInterface(NavigationMode);
}


void
AddressBook::editContact()
{
    oldName = nameLine->text();
    oldAddress = addressText->toPlainText();
    updateInterface(EditingMode);
}


void
AddressBook::removeContact()
{
    QString name = nameLine->text();
    QString address = addressText->toPlainText();
    if (contacts.contains(name)) {
    	// Display confirmation dialog
        int button = QMessageBox::question(this,
            tr("Confirm remove"),
            tr("Are you sure you want to remove \"%1\"?").arg(name),
            QMessageBox::Yes | QMessageBox::No);
        if (button == QMessageBox::Yes) {
        	int index = contactsList->findText(name, Qt::MatchExactly);
        	assert(index>=0);
        	contactsList->removeItem(index);
            contacts.remove(name);
            updateInterface(NavigationMode);
            prev();
            QMessageBox::information(this, tr("Remove Successful"),
                tr("\"%1\" has been removed from your address book.").arg(name));
            modified = true;
        }
    }
    updateInterface(NavigationMode);
}


void
AddressBook::next()
{
	assert(currentMode==NavigationMode);
	QMap<QString, QString>::iterator it = contacts.find(nameLine->text());
    if (it != contacts.end())
        it++;
    if (it == contacts.end()) {
        it = contacts.begin();
    }
    // Show new info
    assert(!it.key().isEmpty());
    nameLine->setText(it.key());
   	addressText->setText(it.value());
    // Update contactsList text
    int index = contactsList->findText(it.key());
    assert(index>=0);
    contactsList->setCurrentIndex(index);
}


void
AddressBook::prev()
{
	assert(currentMode==NavigationMode);
	QMap<QString, QString>::iterator it = contacts.find(nameLine->text());
    if (it == contacts.begin()) {
        it = contacts.end();  // recall final positon is actually empty
    }
    it--;
    // Show new info
    assert(!it.key().isEmpty());
    nameLine->setText(it.key());
    addressText->setText(it.value());
    // Update contactsList text
    int index = contactsList->findText(it.key());
    assert(index>=0);
    contactsList->setCurrentIndex(index);
}


void
AddressBook::direct(const QString& text)
{
	QMap<QString, QString>::iterator it = contacts.find(text);
	if (it != contacts.end()) {
		nameLine->setText(it.key());
		addressText->setText(it.value());
	}
}


void
AddressBook::load()
{
	bool ok(false);
    TiXmlDocument *XMLDoc(0);
    TiXmlElement *XMLRoot(0);

    assert(currentMode==NavigationMode);

	if (modified) {
        int button = QMessageBox::question(this,
            tr("Confirm action"),
            tr("Current contact list has changes. "
            	"Are you sure you want to load a new one?"),
            QMessageBox::Yes | QMessageBox::No);
        if (button == QMessageBox::No) {
        	return;
        }
	}

	// Get user input
	const QString fname = QInputDialog::getText(this,
				tr("Load contacts list"),
				tr("File name:"),
				QLineEdit::Normal,
				"",
				&ok);
	if (!ok || fname.isEmpty())
		return;
	if (!QFile::exists(fname)) {
        QMessageBox::information(this, tr("Error"),
            tr("No file \"%1\" found.").arg(fname));
        return;
	}

	// Parse specified file
	XMLDoc = new TiXmlDocument();
	ok = XMLDoc->LoadFile(fname.toStdString().c_str());
	if (!ok) {
		QMessageBox::information(this, tr("Error"),
			tr("File \"%1\" could not be parsed.").arg(fname));
		return;
	}
	assert(!XMLDoc->Error());
	XMLRoot = XMLDoc->RootElement();
	assert(XMLRoot);
	if (XMLRoot->Value() != QString(xmlRootName)) {
		QMessageBox::information(this, tr("Error"),
			tr("File \"%1\" could not be parsed: wrong XML header.").arg(fname));
	}

	// Change current info for loaded info
	contacts.clear();
	contactsList->clear();
	if (!parseContactsList(XMLRoot)) {
		QMessageBox::information(this, tr("Error"),
			tr("File \"%1\" could not be parsed.").arg(fname));
	}

	delete XMLDoc;
	modified = false;
	updateInterface(NavigationMode);
}


void
AddressBook::save()
{
	bool ok(false);
	QString fname;
    TiXmlDocument *XMLDoc(0);
    TiXmlElement *XMLRoot(0);

	assert(currentMode == NavigationMode);
	if (!modified) {
        QMessageBox::information(this, tr("Message"), tr("No chages to be saved."));
        return;
	} else if (contacts.empty()) {
        QMessageBox::information(this, tr("Message"), tr("No data to be saved."));
        return;
	}

	// Get user input
	do {
		fname = QInputDialog::getText(this, tr("Save contacts list"),
					tr("File name:"), QLineEdit::Normal, "", &ok);
		if (!ok)
			return;
		else if (QFile::exists(fname))
			QMessageBox::information(this, tr("Error"),
					tr("File \"%1\" already exists.").arg(fname));
	} while (QFile::exists(fname));

	// Create and open new file
	createEmptyXmlFile(fname);
	XMLDoc = new TiXmlDocument();
	XMLDoc->LoadFile(fname.toStdString().c_str());
	XMLRoot = XMLDoc->RootElement();
	assert(XMLRoot->Value() == QString(xmlRootName));
	XMLRoot = XMLRoot->FirstChildElement(xmlListName);
	assert(XMLRoot);

	// Fill in with current data
	QMap<QString,QString>::iterator it;
	for (it = contacts.begin() ; it != contacts.end() ; it++) {
		TiXmlElement item(xmlListItem);
		item.SetAttribute(xmlListItemKey, it.key().toStdString().c_str());
		item.SetAttribute(xmlListItemVal, it.value().toStdString().c_str());
		XMLRoot->InsertEndChild(item);
	}
	XMLDoc->SaveFile(fname.toStdString().c_str());
	delete XMLDoc;
	modified = false;
}


void
AddressBook::shutdown()
{
	assert(currentMode==NavigationMode);
	if (modified) {
        int button = QMessageBox::question(this,
            tr("Confirm action"),
            tr("Current contact list has changes. "
            	"Are you sure you want to quit?"),
            QMessageBox::Yes | QMessageBox::No);
        if (button == QMessageBox::No) {
        	return;
        }
	}
	close();
}


void
AddressBook::updateInterface(Mode mode)
{
	currentMode = mode;
	switch (mode)
	{
	case AddingMode:
	case EditingMode:
		// Update text areas
		contactsList->setEnabled(false);
		nameLine->setReadOnly(false);
		addressText->setReadOnly(false);
		nameLine->setFocus(Qt::OtherFocusReason);
		// Update buttons
		addButton->setEnabled(false);
		submitButton->show();
		cancelButton->show();
		editButton->setEnabled(false);
		removeButton->setEnabled(false);
		nextButton->setEnabled(false);
		prevButton->setEnabled(false);
		loadButton->setEnabled(false);
		saveButton->setEnabled(false);
		exitButton->setEnabled(false);
		break;

	case NavigationMode:
		// Update text areas
		if (contacts.isEmpty()) {
			nameLine->clear();
			addressText->clear();
		} else {
			contactsList->setEnabled(true);
		}
		nameLine->setReadOnly(true);
		addressText->setReadOnly(true);
		// Update buttons
		addButton->setEnabled(true);
		submitButton->hide();
		cancelButton->hide();
		editButton->setEnabled(contacts.size()>0);
		removeButton->setEnabled(contacts.size()>0);
		contactsList->setEnabled(contacts.size()>0);
		nextButton->setEnabled(contacts.size()>1);
		prevButton->setEnabled(contacts.size()>1);
		loadButton->setEnabled(true);
		saveButton->setEnabled(modified);
		exitButton->setEnabled(true);
		break;

	default:
		fprintf(stderr, "Invalid mode!\n");
		break;
	}
}


bool
AddressBook::parseContactsList(TiXmlElement* XMLRoot)
{
	assert(XMLRoot);
	TiXmlElement* elem = XMLRoot->FirstChildElement(xmlListName);
	if (!elem) {
		fprintf(stderr, "Invalid XML list name.\n");
		return false;
	}
	elem = elem->FirstChildElement(xmlListItem);
	if (!elem) {
		fprintf(stderr, "Invalid XML list item name.\n");
		return false;
	}
	while (elem) {
		// Retrieve data from XML file
		const char* name = elem->Attribute(xmlListItemKey);
		const char* addr = elem->Attribute(xmlListItemVal);
		if (strlen(name)>0) {
			// Insert in app
			if (contacts.contains(name)) {
				printf("Duplicate names not allowed (%s)\n", name);
			} else {
				contacts.insert(name, addr);
				contactsList->addItem(name);
			}
		}
		elem = elem->NextSiblingElement(xmlListItem);
	}
	return true;
}


void
AddressBook::createEmptyXmlFile(const QString& fname)
{
	assert(!QFile::exists(fname));
	// Create new file
	QFile newXML(fname);
	newXML.open(QFile::WriteOnly | QFile::Text);
	assert(newXML.isOpen());
	// Insert the XML skeleton
	QTextStream stream(&newXML);
	stream << "<" << xmlRootName << ">\n";
	stream << "\t<" << xmlListName << ">\n";
	stream << "\t</" << xmlListName << ">\n";
	stream << "</" << xmlRootName << ">\n";
	stream.flush();
	newXML.close();
}
