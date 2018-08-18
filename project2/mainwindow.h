#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QMessageBox>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include "ipaddredit.h"
#include "ftpmanager.h"
#include  "qftp.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_4_clicked();
    bool on_pushButton_3_clicked();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError error);
    void ftpCommandFinished(int n, bool bo);
    void Ftpdone(bool errer);
    void addToList(QUrlInfo urlInfo);
    void stateChanged(int state);


private:
    Ui::MainWindow *ui;
    void DirListProcess();
    void qftpCopyProcess();
    bool correctJudgment();
    void init(bool mode);
    void readJosn();
    QUrlInfo fileInfo();
    void copyFile();

    CIpAddrEdit *m_pAddredit; 
    QFtp *m_pFtp;
    int m_iDownloadDir;
    int m_iProgressBarValue;
    bool m_bConnectionStandardBit;
    QString m_sFtpFilePath;//保存FTP目录路径
    QString m_sFtpFileNamePath;//保存FTP文件路径
    QString m_sMyDownloadingPath;
    QString m_sMyDownloadingPpath;
    QString m_sIpAddres;
    QDir m_dir;
    QFile m_qFile;
    QVector <QUrlInfo> m_verMyFileInfe;
    QMap <QString,bool> m_mapDir;
    QMap <QString,bool> m_mapFile;
    QMap <QString,QString> m_mapJsonData;
};

#endif // MAINWINDOW_H
