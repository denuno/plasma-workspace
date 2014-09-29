/*
 *   Copyright (C) 2014  Vishesh Handa <me@vhanda.in>
 *   Copyright (C) 2006  Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "kdedksysguard.h"

#include <QTimer>
#include <QAction>
#include <QDebug>
#include <QStandardPaths>
#include <QProcess>

#include <KPluginLoader>
#include <KPluginFactory>
#include <KLocalizedString>

#include <KGlobalAccel>
#include <KActionCollection>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

K_PLUGIN_FACTORY(KSysGuardFactory, registerPlugin<KDEDKSysGuard>();)

KDEDKSysGuard::KDEDKSysGuard(QObject* parent, const QVariantList&)
{
    QTimer::singleShot(0, this, SLOT(init()));
}

KDEDKSysGuard::~KDEDKSysGuard()
{
}

void KDEDKSysGuard::init()
{
    KActionCollection* actionCollection = new KActionCollection(this);

    QAction* action = actionCollection->addAction(QLatin1String("Show System Activity"));
    action->setText(i18n("Show System Activity"));
    connect(action, SIGNAL(triggered(bool)), SLOT(showTaskManager()));

    QKeySequence keysq(Qt::CTRL + Qt::Key_Escape);
    KGlobalAccel::self()->setShortcut(action, QList<QKeySequence>() << keysq);
}

void KDEDKSysGuard::showTaskManager()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QDBusConnectionInterface* interface = con.interface();
    if (interface->isServiceRegistered("org.kde.systemmonitor")) {
        QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.systemmonitor"),
                                                          QStringLiteral("/"),
                                                          QStringLiteral("org.qtproject.Qt.QWidget"),
                                                          QStringLiteral("close"));

        con.asyncCall(msg);
    }
    else {
        QString exe = QStandardPaths::findExecutable("systemmonitor");
        QProcess::startDetached(exe);
    }
}

#include "kdedksysguard.moc"
