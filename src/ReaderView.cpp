#include "ReaderView.h"
#include "TxtReader.h"

ReaderView::ReaderView(QWidget * parent)
    :QTextEdit(parent)
{
    p = (TxtReader *)parent;
    setWordWrapMode(QTextOption::WrapAnywhere);
    setAlignment(Qt::AlignTop|Qt::AlignLeft);
    setReadOnly(true);
    setTextInteractionFlags(Qt::NoTextInteraction);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    //setContextMenuPolicy(Qt::CustomContextMenu);
    QPalette pl=this->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    this->setPalette(pl);
    this->setAttribute(Qt::WA_TransparentForMouseEvents,true);//disable mouse event for ReaderView.Also to see grabMouse()
}

void ReaderView::keyPressEvent(QKeyEvent * event)
{
        if (event->key() == Qt::Key_Right || event->key() == Qt::Key_Down){
            p->jumpToNextPage();
        } else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Up){
            p->jumpToPrePage();
        } else {
            QTextEdit::keyPressEvent(event);
        }
        return ;
}
