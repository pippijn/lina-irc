/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "mainwindow.h"
#include "application.h"
#include "sessiontabwidget.h"
#include "maintabwidget.h"
#include "sharedtimer.h"
#include "connectioninfo.h"
#include "homepage.h"
#include "session.h"
#include <QInputDialog>
#include <QSettings>
#include <QShortcut>
#include <irccommand.h>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), tabWidget(new MainTabWidget(this)), trayIcon(0)
{
    setCentralWidget(tabWidget);
    connect(tabWidget, SIGNAL(tabMenuRequested(int,QPoint)), this, SLOT(onTabMenuRequested(int,QPoint)));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
    connect(tabWidget, SIGNAL(alertStatusChanged(bool)), this, SLOT(activateAlert(bool)));

    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        trayIcon = new TrayIcon(this);
        trayIcon->setIcon(QApplication::windowIcon());
        trayIcon->setVisible(true);
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    }

    QShortcut* shortcut = new QShortcut(QKeySequence(tr("Ctrl+Q")), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

    QSettings settings;
    if (settings.contains("geometry"))
        restoreGeometry(settings.value("geometry").toByteArray());

    connectTo();
}

MainWindow::~MainWindow()
{
}

QSize MainWindow::sizeHint() const
{
    return QSize(800, 600);
}

void MainWindow::connectTo()
{
    QString name = QInputDialog::getText(this, tr("User name"), tr("Name:"));

    ConnectionInfo conn;
    conn.host = "irc.xinutec.org";
    conn.name = "Xinutec";
    conn.port = 7776;
    conn.secure = true;
    conn.nick = name;
    conn.user = name;
    conn.real = name;
    conn.quit = "Leaving";
    connectTo (conn);
}

void MainWindow::connectTo(const ConnectionInfo& connection)
{
    Session* session = Session::fromConnection(connection, this);
    session->setEncoding(Application::encoding());
    session->setUserName(connection.user);
    if (session->ensureNetwork())
        session->open();

    SessionTabWidget* tab = new SessionTabWidget(session, this);
    if (connection.name.isEmpty())
        connect(tab, SIGNAL(titleChanged(QString)), tabWidget, SLOT(setSessionTitle(QString)));
    connect(tab, SIGNAL(inactiveStatusChanged(bool)), tabWidget, SLOT(setInactive(bool)));
    connect(tab, SIGNAL(alertStatusChanged(bool)), tabWidget, SLOT(setAlerted(bool)));
    connect(tab, SIGNAL(highlightStatusChanged(bool)), tabWidget, SLOT(setHighlighted(bool)));

    int index = tabWidget->addTab(tab, connection.name.isEmpty() ? session->host() : connection.name);
    tabWidget->setCurrentIndex(index);
    tabWidget->setTabInactive(index, !session->isActive());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());

    QMainWindow::closeEvent(event);
}

void MainWindow::changeEvent(QEvent* event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::ActivationChange)
    {
        if (isActiveWindow())
        {
            if (trayIcon)
                trayIcon->unalert();
            if (tabWidget)
            {
                SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidget->currentWidget());
                if (tab)
                    QMetaObject::invokeMethod(tab, "delayedTabReset");
            }
        }
    }
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::DoubleClick:
            setVisible(!isVisible());
            break;
        case QSystemTrayIcon::Trigger:
            raise();
            activateWindow();
            break;
        default:
            break;
    }
}

void MainWindow::activateAlert(bool activate)
{
    if (!isActiveWindow())
    {
        if (activate)
        {
            if (trayIcon)
                trayIcon->alert();
            QApplication::alert(this);
        }
        else
        {
            if (trayIcon)
                trayIcon->unalert();
        }
    }
}

void MainWindow::tabActivated(int index)
{
    if (index > 0 && index < tabWidget->count() - 1)
    {
        QTabWidget* tab = qobject_cast<QTabWidget*>(tabWidget->widget(index));
        if (tab)
        {
            setWindowFilePath(tab->tabText(tab->currentIndex()));
            QMetaObject::invokeMethod(tab, "delayedTabReset");
        }
    }
}

void MainWindow::onTabMenuRequested(int index, const QPoint& pos)
{
    if (index > 0 && index < tabWidget->count() - 1)
    {
        SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(tabWidget->widget(index));
        if (tab)
            QMetaObject::invokeMethod(tab, "onTabMenuRequested", Q_ARG(int, 0), Q_ARG(QPoint, pos));
    }
}
