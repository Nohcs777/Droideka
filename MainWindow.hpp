#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "WidgetTestOpenGL.hpp"
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    
signals:
    
public slots:

protected:
    virtual void keyPressEvent(QKeyEvent* inEvent);

private slots:

private:
    void setupActions();
    void setupMenu();

    WidgetTestOpenGL* mGLW;
    QMenu* mHelpMenu;
    QAction* mAboutAct;
    QAction* mAboutQtAct;
};

#endif
