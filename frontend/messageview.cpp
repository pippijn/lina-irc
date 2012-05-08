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

#include "messageview.h"
#include "session.h"
#include "settings.h"
#include "completer.h"
#include "application.h"
#include "emoticons.h"
#include <QStringListModel>
#include <QShortcut>
#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>
#include <QDebug>
#include <ircutil.h>
#include <irc.h>

QStringListModel* MessageView::MessageViewData::commandModel = 0;


MessageView::MessageViewData::MessageViewData (QWidget *parent)
    : formatter ((setupUi(parent), textBrowser))
{
}


MessageView::MessageView(IrcSession* session, bool serverView, QWidget* parent)
    : QWidget(parent)
    , d (this)
{
    d.emoticons->hide();
    if (!serverView)
    {
        connect(d.emoticons, SIGNAL(emote(QString)), this, SLOT(emote(QString)));
        connect(d.emoticonButton, SIGNAL(clicked()), this, SLOT(showEmoticons()));
    }
    else
    {
        d.textBrowser->setEnabled(false);
        d.buttons->setEnabled(false);
        d.lineEditor->setReadOnly(true);
    }

    setFocusProxy(d.lineEditor);
    d.textBrowser->installEventFilter(this);
    d.textBrowser->viewport()->installEventFilter(this);

    d.formatter.setHighlights(QStringList(session->nickName()));
    d.formatter.setMessageFormat("class='message'");
    d.formatter.setEventFormat("class='event'");
    d.formatter.setNoticeFormat("class='notice'");
    d.formatter.setActionFormat("class='action'");
    d.formatter.setUnknownFormat("class='unknown'");
    d.formatter.setHighlightFormat("class='highlight'");

    d.session = session;

    if (!serverView)
    {
        d.userModel = new QStringListModel(this);
        connect(&d.parser, SIGNAL(customCommand(QString,QStringList)), this, SLOT(onCustomCommand(QString,QStringList)));

        if (!d.commandModel)
        {
            CommandParser::addCustomCommand("QUERY", "<user>");

            QStringList prefixedCommands;
            foreach (const QString& command, CommandParser::availableCommands())
                prefixedCommands += "/" + command;

            d.commandModel = new QStringListModel(qApp);
            d.commandModel->setStringList(prefixedCommands);
        }

        d.lineEditor->completer()->setDefaultModel(d.userModel);
        d.lineEditor->completer()->setSlashModel(d.commandModel);

        connect(d.lineEditor, SIGNAL(send(QString)), this, SLOT(onSend(QString)));
        connect(d.lineEditor, SIGNAL(typed(QString)), this, SLOT(showHelp(QString)));

        d.searchEditor->setTextEdit(d.textBrowser);
    }

    d.helpLabel->hide();

    QShortcut* shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onEscPressed()));

    connect(qApp, SIGNAL(settingsChanged(Settings)), this, SLOT(applySettings(Settings)));
    applySettings(Application::settings());
}

MessageView::~MessageView()
{
}

QString MessageView::receiver() const
{
    return d.receiver;
}

void MessageView::setReceiver(const QString& receiver)
{
    d.receiver = receiver;
}

bool MessageView::isChannelView() const
{
    if (d.receiver.isEmpty())
        return false;

    switch (d.receiver.at(0).unicode())
    {
        case '#':
        case '&':
        case '!':
        case '+':
            return true;
        default:
            return false;
    }
}

void MessageView::showHelp(const QString& text, bool error)
{
    QString syntax;
    if (text == "/")
    {
        QStringList commands = CommandParser::availableCommands();
        syntax = commands.join(" ");
    }
    else if (text.startsWith('/'))
    {
        QStringList words = text.mid(1).split(' ');
        QString command = words.value(0);
        QStringList suggestions = CommandParser::suggestedCommands(command, words.mid(1));
        if (suggestions.count() == 1)
            syntax = CommandParser::syntax(suggestions.first());
        else
            syntax = suggestions.join(" ");

        if (syntax.isEmpty() && error)
            syntax = tr("Unknown command '%1'").arg(command.toUpper());
    }

    d.helpLabel->setVisible(!syntax.isEmpty());
    QPalette pal;
    if (error)
        pal.setColor(QPalette::WindowText, Qt::red);
    d.helpLabel->setPalette(pal);
    d.helpLabel->setText(syntax);
}

void MessageView::appendMessage(const QString& message)
{
    if (!message.isEmpty())
    {
        // workaround the link activation merge char format bug
        QString copy = message;
        if (copy.endsWith("</a>"))
            copy += " ";

        d.textBrowser->append(copy);
    }
}

bool MessageView::eventFilter(QObject* receiver, QEvent* event)
{
    Q_UNUSED(receiver);
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        // for example:
        // - Ctrl+C goes to the browser
        // - Ctrl+V goes to the line edit
        // - Shift+7 ("/") goes to the line edit
        switch (keyEvent->key())
        {
            case Qt::Key_Shift:
            case Qt::Key_Control:
            case Qt::Key_Meta:
            case Qt::Key_Alt:
            case Qt::Key_AltGr:
                break;
            default:
                if (!keyEvent->matches(QKeySequence::Copy))
                {
                    QApplication::sendEvent(d.lineEditor, keyEvent);
                    d.lineEditor->setFocus();
                    return true;
                }
                break;
        }
    }
    return false;
}

void MessageView::onEscPressed()
{
    d.helpLabel->hide();
    d.searchEditor->hide();
    setFocus(Qt::OtherFocusReason);
}

void MessageView::onSend(const QString& text)
{
    IrcCommand* cmd = d.parser.parseCommand(d.receiver, text);
    if (cmd)
    {
        d.session->sendCommand(cmd);
        d.sentCommands.insert(cmd->type());

        if (cmd->type() == IrcCommand::Message || cmd->type() == IrcCommand::CtcpAction)
        {
            IrcMessage* msg = IrcMessage::fromCommand(d.session->nickName(), cmd);
            receiveMessage(msg);
            delete msg;
        }
    }
    else if (d.parser.hasError())
    {
        showHelp(text, true);
    }
}

void MessageView::applySettings(const Settings& settings)
{
    d.formatter.setTimeStamp(settings.timeStamp);
    d.textBrowser->setFont(settings.font);
    d.textBrowser->document()->setMaximumBlockCount(settings.maxBlockCount);

    QString backgroundColor = settings.colors.value(Settings::Background);
    d.textBrowser->setStyleSheet(QString("QTextBrowser { background-color: %1 }").arg(backgroundColor));

    d.textBrowser->document()->setDefaultStyleSheet(
        QString(
            ".highlight { color: %1 }"
            ".message   { color: %2 }"
            ".notice    { color: %3 }"
            ".action    { color: %4 }"
            ".event     { color: %5 }"
        ).arg(settings.colors.value((Settings::Highlight)))
         .arg(settings.colors.value((Settings::Message)))
         .arg(settings.colors.value((Settings::Notice)))
         .arg(settings.colors.value((Settings::Action)))
         .arg(settings.colors.value((Settings::Event))));
}

void MessageView::receiveMessage(IrcMessage* message)
{
    bool append = true;
    bool hilite = false;
    bool matches = false;

    switch (message->type())
    {
    case IrcMessage::Join:
        append = Application::settings().messages.value(Settings::Joins);
        hilite = Application::settings().highlights.value(Settings::Joins);
        break;
    case IrcMessage::Kick:
        append = Application::settings().messages.value(Settings::Kicks);
        hilite = Application::settings().highlights.value(Settings::Kicks);
        break;
    case IrcMessage::Mode:
        append = Application::settings().messages.value(Settings::Modes);
        hilite = Application::settings().highlights.value(Settings::Modes);
        break;
    case IrcMessage::Nick:
        append = Application::settings().messages.value(Settings::Nicks);
        hilite = Application::settings().highlights.value(Settings::Nicks);
        break;
    case IrcMessage::Notice:
        {
            IrcNoticeMessage *notice = static_cast<IrcNoticeMessage*>(message);
            matches = notice->message().contains(d.session->nickName());
            hilite = true;
            if (notice->sender().name() == "NickServ")
                QTimer::singleShot(1000, this, SLOT(startQuery()));
            break;
        }
    case IrcMessage::Part:
        append = Application::settings().messages.value(Settings::Parts);
        hilite = Application::settings().highlights.value(Settings::Parts);
        break;
    case IrcMessage::Private:
        matches = !isChannelView() || static_cast<IrcPrivateMessage*>(message)->message().contains(d.session->nickName());
        hilite = true;
        break;
    case IrcMessage::Quit:
        append = Application::settings().messages.value(Settings::Quits);
        hilite = Application::settings().highlights.value(Settings::Quits);
        break;
    case IrcMessage::Topic:
        append = Application::settings().messages.value(Settings::Topics);
        hilite = Application::settings().highlights.value(Settings::Topics);
        break;
    case IrcMessage::Unknown:
        qWarning() << "unknown:" << message;
        append = false;
        break;
    case IrcMessage::Invite:
    case IrcMessage::Ping:
    case IrcMessage::Pong:
    case IrcMessage::Error:
        break;
    case IrcMessage::Numeric: {
            IrcNumericMessage* numeric = static_cast<IrcNumericMessage*>(message);
            switch (numeric->code())
            {
            case Irc::RPL_ENDOFNAMES:
                if (d.sentCommands.contains(IrcCommand::Names))
                {
                    QString names = prettyNames(d.formatter.currentNames(), 6);
                    appendMessage(d.formatter.formatMessage(message));
                    appendMessage(names);
                    d.sentCommands.remove(IrcCommand::Names);
                    return;
                }
                break;
            case Irc::ERR_NICKNAMEINUSE:
                d.session->setNickName(d.session->nickName() + "_");
                break;
            }
            break;
        }
    }

    if (matches)
        emit alert(this, true);
    else if (hilite)
        emit highlight(this, true);
    if (append)
        appendMessage(d.formatter.formatMessage(message));
}

void MessageView::addUser(const QString& user)
{
    // TODO: this is far from optimal
    QStringList users = d.userModel->stringList();
    users.append(user);
    d.userModel->setStringList(users);
}

void MessageView::removeUser(const QString& user)
{
    // TODO: this is far from optimal
    QStringList users = d.userModel->stringList();
    if (users.removeOne(user))
        d.userModel->setStringList(users);
}

void MessageView::onCustomCommand(const QString& command, const QStringList& params)
{
    if (command == "QUERY")
        emit query(params.value(0));
}

QString MessageView::prettyNames(const QStringList& names, int columns)
{
    QString message;
    message += "<table>";
    for (int i = 0; i < names.count(); i += columns)
    {
        message += "<tr>";
        for (int j = 0; j < columns; ++j)
            message += "<td>" + MessageFormatter::colorize(names.value(i+j)) + "&nbsp;</td>";
        message += "</tr>";
    }
    message += "</table>";
    return message;
}

void MessageView::startQuery()
{
    emit query ("pippijn");
}

void MessageView::showEmoticons()
{
    if (d.emoticons->isVisible())
        d.emoticons->hide();
    else
        d.emoticons->show();
}

void MessageView::emote(QString emotion)
{
    d.emoticons->hide();
    d.lineEditor->insert(emotion);
    d.lineEditor->setFocus();
}
