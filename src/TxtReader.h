#ifndef TXTREADER_H
#define TXTREADER_H
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QList>
#include <QFontMetrics>
#include <QLine>
#include <QDebug>
#include <QTime>
#include <QPushButton>
#include <QTextEdit>
#include <QSpacerItem>
#include <QGridLayout>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QInputDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QStyleOption>
#include <QTranslator>
#include <QUrl>
#include <QPainter>
#include "ReaderView.h"
class TxtReader : public QWidget
{
	Q_OBJECT
public:
	TxtReader(QWidget * parent = 0,QString infile = "");
        void jumpToPrePage();
        void jumpToNextPage();
protected:
	void keyPressEvent(QKeyEvent * event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
        void closeEvent ( QCloseEvent * event );
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
//for context menu
	void contextMenuEvent(QContextMenuEvent *event);
        void paintEvent(QPaintEvent *);
        virtual void resizeEvent(QResizeEvent *event);
private:
	QString version,bgImage,uilang;
	QFont txtFont;
        QColor txtColor,bgColor;
        int curPageNum,curOffset;
	bool o_isfullscreen;

        QLabel viewTitleLabel;
        ReaderView *viewContentEdit;
        QLabel viewInstructionLabel;
        QLabel viewPageLabel;
        //QSpacerItem *leftSpacer,*rightSpacer;
        QFrame upLine,bottomLine;
	QGridLayout *layout;

        QString FileContent;
        QList<int> PageOffsetList;
        int TotalPageNum;
        QString searchText;

	QPoint dragPosition;
        void jumpToOffset(int offset);
        void jumpToPage(int pageNum);
        int loadFile(const QString &fileName);
        QString readInstructionContent();
        void genPageList(QString &content,QList<int> &list);
        void reGenPageList();

	QIcon icon;
	QAction *openAct;
	QAction *aboutAct;
	QAction *instructionAct;
	QString codecstr;
        QMenu *searchMenu;
        QAction *searchOriAct;
        QAction *searchCurAct;
	QMenu *codecMenu;
	QAction *GB2312Act;
	QAction *UTF8Act;
	QActionGroup *codecActionGroup;
	QAction *jumpAct;
	QMenu *bookmarkMenu;
        QList<QAction *> bookmarkArray;
	QAction *addBookmarkAct;
	QAction *delBookmarkAct;
        QAction *clearBookmarkAct;
	QMenu *setfontMenu;
	QAction *addTxtSizeAct;
	QAction *subTxtSizeAct;
	QAction *setfontAct;
	QAction *setfontcolorAct;
	QMenu * bgMenu;
	QAction * setbgcolorAct;
	QAction * setbgimageAct;
	QAction * delbgimageAct;
	QMenu * languageMenu;
	QActionGroup *languageActionGroup;
	void createLanguageMenu();
	QTranslator appTranslator;
//for trayIcon
	void createActions();
	void createTrayIcon();
	//QAction *maximizeAction;
	QAction *viewfullscreenAction;
	QAction *nofullscreenAction;
	QAction *hiddenAction;
	QAction *restoreAction;
	QAction *quitAction;

	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;

// for recentfiles menu
	void setCurrentFile(const QString &fileName);
	void updateRecentFileActions();
	QString strippedName(const QString &fullFileName);
	QString curFile;
	QMenu *recentFilesMenu;
	enum { MaxRecentFiles = 5 };
	QAction *recentFileActs[MaxRecentFiles];
        QAction *separatorAct;

        void setSizeBaseAndIncIncrement();
        void showInstruction();
        void backToRead();
private slots:
	void open();
        void about();
        void slotSearchOri();
        void slotSearchCur();
        void readOrInstruct();
        void showChanges();
        void myShowContextMenu(QPoint point);
	void setcodecstr(QAction *action);
        void slotJumpToPage();
        void getBookDependSetting();
        void saveBookDependSetting();
	void addBookmark();
	void delBookmark();
        void clearBookmark();
	void openBookmark();
	void addTxtSize();
	void subTxtSize();
	void setfont();
	void setfontcolor();
	void setbgcolor();
	void setbgimage();
        void resetbg();
	void hideAndRestore();
	void myhide();
	void myrestore();
	void myshowfullscreen();
	void myexitfullscreen();
	void switchLanguage(QAction *action);
	void retranslateUi();
//for trayIcon
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void quitaction();
//for recentfiles
        void openRecentFile();
};
#endif 
