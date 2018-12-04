#ifndef __READERVIEW_H__
#define __READERVIEW_H__
#include <QWidget>
#include <QTextDocument>
#include <QPainter>
#include <QAbstractTextDocumentLayout>

class readerView : public QWidget
{
	Q_OBJECT
public:
    readerView(QTextDocument *document, QWidget *parent);
    void viewpage(int whichpage);
    QAbstractTextDocumentLayout::PaintContext ctx;
    int interPageSpacing[2];//0:R/T/L;1:B;
    void setDoc(QTextDocument *rdoc);
    QTextDocument * getDoc(){return doc;};

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *);

private:
    void paintPage(QPainter *painter, int page);
    QTextDocument *doc;
    qreal scale;
    int page;
    QRectF viewRect;
signals:
	void pagecountchanged();
};

#endif // __READERVIEW_H__
