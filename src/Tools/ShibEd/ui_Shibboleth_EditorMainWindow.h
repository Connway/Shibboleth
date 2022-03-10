/********************************************************************************
** Form generated from reading UI file 'Shibboleth_EditorMainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.2.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHIBBOLETH_EDITORMAINWINDOW_H
#define UI_SHIBBOLETH_EDITORMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EditorMainWindow
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *EditorMainWindow)
    {
        if (EditorMainWindow->objectName().isEmpty())
            EditorMainWindow->setObjectName(QString::fromUtf8("EditorMainWindow"));
        EditorMainWindow->resize(800, 600);
        centralwidget = new QWidget(EditorMainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        EditorMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(EditorMainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        EditorMainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(EditorMainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        EditorMainWindow->setStatusBar(statusbar);

        retranslateUi(EditorMainWindow);

        QMetaObject::connectSlotsByName(EditorMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *EditorMainWindow)
    {
        EditorMainWindow->setWindowTitle(QCoreApplication::translate("EditorMainWindow", "EditorMainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EditorMainWindow: public Ui_EditorMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHIBBOLETH_EDITORMAINWINDOW_H
