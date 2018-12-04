#include "TxtReader.h"
#include <QTextCodec>
int main(int argc,char ** argv)
{
	QApplication app(argc,argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	TxtReader * txtReader = new TxtReader(0,argv[1]);
        txtReader->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);
	//txtReader->setWindowTitle(QObject::tr("TXT Reader --- By lsyer"));
        txtReader->setMinimumSize(270,196);
        txtReader->setMaximumSize(2430,1764);
        //txtReader->resize(804,468);
	txtReader->show();
	return app.exec();
}
