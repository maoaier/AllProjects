#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QDir>
#include <QThread>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readJosn();
    m_pAddredit=new CIpAddrEdit(ui->centralWidget);
    ui->progressBar->setValue(0);
    this->setAutoFillBackground(true);
    m_iDownloadDir=0;
    m_pFtp=NULL;
    m_sIpAddres.clear();
    m_bConnectionStandardBit=false;
}

MainWindow::~MainWindow()
{
    delete ui;
}


//下载文件按钮转换的槽
void MainWindow::on_pushButton_clicked()
{
    bool bo=on_pushButton_3_clicked();
    if(bo==true)
    {
        copyFile();
    }
}


//选择目录
void MainWindow::on_pushButton_2_clicked()
{
    //QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes, QMessageBox::Yes);

   // QUrlInfo myFileInfo=fileInfo();
    QString DIr=QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("Save path"),QDir::currentPath()));
    if(!DIr.isEmpty())
    {
        if(ui->comboBox_catalog->findText(DIr)==-1)
            ui->comboBox_catalog->addItem(DIr);
        ui->comboBox_catalog->setCurrentIndex(ui->comboBox_catalog->findText(DIr));
    }
}

//输入账号和密码
void MainWindow::init(bool mode)
{
    if(mode == true)
    {
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_4->setEnabled(false);
    }
    if(mode == false)
    {
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
    }
}
//读取文件json
void MainWindow::readJosn()
{
    QFile loadFile("1.json");
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"could't open projects json";
       // QMessageBox::warning()
        //ui->lineEdit_2->setText("could't open projects json");
        QMessageBox::critical(NULL,"错误","could't open projects json!!!",QMessageBox::Yes,QMessageBox::Yes);
        return ;
    }
    QByteArray allData = loadFile.readAll();
    loadFile.close();
    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));
    if(json_error.error != QJsonParseError::NoError)
    {
       // qDebug() << "json error!";
        QMessageBox::critical(NULL,"错误","json error!!!",QMessageBox::Yes,QMessageBox::Yes);
        //ui->lineEdit_2->setText("");
        return;
    }
    QJsonObject rootObj = jsonDoc.object();
    //输出所有key，这一步是非必须的，放最后的话，你可能读不到任何key
    QStringList keys = rootObj.keys();
    for(int i = 0; i < keys.size(); i++)
    {
       qDebug() << "key" << i << " is:" << keys.at(i);
      // QJsonObject subObj = rootObj.value(keys.at(i)).toObject();
       m_mapJsonData.insert(keys.at(i),rootObj.value(keys.at(i)).toString());
       qDebug()<<"rootObj.value(keys.at(i)).toString()"<<rootObj.value(keys.at(i)).toString();
       QString fileName=keys.at(i);
       if(!fileName.isEmpty())
       {

           if(ui->comboBox_file->findText(fileName)==-1)
               ui->comboBox_file->addItem(fileName);
       }
    }

    return ;
}

//返回文件信息
QUrlInfo MainWindow::fileInfo()
{
    QString str=NULL;
    QMap<QString,QString>::const_iterator  mi=m_mapJsonData.find(ui->comboBox_file->currentText());
    if(mi!=m_mapJsonData.end())
    {
        str=mi.value();

    }
    for(int i=0;i<m_verMyFileInfe.size();i++)
    {
        if(m_verMyFileInfe.at(i).name()==str)
        return  m_verMyFileInfe.at(i);
    }
   // ui->lineEdit_2->setText("下载失败:不存在这设备的文件。");
    QMessageBox::information(NULL, "消息", "下载失败，不存在这设备的文件！！！", QMessageBox::Yes, QMessageBox::Yes);
    QUrlInfo info;
    info.setName(NULL);
    return info;

}

void MainWindow::copyFile()
{

    bool judge=correctJudgment();
    if(judge==false)
    {
        return ;
    }
    QUrlInfo myFileInfo=fileInfo();
    if(myFileInfo.name()==NULL)
    {
        return ;
    }
    qDebug()<<"myFileinfo.name"<<myFileInfo.name();
    init(true);
    m_iProgressBarValue=0;
    ui->progressBar->setValue(m_iProgressBarValue);
    m_sMyDownloadingPath=ui->comboBox_catalog->currentText();
    if(!myFileInfo.isDir())
    {
        ui->progressBar->setMaximum(1);
        qDebug()<<"进入下载按钮"<<endl;
        QString filePath;
        QString submitFile;
        submitFile=myFileInfo.name();
        qDebug()<<"submitFile="<<submitFile<<endl;
        filePath=ui->comboBox_catalog->currentText()+"\\"+submitFile;
        m_qFile.setFileName(filePath);
        if( !m_qFile.open(QIODevice::WriteOnly) )
        {
            qDebug() << "Error: cannot write file"
                 << qPrintable(m_qFile.fileName()) << ":"
                 << qPrintable(m_qFile.errorString()) ;
            return ;
        }
        m_sFtpFileNamePath=myFileInfo.name();
        m_mapFile.insert(m_sFtpFileNamePath,false);
        m_pFtp->get(submitFile,&m_qFile);

    }
    //下载目录
    else
    {
        m_iDownloadDir=1;
        m_sFtpFilePath=myFileInfo.name();
        m_mapDir.insert(m_sFtpFilePath,false);
        QString myDirName=m_sMyDownloadingPath+"\\"+m_sFtpFilePath;
        if(!m_dir.exists(myDirName))
        {
            m_dir.mkdir(myDirName);
        }
       m_pFtp->list(m_sFtpFilePath);
    }

}

//目录列表处理
void MainWindow::DirListProcess()
{
    QMap<QString,bool>::const_iterator  mi=m_mapDir.find(m_sFtpFilePath);
    if(mi!=m_mapDir.end())
    {
        m_mapDir.insert(m_sFtpFilePath,true);
    }

    QMap<QString,bool>::const_iterator i;
    for(i=m_mapDir.constBegin();i!=m_mapDir.constEnd();++i)
    {
       if(i.value()==false)
       {
           m_sFtpFilePath=i.key();
           m_pFtp->list(m_sFtpFilePath);
           return ;
       }
       qDebug()<<"m_mapDir.key()=="<<i.key()<<"m_mapDir.value()"<<i.value();;
   }

    ui->progressBar->setMaximum(m_mapFile.size());
    m_iProgressBarValue=0;
    ui->progressBar->setValue(m_iProgressBarValue);//.。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
    QMap<QString,bool>::const_iterator j;
    for(j=m_mapFile.constBegin();j!=m_mapFile.constEnd();++j)
    {
       if(j.value()==false)
       {
        m_sFtpFileNamePath=j.key();
        QString myfilepath=m_sMyDownloadingPath+"\\"+m_sFtpFileNamePath;
        m_qFile.setFileName(myfilepath);
        if( !m_qFile.open(QIODevice::WriteOnly) )
        {
          qDebug() << "Error: cannot write file"
               << qPrintable(m_qFile.fileName()) << ":"
               << qPrintable(m_qFile.errorString()) ;
          return ;
        }
        m_pFtp->get(m_sFtpFileNamePath,&m_qFile);
        return ;
       }
   }
}
//Qftp拷贝目录进程
void MainWindow::qftpCopyProcess()
{
    QMap<QString,bool>::const_iterator  mi=m_mapFile.find(m_sFtpFileNamePath);
    if(mi!=m_mapFile.end())
    {
        m_mapFile.insert(m_sFtpFileNamePath,true);
    }
    QMap<QString,bool>::const_iterator i;
    for(i=m_mapFile.constBegin();i!=m_mapFile.constEnd();++i)
    {
         if(i.value()==false)
         {
            m_sFtpFileNamePath=i.key();
            QString myfilepath=m_sMyDownloadingPath+"\\"+m_sFtpFileNamePath;
            m_qFile.setFileName(myfilepath);
            if( !m_qFile.open(QIODevice::WriteOnly) )
            {

                return ;
            }
            m_pFtp->get(m_sFtpFileNamePath,&m_qFile);
            return ;
         }
     }
     m_mapFile.clear();
     m_mapDir.clear();
     init(false);
     ui->progressBar->setValue(0);
     QMessageBox::information(NULL, "消息", "下载完成。", QMessageBox::Yes, QMessageBox::Yes);
}


void MainWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{

    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesReceived);
    init(false);
}


// 错误处理
void MainWindow::error(QNetworkReply::NetworkError error)
{
    switch (error) {
        case QNetworkReply::HostNotFoundError :
        break;
        // 其他错误处理
        default:
        break;
    }
}
//判断参数是否标准
bool MainWindow::correctJudgment()
{

    QString Ip;
    Ip=m_pAddredit->m_strGetText();
    if(Ip.isEmpty()!=0)
    {
        //ui->lineEdit_2->setText("错误：please enter IP number");
        QMessageBox::information(NULL, "消息", "请输入IP", QMessageBox::Yes, QMessageBox::Yes);
         return false;
    }
    QString submitFile;
    submitFile=ui->comboBox_file->currentText();
    if(submitFile.isEmpty()!=0)
     {
        //qDebug()<<"Please enter the required file."<<endl;
        //ui->lineEdit_2->setText("错误：Please enter the required file.");
        QMessageBox::information(NULL, "消息", "请选择设备", QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }
    QString filePath;
    filePath=ui->comboBox_catalog->currentText();
    if(filePath.isEmpty()!=0)
    {
        //ui->lineEdit_2->setText("错误:please enter download path");
        QMessageBox::information(NULL, "消息", "请选择下载目录", QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }
    if(m_bConnectionStandardBit==false)
    {

        QMessageBox::information(NULL, "消息", "连接失败或者没有连接", QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }
    return true;
}

//需要修改连接
bool MainWindow::on_pushButton_3_clicked()
{  
    if(m_sIpAddres==m_pAddredit->m_strGetText())
    {
        return true;
    }
    m_bConnectionStandardBit=false;
    if(!m_verMyFileInfe.isEmpty())
    {
        m_verMyFileInfe.clear();
    }
    if(m_pFtp)
    {

        m_pFtp->abort();
        m_pFtp->deleteLater();
        m_pFtp = 0;
    }
    m_pFtp =new QFtp(this);
    m_iDownloadDir=0;
    m_pFtp->connectToHost(m_pAddredit->m_strGetText(),21);
    m_sIpAddres=m_pAddredit->m_strGetText();
    m_pFtp->login("anonymous","3067804928@qq.com");
    m_pFtp->list();
   // stateChanged(m_pFtp->state());
    connect(m_pFtp, SIGNAL(commandFinished(int, bool)),this, SLOT(ftpCommandFinished(int,bool)));
    connect(m_pFtp, SIGNAL(done(bool)), this, SLOT(Ftpdone(bool)));
    connect(m_pFtp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(addToList(QUrlInfo)));
    connect(m_pFtp,SIGNAL(stateChanged(int)),this,SLOT(stateChanged(int)));
    return false;
}
//信息结束处理
void MainWindow::ftpCommandFinished(int n, bool bo)
{


    Q_UNUSED(n);
    QFtp::Command command = m_pFtp->currentCommand();
    switch (command) {
        case QFtp::List:  {  // 连接 FTP 服务器
            if (bo) { // 发生错误
                QMessageBox::information(this, "FTP", QStringLiteral("查看目录错误"));
                init(false);
            } else {
                qDebug()<<"DirListProcess()";
                if(m_iDownloadDir==0)
                {
                   copyFile();
                   return ;
                }
                    DirListProcess();
                }
                break;
             }
      case QFtp::Get:
        {
            if(bo)
             {
                 QMessageBox::information(this, "FTP", QStringLiteral("下载文件错误"));
            }
            else
            {
                ui->progressBar->setValue(++m_iProgressBarValue);//.。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
                m_qFile.close();
                qftpCopyProcess();

            }
        }
        default:
            break;
        }
}


void MainWindow::Ftpdone(bool errer)
{

    if(errer==false)
    {
         m_bConnectionStandardBit=true;
       // ui->lineEdit_2->setText("连接成功");
    }
    else
    {
         //ui->lineEdit_2->setText("连接失败");
    }
}

//返回对面目录******************************************************************************
void MainWindow::addToList(QUrlInfo urlInfo)
{
    if(m_iDownloadDir==0)
    {
        m_verMyFileInfe.append(urlInfo);
        qDebug()<<"urlInfo.name()"<<urlInfo.name();
    }
    else
    {
         if(!urlInfo.isDir())
         {
              QString Ftpfilename=m_sFtpFilePath+"\\"+urlInfo.name();
              m_mapFile.insert(Ftpfilename,false);

         }
         else
         {
             QString FtpDirname=m_sFtpFilePath+"\\"+urlInfo.name();
             m_mapDir.insert(FtpDirname,false);
             QString Myfilename=m_sMyDownloadingPath+"\\"+FtpDirname;
             if(!m_dir.exists(Myfilename))
             {
                 m_dir.mkdir(Myfilename);
             }

         }

    }

}



void MainWindow::stateChanged(int state)
{
    QString str;
    switch (state) {
        case QFtp::Unconnected: {

            str="没有连接到主机";
            break;
        }
        case QFtp::HostLookup: {

             //str="正在进行主机名查找";
            break;
        }
        case QFtp::Connecting: {
           // str="正在尝试连接到主机";
            break;
        }
        case QFtp::Connected: {
            qDebug()<<"已实现与主机的连接";
            m_bConnectionStandardBit=true;
            return ;
        }
        case QFtp::LoggedIn: {
           // ui->lineEdit_2->setText(QStringLiteral("已实现连接和用户登录"));
            m_bConnectionStandardBit=true;
            break;
        }
        case QFtp::Closing: {
            str="连接正在关闭";
            break;
        }
        default:
            break;
        }
    if(!str.isEmpty())
    {
        QMessageBox::information(NULL, "消息", str, QMessageBox::Yes, QMessageBox::Yes);
    }
}

void MainWindow::on_pushButton_4_clicked()
{


    ui->comboBox_catalog->clear();
    ui->progressBar->setValue(0);

}
