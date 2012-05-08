# -------------------------------------------------
# Project created by QtCreator 2012-05-03T21:26:17
# -------------------------------------------------
QT += core \
    gui \
    network
TARGET = lina-irc
TEMPLATE = app
INCLUDEPATH += include \
    frontend/shared \
    frontend/util \
    frontend \
    frontend/3rdparty
SOURCES += src/ircutil.cpp \
    src/ircsession.cpp \
    src/ircsender.cpp \
    src/ircparser.cpp \
    src/ircmessage.cpp \
    src/ircencoder.cpp \
    src/irccommand.cpp \
    src/irc.cpp \
    frontend/settings.cpp \
    frontend/sessiontabwidget.cpp \
    frontend/searcheditor.cpp \
    frontend/messageview.cpp \
    frontend/mainwindow.cpp \
    frontend/maintabwidget.cpp \
    frontend/main.cpp \
    frontend/lineeditor.cpp \
    frontend/application.cpp \
    frontend/3rdparty/qtwin.cpp \
    frontend/3rdparty/fancylineedit.cpp \
    frontend/util/trayicon.cpp \
    frontend/util/textbrowser.cpp \
    frontend/util/tabwidget.cpp \
    frontend/util/sharedtimer.cpp \
    frontend/util/historylineedit.cpp \
    frontend/util/completer.cpp \
    frontend/shared/session.cpp \
    frontend/shared/messagehandler.cpp \
    frontend/shared/messageformatter.cpp \
    frontend/shared/commandparser.cpp \
    frontend/emoticons.cpp
HEADERS += include/irccommand.h \
    include/ircencoder_p.h \
    include/ircglobal.h \
    include/irc.h \
    include/ircmessage.h \
    include/ircparser_p.h \
    include/ircsender.h \
    include/ircsession.h \
    include/ircsession_p.h \
    include/ircutil.h \
    frontend/settings.h \
    frontend/sessiontabwidget.h \
    frontend/searcheditor.h \
    frontend/messageview.h \
    frontend/mainwindow.h \
    frontend/maintabwidget.h \
    frontend/lineeditor.h \
    frontend/application.h \
    frontend/3rdparty/qtwin.h \
    frontend/3rdparty/fancylineedit.h \
    frontend/util/trayicon.h \
    frontend/util/textbrowser.h \
    frontend/util/tabwidget.h \
    frontend/util/tabwidget_p.h \
    frontend/util/sharedtimer.h \
    frontend/util/historylineedit.h \
    frontend/util/completer.h \
    frontend/shared/streamer.h \
    frontend/shared/session.h \
    frontend/shared/messagehandler.h \
    frontend/shared/messageformatter.h \
    frontend/shared/connectioninfo.h \
    frontend/shared/commandparser.h \
    frontend/shared/channelinfo.h \
    frontend/emoticons.h
FORMS += frontend/messageview.ui \
    frontend/emoticons.ui
RESOURCES += lina-irc.qrc
