#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ctrls/qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void _initStatPlot();
    void _initWavePlot();
    void _analyseStaDat(QString fileName, int type);
    void _analyseWaveDat(QString fileName);
    void _on_mouseWheel(QCustomPlot *plot, QWheelEvent *event);

private slots:
    void on_action_fileopen_triggered();
    void on_action_max_triggered();
    void on_action_min_triggered();
    void on_action_avg_triggered();
    void on_action_cp95_triggered();
    void on_plot_stat_mouseWheel(QWheelEvent *event);
    void on_plot_wave_mouseWheel(QWheelEvent *event);
    void on_plot_bar_mouseWheel(QWheelEvent *event);
    void on_plot_bar_mouseMove(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
    QString m_fileName;
    QString m_waveFile;
};

#endif // MAINWINDOW_H
