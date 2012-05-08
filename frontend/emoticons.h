#ifndef EMOTICONS_H
#define EMOTICONS_H

#include <QWidget>
#include <QHash>

namespace Ui {
    class Emoticons;
}

class Emoticons : public QWidget {
    Q_OBJECT
public:
    Emoticons(QWidget *parent = 0);
    ~Emoticons();

    static QHash<QString, QString> const emos;
    static QHash<QString, QString> const gifs;

protected:
    void changeEvent(QEvent *e);

private slots:
    void iconClicked();

signals:
    void emote(QString emotion);

private:
    Ui::Emoticons *ui;
};

#endif // EMOTICONS_H
