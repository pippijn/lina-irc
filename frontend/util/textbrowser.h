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

#ifndef TEXTBROWSER_H
#define TEXTBROWSER_H

#include <QTextBrowser>
#include <QHash>
#include <QMovie>

class TextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    TextBrowser(QWidget* parent = 0);

    void addAnimation(QUrl const &url, QString const &fileName);

public slots:
    void scrollToTop();
    void scrollToBottom();
    void scrollToNextPage();
    void scrollToPreviousPage();

private slots:
    void animate();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    static QHash<QMovie *, QUrl> urls;
};

#endif // TEXTBROWSER_H
