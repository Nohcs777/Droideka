#include "MainWindow.hpp"
#include <QtGui>
#include <QKeyEvent>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    mCanvas = new CanvasOpenGL(this);
    loadCards();

    setCentralWidget(mCanvas);

    setWindowTitle(QString("Droideka"));
    resize(QSize(800, 600));

    setupMenu();

    mIsFullScreen = false;
    setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::keyPressEvent(QKeyEvent* inEvent)
{
    switch (inEvent->key())
    {
    case Qt::Key_F11:
        toggleFullscreen();
        break;

    case Qt::Key_Escape:
        close();
        break;

    default:
        mCanvas->onKeyPress(inEvent);
        break;
    }
}

void MainWindow::loadCards()
{
    QFile file(QString("cards.txt"));

    if (file.open(QIODevice::ReadOnly))
    {
        qDebug() << "opened file";
        QTextStream stream(&file);

        QString front;
        QString back;
        int count;

        stream >> front >> back >> count;

        while (!stream.atEnd())
        {
            qDebug() << "Loading" << front << "and" << back;

            for (int i = 0; i < count; ++i)
                mCanvas->addCard(front, back);

            stream >> front >> back >> count;
        }

        file.close();
    }
    else
    {
        qDebug() << "failed to open file";
    }
}

void MainWindow::setupMenu()
{
    setupActions();

    mHelpMenu = menuBar()->addMenu("Help");
    mHelpMenu->addAction(mAboutQtAct);
    mHelpMenu->addAction(mAboutAct);

    statusBar();
}

void MainWindow::setupActions()
{
    mAboutAct = new QAction("About &DEJARIX", this);
    mAboutAct->setStatusTip("Information about DEJARIX");
    connect(mAboutAct, SIGNAL(triggered()), this, SLOT(showAboutDialog()));

    mAboutQtAct = new QAction("About &Qt", this);
    mAboutQtAct->setStatusTip("Information about Qt");
    connect(mAboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::showAboutDialog()
{
    QMessageBox::about(this, QString("About DEJARIX"),
        QString("DEJARIX 0.1 -- Star Wars CCG Online"));
}

void MainWindow::toggleFullscreen()
{
    mIsFullScreen = !mIsFullScreen;

    if (mIsFullScreen)
        showFullScreen();
    else
        showNormal();
}
