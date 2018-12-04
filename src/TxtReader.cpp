#include "TxtReader.h"
TxtReader::TxtReader(QWidget * parent,QString infile)
	:QWidget(parent)
{
	//w1 = 0;
        version = QString("0.6.6");
        setWindowTitle( QString("TXTReader %1 --- By lsyer").arg(version));
        qApp->installTranslator(&appTranslator);
        qApp->setQuitOnLastWindowClosed(false);
        setAcceptDrops(true);

        QSettings settings("lsyer", "txtreader");
	QPoint pos = settings.value("pos", QPoint(100, 60)).toPoint();
        QSize size = settings.value("size", QSize(840, 618)).toSize();
	curFile = settings.value("curFile", QString("")).toString();
        curOffset = -1;
        txtFont.fromString(settings.value("txtFont",QString("仿宋_GB2312,14,-1,5,50,0,0,0,0,0")).toString());
        txtColor=QColor(settings.value("txtColor", QColor("#000000")).toString());
	o_isfullscreen = settings.value("o_isfullscreen", 0).toBool();
	bgColor=QColor(settings.value("bgColor", QColor("#ffffff")).toString());
	bgImage = settings.value("bgImage", QString("")).toString();
	codecstr = settings.value("codecstr", QString("GB2312")).toString();
        uilang = settings.value("uilang", QString("zh_CN")).toString();

	createActions();
	createLanguageMenu();
	createTrayIcon();
	setStyleSheet( "TxtReader {background-color: "+bgColor.name()+";background-image: url("+bgImage+");background-position:center;;}");

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	icon = QIcon(":/images/icon.png");
	trayIcon->setIcon(icon);
	this->setWindowIcon(icon);
        trayIcon->setToolTip(QString("TxtReader %1").arg(version));
        trayIcon->show();

        upLine.setFrameShape(QFrame::HLine);
        upLine.setFrameShadow(QFrame::Sunken);
        bottomLine.setFrameShape(QFrame::HLine);
        bottomLine.setFrameShadow(QFrame::Sunken);
        viewTitleLabel.setFixedHeight(15);
        viewTitleLabel.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        QSpacerItem *leftSpacer=new QSpacerItem(15,1,QSizePolicy::Fixed,QSizePolicy::Fixed);
        viewContentEdit = new ReaderView(this);
        viewContentEdit->setStyleSheet("color:"+txtColor.name()+";");
        viewContentEdit->setFont(txtFont);
        viewContentEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        //connect(viewContentEdit,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(myShowContextMenu(QPoint)));
        viewInstructionLabel.setFont(txtFont);
        viewInstructionLabel.setWordWrap(true);
        viewInstructionLabel.setAlignment(Qt::AlignTop|Qt::AlignLeft);
        viewInstructionLabel.setVisible(false);
        QSpacerItem *rightSpacer=new QSpacerItem(15,1,QSizePolicy::Fixed,QSizePolicy::Fixed);
        viewPageLabel.setFixedHeight(15);
        viewPageLabel.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        viewPageLabel.setStyleSheet("color:"+txtColor.name()+";");
        viewPageLabel.setAlignment(Qt::AlignTop|Qt::AlignRight);
        //viewPageLabel.setText(QString(tr("第 %1 页/共 %2 页")).arg(curPageNum).arg(doc.pageCount()));

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(&viewTitleLabel,0,1);
        layout->addWidget(&upLine,1,1);
        layout->addItem(leftSpacer,2,0);
        layout->addWidget(viewContentEdit,2,1);
        layout->addItem(rightSpacer,2,2);
        layout->addWidget(&viewInstructionLabel,3,1);
        layout->addWidget(&bottomLine,4,1);
        layout->addWidget(&viewPageLabel,5,1);
        this->setLayout(layout);
        setSizeBaseAndIncIncrement();

        appTranslator.load(":/i18n/txtreader_" + uilang);
	retranslateUi();
	//qDebug()<<"infile2:"<< infile <<"\n";
        if(infile.isEmpty()){
            loadFile(curFile);
        }else{
            loadFile(infile);
        }

        if(o_isfullscreen){
                viewfullscreenAction->setEnabled(false);
                nofullscreenAction->setEnabled(true);
                showFullScreen();
        }else{
                viewfullscreenAction->setEnabled(true);
                nofullscreenAction->setEnabled(false);
                resize(size);
                move(pos);
        }
        restoreAction->setEnabled(false);

}

void TxtReader::resizeEvent(QResizeEvent *event)
{
    //qDebug()<<":resizeEvent\n";
    //if(event->spontaneous()){ //if resizeEvent is created inside the app
        reGenPageList();
        QWidget::resizeEvent(event);
        //qDebug()<<"this->width():"<<this->width()<<" this->height()"<<this->height()<<"\n";
        //qDebug()<<"viewContentEdit->width():"<<viewContentEdit->width()<<" viewContentEdit->height()"<<viewContentEdit->height()<<"\n";
        //qDebug()<<"base.width():"<<this->width()-viewContentEdit->width()<<" base.height()"<<this->height()-viewContentEdit->height()<<"\n";
    //}
}

void TxtReader::setSizeBaseAndIncIncrement()
{
    QFontMetrics fm(txtFont);
    this->setBaseSize(fm.maxWidth()+60,qMax(fm.lineSpacing(),fm.height())+94);//52+8,86+8,and 52,86 is the basesize of other widgets.And 8 will be used in genPageList(QString &content,QList<int> &list)
    this->setSizeIncrement(fm.maxWidth(),qMax(fm.lineSpacing(),fm.height()));
}

void TxtReader::jumpToOffset(int offset)
{
    //qDebug()<<offset<<":offset\n";
    if(offset<0 || offset>FileContent.size()){
        jumpToPage(1);
        return;
    }

    int i;
    for(i=0;i<TotalPageNum;i++){
        if(PageOffsetList[i] > offset)
            break;
    }
    //qDebug()<<i<<":i\n";
    jumpToPage(i);
    return;
}

void TxtReader::jumpToPage(int pageNum)
{
        if(pageNum < 1)
                curPageNum = 1;
        else if(pageNum > TotalPageNum)
                curPageNum = TotalPageNum;
	else
                curPageNum = pageNum;

        viewContentEdit->setText(FileContent.mid(PageOffsetList[curPageNum-1],PageOffsetList[curPageNum]-PageOffsetList[curPageNum-1]));
        viewPageLabel.setText(QString(tr("Page %1 /Total %2")).arg(curPageNum).arg(TotalPageNum));

        return;
}
void TxtReader::jumpToPrePage(){
    jumpToPage(curPageNum-1);
    curOffset = PageOffsetList[curPageNum-1];
    return;
}
void TxtReader::jumpToNextPage(){
    jumpToPage(curPageNum+1);
    curOffset = PageOffsetList[curPageNum-1];
    return;
}

void TxtReader::genPageList(QString &content,QList<int> &list){
    /**/
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTime t;
    t.restart();
    //qDebug()<<"StartTime:"<<QTime().currentTime().toString()<<"\n";

    list.clear();
    QFontMetrics fm=QFontMetrics(txtFont);
    int PageLineNum = (viewContentEdit->height()-8) / qMax(fm.lineSpacing(),fm.height());
    int PageLineNow=0;
    int PageWidth=viewContentEdit->width()-8;
    int LineWidthNow=0;
    int AsciiWidth=fm.width("i"),ChineseWidth=qMax(fm.maxWidth(),fm.width("中"));
    int newPageOffset=0;
    list.append(0);
    int FileContentSize=content.size();
    for(int offset=0;offset<FileContentSize;offset++){
        int b = FileContent[offset].toAscii();

        if(b ==0 ) {// Chinese
            LineWidthNow += ChineseWidth;
            if(LineWidthNow > PageWidth){
                LineWidthNow = ChineseWidth;
                PageLineNow++;
                newPageOffset = offset;
            }
        }else if (b == 10){// \n
            LineWidthNow=0;
            PageLineNow++;
            newPageOffset = offset+1;
        }else if (b != 13){// not \r
            // Ascii
            LineWidthNow += AsciiWidth;
            if(LineWidthNow > PageWidth){
                LineWidthNow = AsciiWidth;
                PageLineNow++;
                newPageOffset = offset;
            }
        }

        /**************************************
         ******** optimized by upper code
        if (b == 10){// \n
            LineWidthNow=0;
            PageLineNow++;
            newPageOffset = offset+1;
        }else if (b != 13){// \r
            if(b == 0) {// Chinese
                LineWidthNow += ChineseWidth;
            }else{// Ascii
                LineWidthNow += AsciiWidth;
            }
            if(LineWidthNow > PageWidth){
                LineWidthNow = fm.width(FileContent[offset]);
                PageLineNow++;
                newPageOffset = offset;
            }
        }
        */

        if(PageLineNow==PageLineNum){
            PageLineNow=0;
            list.append(newPageOffset);
        }
    }
    if(list.last() != FileContentSize || FileContentSize == 0)
        list.append(FileContentSize);

    TotalPageNum=PageOffsetList.size() - 1;
    //qDebug()<<"txtFont.pointSize():"<<txtFont.pointSize()<<"\n";
    //qDebug()<<"PageWidth:"<<viewContentEdit->width()<<"  fm.width(i):"<<fm.width("i")<<"  fm.width(ij):"<<fm.width("ij")<<"  fm.width():"<<fm.width(FileContent[20])<<"\n";
    //qDebug()<<"PageHeight:"<<viewContentEdit->height()<<"  LineHeight:"<<fm.height()<<"  lineSpacing:"<<fm.lineSpacing()<<"  leading:"<<fm.leading()<<"  PageLineNum:"<<PageLineNum<<"\n";
    //qDebug()<<"FileContent Size:"<<FileContentSize<<"\n";
    //qDebug()<<"PageCount:"<<PageOffsetList.size()<<"\n";

    //qDebug()<<"CompleteTime:"<<QTime().currentTime().toString()<<"\n";
    qDebug()<<"ElapsedTime:"<<t.elapsed()<<"\n";
    QApplication::restoreOverrideCursor();
    /**/

    return;
}

void TxtReader::reGenPageList()
{
    genPageList(FileContent,PageOffsetList);
    jumpToOffset(curOffset);
}

int TxtReader::loadFile(const QString &fileName)
{
    if (fileName.isEmpty()) {
        showInstruction();
        return 0;
    }
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this,tr("Read File Failed"),
                             tr("Failed to read file %1.\nPerhaps it is not there, or you don't have access permission.").arg(fileName));

        readOrInstruct();
        QSettings settings("lsyer", "txtreader");
        QStringList files = settings.value("recentFileList").toStringList();
        files.removeAll(fileName);
        settings.setValue("recentFileList", files);

        updateRecentFileActions();
        return 0;
    }

	QTextStream in(&file);
	//printf("codec:%s\n",(const char *)codecstr.toLocal8Bit());
    in.setCodec((const char *)codecstr.toLocal8Bit());
    FileContent=in.readAll();
    //genPageList(FileContent,PageOffsetList); //reSizeEvent will be done while app initial,so would not do this,ro it will genPageList more than one time.

    saveBookDependSetting();
    setCurrentFile(fileName);
    getBookDependSetting();
    //statusBar()->showMessage(tr("File loaded"), 2000);
    //jumpToOffset(curOffset); //reSizeEvent will do auomaticly.

    return 1;
}

QString TxtReader::readInstructionContent(){
    QFile file(QString(":/README_%1.HTML").arg(uilang));
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
            return QString(tr("Cannot read file,please contact to author!"));
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    return in.readAll();
}

void TxtReader::readOrInstruct(){
    if(viewContentEdit->isVisible()){
        showInstruction();
    }else{
    	backToRead();
    }
}
void TxtReader::showInstruction()
{
    if(viewInstructionLabel.text().isEmpty())
        viewInstructionLabel.setText(readInstructionContent());

    viewContentEdit->setVisible(false);
    viewPageLabel.setText("");
    viewInstructionLabel.setVisible(true);
    instructionAct->setText(tr("&Go Back"));
}
void TxtReader::backToRead()
{
    viewContentEdit->setVisible(true);
    viewPageLabel.setText(QString(tr("Page %1 /Total %2")).arg(curPageNum).arg(TotalPageNum));
    viewInstructionLabel.setVisible(false);
    instructionAct->setText(tr("&Instruction"));
}

void TxtReader::showChanges(){
    QFile file(QString(":/CHANGES.TXT"));
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, QString("TxtReader %1").arg(version),
                            tr("Cannot read file,please contact to author!"));
            return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QDialog changesDialog;
    changesDialog.setWindowIcon(this->windowIcon());
    changesDialog.setWindowTitle(tr("TxtReader Changes"));
    QGridLayout l;
    QTextEdit t;
    t.setReadOnly(true);
    t.setText(in.readAll());
    QPushButton b(QIcon(":/images/close.png"),tr("&Close"),this);
    connect(&b,SIGNAL(clicked()),&changesDialog,SLOT(close()));
    l.addWidget(&t);
    l.addWidget(&b,1,0,Qt::AlignRight);
    changesDialog.setLayout(&l);
    changesDialog.resize(400,300);
    changesDialog.exec();
}

void TxtReader::myShowContextMenu(QPoint point)
{
    trayIconMenu->exec(point);
}

void TxtReader::createActions()
{
    openAct = new QAction(QIcon(":/images/open.png"),"", this);
    //openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    jumpAct = new QAction(QIcon(":/images/jump.png"),"", this);
    //openAct->setShortcut(tr("Ctrl+O"));
    connect(jumpAct, SIGNAL(triggered()), this, SLOT(slotJumpToPage()));

    addBookmarkAct = new QAction(QIcon(":/images/bookmark-new.png"),"", this);
    //openAct->setShortcut(tr("Ctrl+O"));
    connect(addBookmarkAct, SIGNAL(triggered()), this, SLOT(addBookmark()));

    delBookmarkAct = new QAction(QIcon(":/images/bookmark-del.png"),"", this);
    //openAct->setShortcut(tr("Ctrl+O"));
    connect(delBookmarkAct, SIGNAL(triggered()), this, SLOT(delBookmark()));

    clearBookmarkAct = new QAction(QIcon(":/images/bookmark-clear.png"),"", this);
    //openAct->setShortcut(tr("Ctrl+O"));
    connect(clearBookmarkAct, SIGNAL(triggered()), this, SLOT(clearBookmark()));

    addTxtSizeAct = new QAction(QIcon(":/images/zoom-in.png"),"", this);
    //openAct->setShortcut(tr("Ctrl+O"));
    connect(addTxtSizeAct, SIGNAL(triggered()), this, SLOT(addTxtSize()));

    subTxtSizeAct = new QAction(QIcon(":/images/zoom-out.png"),"", this);
    //openAct->setShortcut(tr("Ctrl+O"));
    connect(subTxtSizeAct, SIGNAL(triggered()), this, SLOT(subTxtSize()));

    setfontAct = new QAction(QIcon(":/images/setfont.png"),"", this);
    connect(setfontAct, SIGNAL(triggered()), this, SLOT(setfont()));

    setfontcolorAct = new QAction(QIcon(":/images/bgcolor.png"),"", this);
    connect(setfontcolorAct, SIGNAL(triggered()), this, SLOT(setfontcolor()));

    setbgcolorAct = new QAction(QIcon(":/images/bgcolor.png"),"", this);
    connect(setbgcolorAct, SIGNAL(triggered()), this, SLOT(setbgcolor()));

    setbgimageAct = new QAction(QIcon(":/images/bgimage.png"),"", this);
    connect(setbgimageAct, SIGNAL(triggered()), this, SLOT(setbgimage()));
    
    delbgimageAct = new QAction(QIcon(":/images/reset.png"),"", this);
    connect(delbgimageAct, SIGNAL(triggered()), this, SLOT(resetbg()));

    codecActionGroup = new QActionGroup(this);
    connect(codecActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(setcodecstr(QAction *)));
    GB2312Act = new QAction(tr("&GB2312"), this);
    GB2312Act->setData("GB2312");
    GB2312Act->setCheckable(true);
    codecActionGroup->addAction(GB2312Act);
    if (codecstr == "GB2312") GB2312Act->setChecked(true);

    UTF8Act = new QAction(tr("&UTF-8"), this);
    UTF8Act->setData("UTF-8");
    UTF8Act->setCheckable(true);
    codecActionGroup->addAction(UTF8Act);
    if (codecstr == "UTF-8") UTF8Act->setChecked(true);

    searchOriAct = new QAction(QIcon(":/images/searchOri.png"),"", this);
    connect(searchOriAct, SIGNAL(triggered()), this, SLOT(slotSearchOri()));

    searchCurAct = new QAction(QIcon(":/images/searchCur.png"),"", this);
    connect(searchCurAct, SIGNAL(triggered()), this, SLOT(slotSearchCur()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    //maximizeAction = new QAction(tr("Ma&ximize"), this);
    //connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));
    
    viewfullscreenAction = new QAction(QIcon(":/images/fullscreen.png"),"", this);
    connect(viewfullscreenAction, SIGNAL(triggered()), this, SLOT(myshowfullscreen()));
    
    nofullscreenAction = new QAction(QIcon(":/images/nofullscreen.png"),"", this);
    connect(nofullscreenAction, SIGNAL(triggered()), this, SLOT(myexitfullscreen()));
    
    hiddenAction = new QAction(QIcon(":/images/min.png"),"", this);
    connect(hiddenAction, SIGNAL(triggered()), this, SLOT(myhide()));

    restoreAction = new QAction(QIcon(":/images/restore.png"),"", this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(myrestore()));

    quitAction = new QAction(QIcon(":/images/exit.png"),"", this);
    //quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quitaction()));

    aboutAct = new QAction(QIcon(":/images/about.png"),"", this);
    //aboutAct->setShortcut(tr("Ctrl+A"));
    //aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    instructionAct = new QAction(QIcon(":/images/instruction.png"),"", this);
    //instructionAct->setShortcut(tr("Ctrl+A"));
    //instructionAct->setStatusTip(tr("Show the application's About box"));
    connect(instructionAct, SIGNAL(triggered()), this, SLOT(readOrInstruct()));
	return ;
}

void TxtReader::createLanguageMenu()
{
	languageMenu = new QMenu(this);
	languageMenu->setIcon(QIcon(":/images/lang.png"));

    languageActionGroup = new QActionGroup(this);
    connect(languageActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(switchLanguage(QAction *)));

    QDir qmDir(":/i18n/");
    QStringList fileNames =
            qmDir.entryList(QStringList("txtreader_*.qm")); 

    for (int i = 0; i < fileNames.size(); ++i) {
    	QString locale = fileNames[i];
        locale.remove(0, locale.indexOf('_') + 1);
        locale.chop(3);
        
        QAction *action = new QAction(tr("&%1 %2")
                                      .arg(i + 1).arg(locale), this);
        action->setCheckable(true);
        action->setData(locale);

        languageMenu->addAction(action);
        languageActionGroup->addAction(action);

        if (locale == uilang)
            action->setChecked(true);
    }
}

void TxtReader::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(openAct);
    trayIconMenu->addAction(jumpAct);

    bookmarkMenu = trayIconMenu->addMenu(QIcon(":/images/bookmark.png"),"");
    bookmarkMenu->addAction(addBookmarkAct);
    bookmarkMenu->addAction(delBookmarkAct);
    bookmarkMenu->addAction(clearBookmarkAct);
    bookmarkMenu->addSeparator();

    setfontMenu = trayIconMenu->addMenu(QIcon(":/images/font.png"),"");
    setfontMenu->addAction(addTxtSizeAct);
    setfontMenu->addAction(subTxtSizeAct);
    setfontMenu->addAction(setfontAct);
    setfontMenu->addAction(setfontcolorAct);

    bgMenu = trayIconMenu->addMenu(QIcon(":/images/bg.png"),"");
    bgMenu->addAction(setbgcolorAct);
    bgMenu->addAction(setbgimageAct);
    bgMenu->addAction(delbgimageAct);

    codecMenu = trayIconMenu->addMenu(QIcon(":/images/codec.png"),"");
    codecMenu->addAction(GB2312Act);
    codecMenu->addAction(UTF8Act);

    searchMenu = trayIconMenu->addMenu(QIcon(":/images/search.png"),"");
    searchMenu->addAction(searchOriAct);
    searchMenu->addAction(searchCurAct);

    separatorAct = trayIconMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
            trayIconMenu->addAction(recentFileActs[i]);

    trayIconMenu->addSeparator();
    //trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(viewfullscreenAction);
    trayIconMenu->addAction(nofullscreenAction);
    trayIconMenu->addAction(hiddenAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addMenu(languageMenu);
    trayIconMenu->addAction(instructionAct);
    trayIconMenu->addAction(aboutAct);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    updateRecentFileActions();

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
	return ;
}

void TxtReader::setCurrentFile(const QString &fileName)
{
    curFile = fileName;

    setWindowTitle(QString("%1 - TxtReader %2").arg(strippedName(curFile))
                                    .arg(version));

    QSettings settings("lsyer", "txtreader");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    updateRecentFileActions();
	return ;
}

void TxtReader::updateRecentFileActions()
{
	QSettings settings("lsyer", "txtreader");
	QStringList files = settings.value("recentFileList").toStringList();

	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setVisible(true);
        }
	for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
		recentFileActs[j]->setVisible(false);

        separatorAct->setVisible(numRecentFiles > 0);
        return ;
}

QString TxtReader::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

void TxtReader::open()
{
    backToRead();
    if(viewContentEdit->toPlainText()=="")
        viewContentEdit->setText("Wating...");

    QString fileName = QFileDialog::getOpenFileName(this,tr("Open new file"),QFileInfo(curFile).dir().path(),tr("text files (*.txt)"));
    if(loadFile(fileName)){
        reGenPageList();
     }else{
        showInstruction();
    }
}
void TxtReader::about()
{
    QDialog aboutDialog;
    aboutDialog.setWindowIcon(this->windowIcon());
    aboutDialog.setWindowTitle(tr("About"));
    QGridLayout l;
    QLabel p;
    p.setPixmap(QPixmap(":/images/icon.png"));
    QLabel t;
    t.setText(tr("<p align=center>Txt Reader %1 </p><p align=right> Design by <a href='HOMEPAGE' target=_blank>lsyer</a></p><p align=center>CopyLeft(C)2006-YEAR</p>").arg(version));
    QPushButton c(tr("&Changes"));
    connect(&c,SIGNAL(clicked()),this,SLOT(showChanges()));
    QPushButton b(QIcon(":/images/close.png"),tr("&Back"),this);
    connect(&b,SIGNAL(clicked()),&aboutDialog,SLOT(close()));
    b.setFocus();
    l.addWidget(&p,0,0,Qt::AlignCenter);
    l.addWidget(&t,0,1,1,2,Qt::AlignCenter);
    l.addWidget(&c,1,0,1,2,Qt::AlignCenter);
    l.addWidget(&b,1,2,Qt::AlignCenter);
    aboutDialog.setLayout(&l);
    aboutDialog.setFixedSize(aboutDialog.sizeHint());
    //aboutDialog.setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    aboutDialog.exec();

    return ;
}
void TxtReader::setcodecstr(QAction *action)
{
    //QAction *action = qobject_cast<QAction *>(sender());
    //if (action){
    codecstr = action->data().toString();
    if (codecstr == "UTF-8"){
            UTF8Act->setChecked(true);
            GB2312Act->setChecked(false);
    } else {
            UTF8Act->setChecked(false);
            GB2312Act->setChecked(true);
    }
    if(loadFile(curFile))
        reGenPageList();
    //}
    return ;
}
void TxtReader::slotJumpToPage()
{
     bool ok;
     int i = QInputDialog::getInteger(this, QString("Txt Reader %1").arg(version),
                                          tr("Jump to:"), curPageNum,1,TotalPageNum,1, &ok);
     if (ok){
         jumpToPage(i);
         curOffset = PageOffsetList[curPageNum-1];
     }

    return ;
}
void TxtReader::slotSearchOri()
{
    bool ok;
    searchText = QInputDialog::getText(this, QString("Txt Reader %1").arg(version),
                                         tr("Search:"), QLineEdit::Normal,searchText, &ok);
    if ( ok && !(searchText.isNull()) ){
        int pos=FileContent.indexOf(searchText);
        if(pos != -1){
            jumpToOffset(pos);
            curOffset = pos;
        }
    }

   return ;
}
void TxtReader::slotSearchCur()
{
    if ( !(searchText.isNull()) ){
        int pos=FileContent.indexOf(searchText,curOffset + 1);
        if(pos != -1){
            jumpToOffset(pos);
            curOffset = pos;
        }
    }

   return ;
}
void TxtReader::addBookmark()
{
	bool ok;
        QString text = QInputDialog::getText(this, QString("Txt Reader %1").arg(version),
                                          tr("Please input the mark:"), QLineEdit::Normal,
                                          tr("%1").arg(curPageNum), &ok);
	if (ok && !text.isEmpty()){
                QAction * bookmark = new QAction(tr("%1").arg(text), this);
                bookmark->setData(PageOffsetList[curPageNum-1]);
		connect(bookmark, SIGNAL(triggered()), this, SLOT(openBookmark()));
                bookmarkArray.append(bookmark);
		bookmarkMenu->addAction(bookmark);
	}
	return ;
}
void TxtReader::delBookmark()
{
    for(int i=0;i<bookmarkArray.size();i++){
        //qDebug()<<"i:"<<i<<" PageOffsetList[curPageNum-1]:"<<PageOffsetList[curPageNum-1]<<" PageOffsetList[curPageNum]:"<<PageOffsetList[curPageNum]<<"\n";
        if(bookmarkArray[i]->data().toInt()>=PageOffsetList[curPageNum-1] && bookmarkArray[i]->data().toInt()<=PageOffsetList[curPageNum]){
            delete bookmarkArray[i];
            bookmarkArray.removeAt(i);
            break;
        }
    }
    return ;
}
void TxtReader::clearBookmark()
{
    qDeleteAll(bookmarkArray);
    bookmarkArray.clear();
    return ;
}
void TxtReader::openBookmark()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action){
                jumpToOffset(action->data().toInt());
	}
	return ;
}

void TxtReader::getBookDependSetting(){
    QSettings settings("lsyer", tr("txtreader_%1").arg(strippedName(curFile)));
    curOffset = settings.value("curOffset", 0).toInt();
    int j = settings.value("bookmarknum", 0).toInt()+1;
    for(int k=1;k < j;++k){
            QAction * bookmark = new QAction(settings.value(QString("bookmark%1words").arg(k)/*, QString(tr("未知标签"))*/).toString(), this);
            bookmark->setData(settings.value(QString("bookmark%1offset").arg(k)/*, 1*/).toInt());
            connect(bookmark, SIGNAL(triggered()), this, SLOT(openBookmark()));
            bookmarkArray.append(bookmark);
            bookmarkMenu->addAction(bookmark);
    }
}

void TxtReader::saveBookDependSetting(){
    if(curOffset != -1){
        QSettings settings("lsyer", tr("txtreader_%1").arg(strippedName(curFile)));
        settings.setValue("curOffset", curOffset);
        int j=bookmarkArray.size();
        settings.setValue("bookmarknum",j);
        int k=0;
        for(int i=0;i<j;i++){
            ++k;
            settings.setValue(QString("bookmark%1offset").arg(k),bookmarkArray[i]->data().toString());
            settings.setValue(QString("bookmark%1words").arg(k),bookmarkArray[i]->text());
        }
        qDeleteAll(bookmarkArray);
        bookmarkArray.clear();
    }
    return;
}

void TxtReader::addTxtSize()
{
	if(txtFont.pointSize() < 25){
		txtFont.setPointSize(txtFont.pointSize()+1);
                this->setSizeIncrement(txtFont.pointSize(),txtFont.pointSize());
                QApplication::setOverrideCursor(Qt::WaitCursor);
                setSizeBaseAndIncIncrement();
                reGenPageList();
                viewContentEdit->setFont(txtFont);
                QApplication::restoreOverrideCursor();
	}
	return;
}
void TxtReader::subTxtSize()
{
	if(txtFont.pointSize() > 5){
		txtFont.setPointSize(txtFont.pointSize()-1);
                this->setSizeIncrement(txtFont.pointSize(),txtFont.pointSize());
                QApplication::setOverrideCursor(Qt::WaitCursor);
                setSizeBaseAndIncIncrement();
                reGenPageList();
                viewContentEdit->setFont(txtFont);
                QApplication::restoreOverrideCursor();
	}
	return;
}
void TxtReader::setfont()
{
	bool ok;
        QFont font = QFontDialog::getFont(&ok, txtFont, this,QString(tr("Set Font - Txt Reader %1")).arg(version));
	if (ok && font!=txtFont) {
		txtFont = font;
                this->setSizeIncrement(txtFont.pointSize(),txtFont.pointSize());
                QApplication::setOverrideCursor(Qt::WaitCursor);
                setSizeBaseAndIncIncrement();
                reGenPageList();
                viewContentEdit->setFont(txtFont);
                QApplication::restoreOverrideCursor();
	}
	return;
}
void TxtReader::setfontcolor()
{
	QColor Color = QColorDialog::getColor(txtColor, this);
	if (Color.isValid()) {
                txtColor=Color;
                viewContentEdit->setStyleSheet("color:"+txtColor.name()+";");
                viewPageLabel.setStyleSheet("color:"+txtColor.name()+";");
	}
	return ;
}
void TxtReader::setbgcolor()
{
	QColor Color = QColorDialog::getColor(bgColor, this);
	if (Color.isValid()) {
		bgColor=Color;
		setStyleSheet( "TxtReader {background-color: "+bgColor.name()+";background-image: url("+bgImage+");background-position:center;;}");
	}
	return ;
}
void TxtReader::setbgimage()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Select bg-image"),QFileInfo(bgImage).dir().path(),tr("picture files (*.png *.jpg *.jpeg *.gif)"));
    if (!fileName.isEmpty()) {
    	bgImage=fileName;
		setStyleSheet( "TxtReader {background-color: "+bgColor.name()+";background-image: url("+bgImage+");background-position:center;;}");  	
    }
	return ;
}
void TxtReader::resetbg()
{
    bgImage = "";
    bgColor=QColor(Qt::white);
	setStyleSheet( "TxtReader {background-color: "+bgColor.name()+";background-image: url("+bgImage+");background-position:center;}");  	
}
void TxtReader::hideAndRestore()
{
   	if(isHidden()){
		myrestore();
  	}else{
   		myhide();
  	}
}
void TxtReader::myhide()
{
	if(isHidden())
		return;

	o_isfullscreen = isFullScreen();
	hiddenAction->setEnabled(false);
	restoreAction->setEnabled(true);
   	hide();
}
void TxtReader::myrestore()
{
        if(o_isfullscreen){
                showFullScreen();
        }else{
                show();
        }

	hiddenAction->setEnabled(true);
	restoreAction->setEnabled(false);
}
void TxtReader::myshowfullscreen()
{
	o_isfullscreen = true;
	viewfullscreenAction->setEnabled(false);
	nofullscreenAction->setEnabled(true);

        //showFullScreen();
	myrestore();
}
void TxtReader::myexitfullscreen()
{
	o_isfullscreen = false;
	viewfullscreenAction->setEnabled(true);
	nofullscreenAction->setEnabled(false);
	
        showNormal();
        myrestore();
}

void TxtReader::switchLanguage(QAction *action)
{
    QString locale = action->data().toString();
	uilang = locale;
    appTranslator.load(":/i18n/txtreader_" + locale);
    retranslateUi();
}

void TxtReader::retranslateUi()
{
    viewPageLabel.setText(QString(tr("Page %1 /Total %2")).arg(curPageNum).arg(TotalPageNum));
    //newAction->setText(tr("&New"));//
    openAct->setText(tr("&Open"));// = new QAction(QIcon(":/images/open.png"),tr("(&O)打  开"), this);
    jumpAct->setText(tr("&Jump"));// = new QAction(QIcon(":/images/jump.png"),tr("(&J)跳  转"), this);
    addBookmarkAct->setText(tr("&Add Bookmark"));// = new QAction(QIcon(":/images/addbookmark.png"),tr("(&A)添加书签"), this);
    delBookmarkAct->setText(tr("&Del Current"));
    clearBookmarkAct->setText(tr("&Clear All"));
    searchOriAct->setText(tr("&Find"));
    searchCurAct->setText(tr("&Next"));
    addTxtSizeAct->setText(tr("&Zoom In"));// = new QAction(QIcon(":/images/zoom-in.png"),tr("(&F)放  大"), this);
    subTxtSizeAct->setText(tr("&Zoom Out"));// = new QAction(QIcon(":/images/zoom-out.png"),tr("(&S)缩  小"), this);
    setfontAct->setText(tr("&Set Font"));// = new QAction(QIcon(":/images/setfont.png"),tr("(&A)设  置"), this);
    setfontcolorAct->setText(tr("&Color"));// = new QAction(QIcon(":/images/bgcolor.png"),tr("(&C)颜  色"), this);
    setbgcolorAct->setText(tr("&Bg color"));// = new QAction(QIcon(":/images/bgcolor.png"),tr("(&C)颜  色"), this);
    setbgimageAct->setText(tr("&Bg image"));// = new QAction(QIcon(":/images/bgimage.png"),tr("(&P)图  片"), this);    
    delbgimageAct->setText(tr("&Reset"));// = new QAction(QIcon(":/images/reset.png"),tr("(&D)重  设"), this);
    viewfullscreenAction->setText(tr("&Full Screen"));// = new QAction(QIcon(":/images/fullscreen.png"),tr("(&F)全  屏"), this);    
    nofullscreenAction->setText(tr("&Exit Fullscreen"));
    hiddenAction->setText(tr("&Hide"));// = new QAction(QIcon(":/images/min.png"),tr("(&H)隐  藏"), this);
    restoreAction->setText(tr("&Restore"));// = new QAction(QIcon(":/images/restore.png"),tr("(&R)恢  复"), this);
    quitAction->setText(tr("&Quit"));// = new QAction(QIcon(":/images/exit.png"),tr("(&Q)退  出"), this);
    aboutAct->setText(tr("&About"));// = new QAction(QIcon(":/images/about.png"),tr("(&A)关  于"), this);
    instructionAct->setText(tr("&Instruction"));// = new QAction(QIcon(":/images/instruction.png"),tr("(&I)程序说明"), this);
    
    languageMenu->setTitle(tr("&Language"));
    bookmarkMenu->setTitle(tr("&Bookmark"));// = trayIconMenu->addMenu(QIcon(":/images/bookmark.png"),tr("(&B)书  签"));
    setfontMenu->setTitle(tr("&Font"));// = trayIconMenu->addMenu(QIcon(":/images/font.png"),tr("(&F)字  体"));
    bgMenu->setTitle(tr("&Background"));// = trayIconMenu->addMenu(QIcon(":/images/bg.png"),tr("(&G)背  景"));
    codecMenu->setTitle(tr("&Codec"));// = trayIconMenu->addMenu(QIcon(":/images/codec.png"),tr("(&C)编  码"));
    searchMenu->setTitle(tr("&Search"));
}

void TxtReader::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
   // case QSystemTrayIcon::DoubleClick:
   		hideAndRestore();
        break;
    //case QSystemTrayIcon::MiddleClick:
        //showMessage();
        //break;
    default:
        break;
    }
	return ;
}

void TxtReader::quitaction()
{
        QSettings settings("lsyer", "txtreader");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
        settings.setValue("curFile", curFile);
	settings.setValue("o_isfullscreen", o_isfullscreen);
	settings.setValue("txtFont", txtFont);
	settings.setValue("txtColor", txtColor);
	settings.setValue("bgColor", bgColor);
	settings.setValue("bgImage", bgImage);
	settings.setValue("codecstr", codecstr);
        settings.setValue("uilang", uilang);

        saveBookDependSetting();
	qApp->quit();
	return ;
}

void TxtReader::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action){
            if(action->data().toString()!=curFile){
                    if(loadFile(action->data().toString())){
                        reGenPageList();
			    		backToRead();
					}
                }
	}
	return ;
}

void TxtReader::keyPressEvent(QKeyEvent * event)
{
        if (event->key() == Qt::Key_Right || event->key() == Qt::Key_Down){
            jumpToNextPage();
        } else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Up){
            jumpToPrePage();
	} else if (event->key() == Qt::Key_Escape) {
            myhide();
        } else {
            QWidget::keyPressEvent(event);
	}
	return ;
}
void TxtReader::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
	if(this->x()+3*this->width()/4< event->globalX() && event->globalX() < this->x()+this->width()){
                jumpToNextPage();
	}else if(this->x()< event->globalX() && event->globalX() < this->x()+this->width()/4){
                jumpToPrePage();
        }else{
		dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
	}
    }else{
        QWidget::mousePressEvent(event);
    }
    return ;
}
void TxtReader::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		move(event->globalPos() - dragPosition);
		event->accept();
        }else{
            QWidget::mouseMoveEvent(event);
        }
	return ;
}
void TxtReader::contextMenuEvent(QContextMenuEvent *event)
{
    myShowContextMenu(event->globalPos());
    return ;
}

void TxtReader::closeEvent ( QCloseEvent * event )
{
	hideAndRestore();
	event->ignore();
}

void TxtReader::dragEnterEvent(QDragEnterEvent *event)
{
	event->acceptProposedAction();
}

void TxtReader::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;
	QString fileName = urls.first().toLocalFile();
	if (fileName.isEmpty())
		return;
        if(loadFile(fileName))
            reGenPageList();
}

void TxtReader::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

