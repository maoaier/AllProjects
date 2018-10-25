#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QLabel;
class IViewBase;
class CExeclThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    friend class CExeclThread;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_open_triggered();
    void on_action_export_triggered();

    void on_actionDataWave_triggered();

    void on_actionInitInterface_triggered();

protected:
    void _initViewsData();

private:
    Ui::MainWindow *ui;
    QList<IViewBase*> m_funcViews;
    CExeclThread *m_pExeThread;
    QString m_statRootPath;
    QLabel *m_pStatTip;
};

#endif // MAINWINDOW_H
