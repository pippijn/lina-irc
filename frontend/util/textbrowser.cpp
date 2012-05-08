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

#include "textbrowser.h"
#include <QScrollBar>
#include <QDebug>


QHash<QMovie *, QUrl> TextBrowser::urls;


TextBrowser::TextBrowser(QWidget* parent) : QTextBrowser(parent)
{
}

void TextBrowser::addAnimation(QUrl const &url, QString const &fileName)
{
    QMovie *movie = new QMovie(this);
    if (!(movie = urls.key(url, 0)))
    {
        movie = new QMovie(this);
        movie->setCacheMode (QMovie::CacheAll);
        movie->setFileName(fileName);
        urls.insert(movie, url);
        movie->start();
    }

    connect(movie, SIGNAL(frameChanged(int)), this, SLOT(animate()));
}

void TextBrowser::animate()
{
    if (isEnabled())
    {
        QMovie *movie = qobject_cast<QMovie *>(sender());
        Q_ASSERT(movie);

        document()->addResource(QTextDocument::ImageResource, urls.value(movie), movie->currentPixmap());
        setLineWrapColumnOrWidth(lineWrapColumnOrWidth());
    }
}


void TextBrowser::resizeEvent(QResizeEvent* event)
{
    QTextBrowser::resizeEvent(event);

    // http://www.qtsoftware.com/developer/task-tracker/index_html?method=entry&id=240940
    QMetaObject::invokeMethod(this, "scrollToBottom", Qt::QueuedConnection);
}

void TextBrowser::scrollToTop()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderToMinimum);
}

void TextBrowser::scrollToBottom()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);
}

void TextBrowser::scrollToNextPage()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);
}

void TextBrowser::scrollToPreviousPage()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);
}
