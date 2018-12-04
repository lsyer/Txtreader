#ifndef READERVIEW_H
#define READERVIEW_H

#include <QTextEdit>
#include <QKeyEvent>
class TxtReader;
class ReaderView : public QTextEdit
{
    Q_OBJECT
public:
    ReaderView(QWidget * parent = 0);
protected:
    void keyPressEvent(QKeyEvent * event);
private:
    TxtReader *p;
};

#endif // READERVIEW_H
