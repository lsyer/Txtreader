#include "readerview.h"


readerView::readerView(QTextDocument *document, QWidget *parent)
    : QWidget(parent)
{
    this->setBackgroundRole(QPalette::Dark);

    doc = document;
    scale = 1;//zoomin or zoomout scale of the page;
    //interPageSpacing[0] = 15;
    //interPageSpacing[1] = 3;
    page=0;
}

void readerView::viewpage(int whichpage)
{
    page = whichpage-1;
    this->update();
}

void readerView::setDoc(QTextDocument *rdoc)
{
    doc = rdoc;
    if(doc->pageSize() != viewRect.size()){
        doc->setPageSize(viewRect.size());
    }
    this->update();
}

void readerView::resizeEvent(QResizeEvent *)
{
    viewRect=QRectF(QPointF(interPageSpacing[0], interPageSpacing[0]),
                QPointF(rect().width()-interPageSpacing[0],
                        rect().height()-interPageSpacing[1]));
    doc->setPageSize(viewRect.size());
    emit pagecountchanged();
}

void readerView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    

    //p.translate(interPageSpacing[0], interPageSpacing[0]);//move to giving point to start paint;

    //const int pages = doc->pageCount();
    //for (int i = 0; i < pages; ++i) {
        p.save();
        //p.scale(scale, scale);//zoomin or zoomout function for the page;

        paintPage(&p, page);

        p.restore();
       //p.translate(0, interPageSpacing[0] + viewRect.height() * scale);
    //}
}

void readerView::paintPage(QPainter *painter, int page)
{
    QColor col(Qt::black);

    painter->setPen(col);
    //painter->setBrush(Qt::white);
    //painter->drawRect(viewRect);
    //painter->setBrush(Qt::NoBrush);
    painter->drawLine(interPageSpacing[0],interPageSpacing[0],
                      rect().width()-interPageSpacing[0],interPageSpacing[0]);
    painter->drawLine(interPageSpacing[0],rect().height()-interPageSpacing[1],
                      rect().width()-interPageSpacing[0],rect().height()-interPageSpacing[1]);
    //col = col.light();//ÒõÓ°Ğ§¹û

    QRectF docRect(QPointF(0, page * viewRect.height()), viewRect.size());
    //QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.clip = docRect;

    // don't use the system palette text as default text color, on HP/UX
    // for example that's white, and white text on white paper doesn't
    // look that nice
    //ctx.palette.setColor(QPalette::Text, Qt::black);

    painter->translate(0, - page * viewRect.height());

    painter->translate(viewRect.topLeft());
    painter->setClipRect(docRect);//.translated(printPreview->pageTopLeft));
    doc->documentLayout()->draw(painter, ctx);
}
