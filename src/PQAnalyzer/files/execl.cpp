#include "execl.h"
#include <windows.h>
using namespace std;
CExecl::~CExecl()
{

}

CExecl::CExecl()
{

}

//初始化数据
int CExecl::initialization()
{
    HRESULT r = OleInitialize(0);
    if (r != S_OK && r != S_FALSE)
    {
        qWarning("Qt:初始化Ole失败（error %x）",(unsigned int)r);
        return -1;
    }
    m_cExcel=new QAxObject("Excel.Application");
    m_cExcel->setProperty("Visiblie", false);
    m_cWorkBooks = m_cExcel->querySubObject("WorkBooks");
    m_cWorkBooks->dynamicCall("Add");
    m_cWorkBook = m_cExcel->querySubObject("ActiveWorkBook");
    m_cWorkSheets=m_cWorkBook->querySubObject("Sheets");
    m_cWorkSheets->property("Count").toInt();
    m_cWorkSheet = m_cWorkSheets->querySubObject("Item(int)",1);
    m_cWorkSheet->setProperty("Name",SHEET1);
    addSheet(m_cWorkSheet3,SHEET3);
    addSheet(m_cWorkSheet2,SHEET2);

    //设置标题
    sheetControl();
    return 0;
}

//字符串转换为char型
void CExecl::strToCharText(char ch[], QString &str)
{
    memset(ch,0,108);
    char tmp[108]={0};
    QByteArray byte=str.toUtf8();
    strncpy(tmp,byte.data(),100);
    strcpy(ch,"'");
    strcat(ch,tmp);
}

void CExecl::strToChar(char ch[], QString &str)
{
    memset(ch,0,108);
    char tmp[108]={0};
    QByteArray byte=str.toUtf8();
    strncpy(tmp,byte.data(),100);
    strcat(ch,tmp);
}

//增加一列
void CExecl::listAdd(QList<QVariant> &rowData,const char *data)
{
    QList<QVariant> aline;
    aline.append(QVariant(data));
    QVariant conv(aline);
    rowData.append(conv);
}

//数据写入控制第一章表格
void CExecl::writeDataControl_1(CVoltageQuality &testData)
{
    char data[108];
    int line=4;
    int column=0;
    QList<QVariant> rowData1;
    QList<QVariant> rowData2;

    strToChar(data,testData.workingCondition.monitoingPsoition);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet,line,3,testData.workingCondition.monitoingPsoition.toUtf8().data());
    strToChar(data,testData.workingCondition.monitoingTime);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet,line,10,testData.workingCondition.monitoingTime.toUtf8().data());

    line=5;
    strToChar(data,testData.workingCondition.monitoingLine);
    //initData(m_cWorkSheet,line,3,data);
    listAdd(rowData1,data);
    strToChar(data,testData.workingCondition.fieldOperator);
    //initData(m_cWorkSheet,line,10,data);
    listAdd(rowData2,data);

    line=6;
    strToChar(data,testData.workingCondition.voltageLevel);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet,line,3,data);
    strToCharText(data,testData.workingCondition.dataPT);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet,line,10,data);
    strToCharText(data,testData.workingCondition.dataCT);
    initData(m_cWorkSheet,line,16,data);

    line=7;
    strToChar(data,testData.workingCondition.referenceShortCircuitCpy);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet,line,3,data);
    strToChar(data,testData.workingCondition.minimumShortCircuitCpy);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet,line,10,data);

    line=8;
    strToChar(data,testData.workingCondition.userAgreementCpy);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet,line,3,data);
    strToChar(data,testData.workingCondition.powerSuppluEquipmentCpy);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet,line,10,data);

    line=9;
    strToChar(data,testData.workingCondition.workCondition);
    //initData(m_cWorkSheet,line,3,data);
    listAdd(rowData1,data);
    //qDebug("LINE:%d writeDataControl_1 start",__LINE__);

    QVariant totalDataFront(rowData1);
    QString positionFront="C"+QString::number(4)+":"+"C"+QString::number(9);
    QAxObject *rangeFront=m_cWorkSheet->querySubObject("Range(QString)",positionFront);
    rangeFront->setProperty("Value",totalDataFront);

    QVariant totalDataFront2(rowData2);
    QString positionFront2="J"+QString::number(4)+":"+"J"+QString::number(8);
    QAxObject *rangeFront2=m_cWorkSheet->querySubObject("Range(QString)",positionFront2);
    rangeFront2->setProperty("Value",totalDataFront2);

    column=3;
    line=13;
    QList<QVariant> rowData;
    
    for(int i=0;i<26;i++)
    {
        QList<QVariant> aline;
        for(int j=0;j<3;j++)
        {
            for(int k=0;k<5;k++)
            {
                if(4==k)
                {
                    strToChar(data,testData.voltageData[i][j][k]);
                    if(strcmp(data,"不合格")==0)
                    {
                        initDataRed(m_cWorkSheet,line+i,column+(j*5)+k);

                    }
//                    else
//                    {
//                         initData(m_cWorkSheet,line+i,column+(j*5)+k,data);
//                    }
                }
                else
                {
                     strToCharText(data,testData.voltageData[i][j][k]);
                     //initData(m_cWorkSheet,line+i,column+(j*5)+k,data);
                }
                aline.append(QVariant(data));
            }
        }
        strToCharText(data,testData.voltageDataStandard[i]);
        aline.append(QVariant(data));
        QVariant conv(aline);
        rowData.append(conv);
        //initData(m_cWorkSheet,line+i,column+(3*5),data);
    }
    QVariant totalData(rowData);
    QString position="C"+QString::number(13)+":"+"R"+QString::number(38);
    QAxObject *range=m_cWorkSheet->querySubObject("Range(QString)",position);
    range->setProperty("Value",totalData);
    return ;
}

//数据写入控制第二表格
void CExecl::writeDataControl_2(CCurrentQuality &currentData)
{
    char data[108];
    int line=4;
    int column=0;
    QList<QVariant> rowData1;
    QList<QVariant> rowData2;

    strToChar(data,currentData.workingCondition.monitoingPsoition);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet2,line,3,data);
    strToChar(data,currentData.workingCondition.monitoingTime);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet2,line,10,data);

    line=5;
    strToChar(data,currentData.workingCondition.monitoingLine);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet2,line,3,data);
    strToChar(data,currentData.workingCondition.fieldOperator);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet2,line,10,data);

    line=6;
    strToChar(data,currentData.workingCondition.voltageLevel);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet2,line,3,data);
    strToCharText(data,currentData.workingCondition.dataPT);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet2,line,10,data);
    strToCharText(data,currentData.workingCondition.dataCT);
    initData(m_cWorkSheet2,line,16,data);

    line=7;
    strToChar(data,currentData.workingCondition.referenceShortCircuitCpy);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet2,line,3,data);
    strToChar(data,currentData.workingCondition.minimumShortCircuitCpy);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet2,line,10,data);

    line=8;
    strToChar(data,currentData.workingCondition.userAgreementCpy);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet2,line,3,data);
    strToChar(data,currentData.workingCondition.powerSuppluEquipmentCpy);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet2,line,10,data);

    line=9;
    strToChar(data,currentData.workingCondition.workCondition);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet2,line,3,data);

    QVariant totalDataFront(rowData1);
    QString positionFront="C"+QString::number(4)+":"+"C"+QString::number(9);
    QAxObject *rangeFront=m_cWorkSheet2->querySubObject("Range(QString)",positionFront);
    rangeFront->setProperty("Value",totalDataFront);

    QVariant totalDataFront2(rowData2);
    QString positionFront2="J"+QString::number(4)+":"+"J"+QString::number(8);
    QAxObject *rangeFront2=m_cWorkSheet2->querySubObject("Range(QString)",positionFront2);
    rangeFront2->setProperty("Value",totalDataFront2);


    column=3;
    line=13;
    QList<QVariant> rowData;
    //qDebug("LINE:%d writeDataControl_2 start",__LINE__);
    for(int i=0;i<25;i++)
    {
        QList<QVariant> aline;
        for(int j=0;j<3;j++)
        {
            for(int k=0;k<5;k++)
            {
                if(4==k)
                {
                    //qDebug()<<"合格="<<testData.voltageData[i][j][k]<<"\n";
                    strToChar(data,currentData.currentData[i][j][k]);
                    if(strcmp(data,"不合格")==0)
                    {
                        initDataRed(m_cWorkSheet2,line+i,column+(j*5)+k);
                    }
//                    else
//                    {
//                         initData(m_cWorkSheet2,line+i,column+(j*5)+k,data);
//                    }
                }
                else
                {
                     strToCharText(data,currentData.currentData[i][j][k]);
                     //initData(m_cWorkSheet2,line+i,column+(j*5)+k,data);
                }
                aline.append(QVariant(data));
            }
        }
        strToCharText(data,currentData.currentDataStandard[i]);
        aline.append(QVariant(data));
        QVariant conv(aline);
        rowData.append(conv);
        //initData(m_cWorkSheet2,line+i,column+(3*5),data);
    }
    QVariant totalData(rowData);
    QString position="C"+QString::number(13)+":"+"R"+QString::number(37);
    QAxObject *range=m_cWorkSheet2->querySubObject("Range(QString)",position);
    //qDebug("LINE:%d writeDataControl_2 end",__LINE__);
    range->setProperty("Value",totalData);
    return ;
}

//数据写入控制第三表格
void CExecl::writeDataControl_3(CPowerQuality &powerData)
{
    char data[108];
    int line=4;
    int column=0;
    QList<QVariant> rowData1;
    QList<QVariant> rowData2;
    strToChar(data,powerData.workingCondition.monitoingPsoition);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet3,line,3,data);
    strToChar(data,powerData.workingCondition.monitoingTime);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet3,line,9,data);

    line=5;
    strToChar(data,powerData.workingCondition.monitoingLine);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet3,line,3,data);
    strToChar(data,powerData.workingCondition.fieldOperator);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet3,line,9,data);

    line=6;
    strToChar(data,powerData.workingCondition.voltageLevel);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet3,line,3,data);
    strToCharText(data,powerData.workingCondition.dataPT);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet3,line,9,data);
    strToCharText(data,powerData.workingCondition.dataCT);
    initData(m_cWorkSheet3,line,13,data);

    line=7;
    strToChar(data,powerData.workingCondition.referenceShortCircuitCpy);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet3,line,3,data);
    strToChar(data,powerData.workingCondition.minimumShortCircuitCpy);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet3,line,9,data);

    line=8;
    strToChar(data,powerData.workingCondition.userAgreementCpy);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet3,line,3,data);
    strToChar(data,powerData.workingCondition.powerSuppluEquipmentCpy);
    listAdd(rowData2,data);
    //initData(m_cWorkSheet3,line,9,data);

    line=9;
    strToChar(data,powerData.workingCondition.workCondition);
    listAdd(rowData1,data);
    //initData(m_cWorkSheet3,line,3,data);

    QVariant totalDataFront(rowData1);
    QString positionFront="C"+QString::number(4)+":"+"C"+QString::number(9);
    QAxObject *rangeFront=m_cWorkSheet3->querySubObject("Range(QString)",positionFront);
    rangeFront->setProperty("Value",totalDataFront);

    QVariant totalDataFront2(rowData2);
    QString positionFront2="I"+QString::number(4)+":"+"I"+QString::number(8);
    QAxObject *rangeFront2=m_cWorkSheet3->querySubObject("Range(QString)",positionFront2);
    rangeFront2->setProperty("Value",totalDataFront2);
    column=3;
    line=12;
    QList<QVariant> rowData;
    for(int i=0;i<27;i++)
    {
        QList<QVariant> aline;
        for(int j=0;j<6;j++)
        {
            if(5==j)
            {
                //qDebug()<<"合格="<<testData.voltageData[i][j][k]<<"\n";
                strToChar(data,powerData.powerData[i][j]);
                if(strcmp(data,"不合格")==0)
                {
                    initDataRed(m_cWorkSheet3,line+i,column+(j*2));
                }
//                else
//                {
//                     initData(m_cWorkSheet3,line+i,column+(j*2),data);
//                }
            }
            else
            {
                strToCharText(data,powerData.powerData[i][j]);
                //initData(m_cWorkSheet3,line+i,column+(j*2),data);
            }
            aline.append(QVariant(data));
            aline.append(QVariant("\0"));
        }
        QVariant conv(aline);
        rowData.append(conv);

    }
    QVariant totalData(rowData);
    QString position="C"+QString::number(12)+":"+"O"+QString::number(38);
    QAxObject *range=m_cWorkSheet3->querySubObject("Range(QString)",position);
    range->setProperty("Value",totalData);
    return ;
}

//增加表格
void CExecl::addSheet(QAxObject* &pSheet, const char *sheetName)
{
    //int intCount = m_cWorkSheets ->property("Count").toInt();//获取工资表数量
    QAxObject *pLastSheet = m_cWorkSheets->querySubObject("Item(int)",1);
    m_cWorkSheets->querySubObject("Add(QVariant)",pLastSheet->asVariant());
    pSheet = m_cWorkSheets->querySubObject("Item(int)",1);
    pLastSheet->dynamicCall("Move(QVariant)",pSheet->asVariant());
    pSheet->setProperty("Name",sheetName);
    return ;
}

//表格控制
void CExecl::sheetControl()
{
    //合并标题
    mergeTitle(m_cWorkSheet,TITLE1);
    mergeTitle(m_cWorkSheet2,TITLE2);
    mergeTitle(m_cWorkSheet3,TITLE3,15);
    sheetConfiguration();//表格配置
    //qDebug("LINE:%d mergeCellControl_1();",__LINE__);
    mergeCellControl_1();//合并表格
    //qDebug("LINE:%d  mergeCellControl_2();",__LINE__);
    mergeCellControl_2();
    //qDebug("LINE:%d  mergeCellControl_3()",__LINE__);
    mergeCellControl_3(); 
    //qDebug("LINE:%d  initDataControl_1()",__LINE__);
    initDataControl_1();//设置初值
    //qDebug("LINE:%d  initDataControl_2()",__LINE__);
    initDataControl_2();
    //qDebug("LINE:%d  initDataControl_3()",__LINE__);
    initDataControl_3();

}

//设置表格各种参数
void CExecl::sheetConfiguration()
{
    //表格1操作***************************************************************************
    QString merge_cell;
    merge_cell.append(QChar(1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(4));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(18 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(9));  //终止行
    QAxObject *merge_range1_1 = m_cWorkSheet->querySubObject("Range(const QString&)", merge_cell);
    merge_range1_1->setProperty("WrapText", true);
    merge_range1_1->setProperty("HorizontalAlignment", -4131);//水平
    merge_range1_1->setProperty("VerticalAlignment", -4160);//垂直
    QAxObject* border1_1 =merge_range1_1->querySubObject("Borders");
    border1_1->setProperty("Color", QColor(0, 0, 0));
    delete merge_range1_1;

    //表格1数据部分
    merge_cell.clear();
    merge_cell.append(QChar(1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(11));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(18 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(38));  //终止行
    QAxObject *merge_range1_2 = m_cWorkSheet->querySubObject("Range(const QString&)", merge_cell);
    merge_range1_2->setProperty("WrapText", true);
    merge_range1_2->setProperty("HorizontalAlignment", -4108);//水平居中
    merge_range1_2->setProperty("VerticalAlignment", -4108);//垂直
    QAxObject *border1_2 =merge_range1_2->querySubObject("Borders");
    border1_2->setProperty("Color", QColor(0, 0, 0));
    delete merge_range1_2;

    //表格1字体部分
    merge_cell.clear();
    merge_cell.append(QChar(1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(4));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(18 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(41));  //终止行
    QAxObject *merge_range1_3 = m_cWorkSheet->querySubObject("Range(const QString&)", merge_cell);
    merge_range1_3->setProperty("WrapText", true);
    QAxObject *font1_3=merge_range1_3->querySubObject("Font");
    font1_3->setProperty("Size", 10);
    font1_3->setProperty("Name", QStringLiteral("宋体"));  //设置单*/元格字体
    delete merge_range1_3;
    //qDebug("LINE:%d font1_3",__LINE__);

    //表格2操作*******************************************************************************************//
    merge_cell.clear();
    merge_cell.append(QChar(1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(4));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(18 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(9));  //终止行
    QAxObject *merge_range2_1 = m_cWorkSheet2->querySubObject("Range(const QString&)", merge_cell);
    merge_range2_1->setProperty("WrapText", true);
    merge_range2_1->setProperty("HorizontalAlignment", -4131);//水平
    merge_range2_1->setProperty("VerticalAlignment", -4160);//垂直
    QAxObject *border2_1 =merge_range2_1->querySubObject("Borders");
    border2_1->setProperty("Color", QColor(0, 0, 0));
    delete merge_range2_1;

    //表格2数据部分
    merge_cell.clear();
    merge_cell.append(QChar(1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(11));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(18 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(37));  //终止行
    QAxObject *merge_range2_2 = m_cWorkSheet2->querySubObject("Range(const QString&)", merge_cell);
    merge_range2_2->setProperty("WrapText", true);
    merge_range2_2->setProperty("HorizontalAlignment", -4108);//水平居中
    merge_range2_2->setProperty("VerticalAlignment", -4108);//垂直
    QAxObject *border2_2 =merge_range2_2->querySubObject("Borders");
    border2_2->setProperty("Color", QColor(0, 0, 0));
    delete merge_range2_2;

    //表格2字体部分
    merge_cell.clear();
    merge_cell.append(QChar(1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(4));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(18 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(40));  //终止行
    QAxObject *merge_range2_3 = m_cWorkSheet2->querySubObject("Range(const QString&)", merge_cell);
    merge_range2_3->setProperty("WrapText", true);
    QAxObject *font2_3=merge_range2_3->querySubObject("Font");
    font2_3->setProperty("Size", 10);
    font2_3->setProperty("Name", QStringLiteral("宋体"));  //设置单*/元格字体
    delete merge_range2_3;

    //表格3操作*******************************************************************************************//
    merge_cell.clear();
    merge_cell.append(QChar(1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(4));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(15 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(9));  //终止行
    QAxObject *merge_range3_1 = m_cWorkSheet3->querySubObject("Range(const QString&)", merge_cell);
    merge_range3_1->setProperty("WrapText", true);
    merge_range3_1->setProperty("HorizontalAlignment", -4131);//水平
    merge_range3_1->setProperty("VerticalAlignment", -4160);//垂直
    QAxObject *border3_1 =merge_range3_1->querySubObject("Borders");
    border3_1->setProperty("Color", QColor(0, 0, 0));
    delete merge_range3_1;

    //表格3数据部分
    merge_cell.clear();
    merge_cell.append(QChar(1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(11));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(15 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(38));  //终止行
    QAxObject *merge_range3_2 = m_cWorkSheet3->querySubObject("Range(const QString&)", merge_cell);
    merge_range3_2->setProperty("WrapText", true);
    merge_range3_2->setProperty("HorizontalAlignment", -4108);//水平居中
    merge_range3_2->setProperty("VerticalAlignment", -4108);//垂直
    QAxObject *border3_2 =merge_range3_2->querySubObject("Borders");
    border3_2->setProperty("Color", QColor(0, 0, 0));
    delete merge_range3_2;

    //表格3字体部分
    merge_cell.clear();
    merge_cell.append(QChar(1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(4));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(15 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(41));  //终止行
    QAxObject *merge_range3_3 = m_cWorkSheet3->querySubObject("Range(const QString&)", merge_cell);
    merge_range3_3->setProperty("WrapText", true);
    QAxObject *font3_3=merge_range3_3->querySubObject("Font");
    font3_3->setProperty("Size", 10);
    font3_3->setProperty("Name", QStringLiteral("宋体"));  //设置单*/元格字体
    delete merge_range3_3;

}

//控制固定的数据写入
void CExecl::initDataControl_1()
{
    //12
    QList<QVariant> rowData;
    QList<QVariant> aline;
    for(int i=0;i<3;i++)
    {
        aline.append(QVariant("最大值"));
        aline.append(QVariant("平均值"));
        aline.append(QVariant("最小值"));
        aline.append(QVariant("95%值"));
        aline.append(QVariant("结论"));
//        initData(m_cWorkSheet,line,++column,"最大值");
//        initData(m_cWorkSheet,line,++column,"平均值");
//        initData(m_cWorkSheet,line,++column,"最小值");
//        initData(m_cWorkSheet,line,++column,"95%值");
//        initData(m_cWorkSheet,line,++column,"结论");
    }
    QVariant conv(aline);
    QString position="C"+QString::number(12)+":"+"Q"+QString::number(12);
    QAxObject *range=m_cWorkSheet->querySubObject("Range(QString)",position);
    range->setProperty("Value",conv);
    range->clear();

    QList<QVariant> rowData2;
    char data[10];
    for(int i=2;i<=25;i++)
    {
        QList<QVariant> aline2;
        memset(data,0,sizeof(data));
        sprintf(data,"%d",i);
        aline2.append(data);
        QVariant conv(aline2);
        rowData2.append(conv);
        //initData(m_cWorkSheet2,++line,column,data);
    }


    QVariant totalData(rowData2);
    position="B"+QString::number(14)+":"+"B"+QString::number(37);
    range=m_cWorkSheet->querySubObject("Range(QString)",position);
    range->setProperty("Value",totalData);
}

void CExecl::initDataControl_2()
{
    //12
    QList<QVariant> rowData;
    QList<QVariant> aline;
    for(int i=0;i<3;i++)
    {
        aline.append(QVariant("最大值"));
        aline.append(QVariant("平均值"));
        aline.append(QVariant("最小值"));
        aline.append(QVariant("95%值"));
        aline.append(QVariant("结论"));
//        initData(m_cWorkSheet2,line,++column,"最大值");
//        initData(m_cWorkSheet2,line,++column,"平均值");
//        initData(m_cWorkSheet2,line,++column,"最小值");
//        initData(m_cWorkSheet2,line,++column,"95%值");
//        initData(m_cWorkSheet2,line,++column,"结论");
    }
    QVariant conv(aline);
    QString position="C"+QString::number(12)+":"+"Q"+QString::number(12);
    QAxObject *range=m_cWorkSheet2->querySubObject("Range(QString)",position);
    range->setProperty("Value",conv);
    range->clear();
    position.clear();


    QList<QVariant> rowData2;
    char data[10];
    for(int i=2;i<=25;i++)
    {  
        QList<QVariant> aline2;
        memset(data,0,sizeof(data));
        sprintf(data,"%d",i);
        aline2.append(data);
        QVariant conv(aline2);
        rowData2.append(conv);
        //initData(m_cWorkSheet2,++line,column,data);
    }
    QVariant totalData(rowData2);
    position="B"+QString::number(14)+":"+"B"+QString::number(37);
    range=m_cWorkSheet2->querySubObject("Range(QString)",position);
    range->setProperty("Value",totalData);
}

void CExecl::initDataControl_3()
{
    QList<QVariant> rowData;

    for(int i=0;i<3;i++)
    {
        QList<QVariant> aline;
        aline.append(QVariant("L1"));
        QVariant conv(aline);
        rowData.append(conv);

        aline.clear();
        aline.append(QVariant("L2"));
        QVariant conv2(aline);
        rowData.append(conv2);

        aline.clear();
        aline.append(QVariant("L3"));
        QVariant conv3(aline);
        rowData.append(conv3);

//        initData(m_cWorkSheet3,++line,column,"L1");
//        initData(m_cWorkSheet3,++line,column,"L2");
//        initData(m_cWorkSheet3,++line,column,"L3");
    }

    for(int i=0;i<4;i++)
    {
        QList<QVariant> aline;
        aline.append(QVariant("L1"));
        QVariant conv(aline);
        rowData.append(conv);

        aline.clear();
        aline.append(QVariant("L2"));
        QVariant conv2(aline);
        rowData.append(conv2);

        aline.clear();
        aline.append(QVariant("L3"));
        QVariant conv3(aline);
        rowData.append(conv3);

        aline.clear();
        aline.append(QVariant("总"));
        QVariant conv4(aline);
        rowData.append(conv4);

//        initData(m_cWorkSheet3,++line,column,"L1");
//        initData(m_cWorkSheet3,++line,column,"L2");
//        initData(m_cWorkSheet3,++line,column,"L3");
//        initData(m_cWorkSheet3,++line,column,"总");
    }
    QVariant totalData(rowData);
    QString position="B"+QString::number(14)+":"+"B"+QString::number(38);
    QAxObject *range=m_cWorkSheet3->querySubObject("Range(QString)",position);
    range->setProperty("Value",totalData);
}


//初始化固定的数据
void CExecl::initDataRed(QAxObject * &workSheet, int row, int column)
{

    QAxObject * cell = workSheet->querySubObject("Cells(int, int)", row, column);
    QAxObject *font=cell->querySubObject("Font");
    font->setProperty("Color", QColor(250, 0, 0));  //设置单元格字体颜色（红色）
    //设置边框颜色
    delete cell;
    return ;
}


void CExecl::initData(QAxObject *&workSheet, int row, int column, const char *data)
{
     QAxObject * cell = workSheet->querySubObject("Cells(int, int)", row, column);
     cell->setProperty("Value", data);
     delete cell;
     return ;
}



//合并表格操作
void CExecl::mergeCellOPeration(QAxObject * &workSheet, int row1, int column1, int row2, int column2,
    const char *data, bool middle)
{

    QAxObject *cell_first = workSheet->querySubObject("Cells(int,int)", row1,column1 );
    cell_first->setProperty("Value", data);  //设置单元格值
    QString merge_cell;
    merge_cell.append(QChar(column1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(row1));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(column2 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(row2));  //终止行
    QAxObject *merge_range = workSheet->querySubObject("Range(const QString&)", merge_cell);
    merge_range->setProperty("WrapText", true);
    merge_range->setProperty("MergeCells", true);  //合并单元格
    if(true==middle)
    {
        merge_range->setProperty("HorizontalAlignment", -4108);//水平居中
        merge_range->setProperty("VerticalAlignment", -4108);//垂直居中
    }
    else
    {
        //merge_range->setProperty("HorizontalAlignment", -4131);//水平
        merge_range->setProperty("VerticalAlignment", -4160);//垂直
    }

    delete merge_range;
}

void CExecl::mergeCellOPeration(QAxObject *&workSheet, int row1, int column1, int row2, int column2, const char *data)
{
    QAxObject *cell_first = workSheet->querySubObject("Cells(int,int)", row1,column1 );
    cell_first->setProperty("Value", data);  //设置单元格值
    QString merge_cell;
    merge_cell.append(QChar(column1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(row1));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(column2 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(row2));  //终止行
    QAxObject *merge_range = workSheet->querySubObject("Range(const QString&)", merge_cell);
    merge_range->setProperty("MergeCells", true);  //合并单元格

    delete merge_range;
}

void CExecl::mergeCellOPeration(QAxObject *&workSheet, int row1, int column1, int row2, int column2)
{
    QString merge_cell;
    merge_cell.append(QChar(column1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(row1));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(column2 - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(row2));  //终止行
    QAxObject *merge_range = workSheet->querySubObject("Range(const QString&)", merge_cell);
   // merge_range->setProperty("WrapText", true);
    merge_range->setProperty("MergeCells", true);  //合并单元格
    delete merge_range;
}


//合并表格
void CExecl::mergeCellControl_1()
{

    //4_1->4_18 监测位置
//*****************************************表格1开始*************************************************//
    //qDebug("mergeCellControl_1 %d\n",__LINE__);
    int line=4;
    mergeCellOPeration(m_cWorkSheet,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet,line,3,line,7);
    mergeCellOPeration(m_cWorkSheet,line,8,line,9);
    mergeCellOPeration(m_cWorkSheet,line,10,line,18);

    //5_1->5_18
    line=5;
    mergeCellOPeration(m_cWorkSheet,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet,line,3,line,7);
    mergeCellOPeration(m_cWorkSheet,line,8,line,9);
    mergeCellOPeration(m_cWorkSheet,line,10,line,18);

    //6_1->6_18
    line=6;
    mergeCellOPeration(m_cWorkSheet,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet,line,3,line,7);
    mergeCellOPeration(m_cWorkSheet,line,8,line,9);
    mergeCellOPeration(m_cWorkSheet,line,10,line,13);
    mergeCellOPeration(m_cWorkSheet,line,14,line,15,"CT：");
    mergeCellOPeration(m_cWorkSheet,line,16,line,18);

    //7_1->7_18
    line=7;
    mergeCellOPeration(m_cWorkSheet,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet,line,3,line,7);
    mergeCellOPeration(m_cWorkSheet,line,8,line,9);
    mergeCellOPeration(m_cWorkSheet,line,10,line,18);

    //8_1->8_18
    line=8;
    mergeCellOPeration(m_cWorkSheet,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet,line,3,line,7);
    mergeCellOPeration(m_cWorkSheet,line,8,line,9);
    mergeCellOPeration(m_cWorkSheet,line,10,line,18);

    //9_1->9_18
    line=9;
    mergeCellOPeration(m_cWorkSheet,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet,line,3,line,18);

    //10_1->10_18
    line=10;
    mergeCellOPeration(m_cWorkSheet,line,1,line,18);

    //11_1->12_18
    line=11;
    mergeCellOPeration(m_cWorkSheet,line,1,12,2,"参数");
    mergeCellOPeration(m_cWorkSheet,line,3,line,7,"L1");
    mergeCellOPeration(m_cWorkSheet,line,8,line,12,"L2");
    mergeCellOPeration(m_cWorkSheet,line,13,line,17,"L3");
    mergeCellOPeration(m_cWorkSheet,line,18,12,18,"标准");
   // qDebug("mergeCellControl_1 %d\n",__LINE__);
    //13_1->13_2
    line=13;
    mergeCellOPeration(m_cWorkSheet,line,1,line,2,"基波电压（V）");

    //14_1->37_1
    line=14;
    mergeCellOPeration(m_cWorkSheet,line,1,37,1,"电压谐波\n含有率(%)");

    //38_1->38_2
    line=38;
    mergeCellOPeration(m_cWorkSheet,line,1,line,2,"电压谐波总畸变率(％)",false);

    //39_1->39_18
    line=39;
    mergeCellOPeration(m_cWorkSheet,line,1,line,18);

    //40_1->40_18
    line=40;
    mergeCellOPeration(m_cWorkSheet,line,1,line,13);
    mergeCellOPeration(m_cWorkSheet,line,14,line,18,"报告生成单位: Company1");

    //40_1->40_18
    line=41;
    mergeCellOPeration(m_cWorkSheet,line,14,line,18,"报告生成操作员:Operator2");

    //优化
    QList<QVariant> rowData;

    listAdd(rowData,"监测位置：");
    listAdd(rowData,"监测线路：");
    listAdd(rowData,"电压等级：");
    listAdd(rowData,"基准短路容量：");
    listAdd(rowData,"用户协议容量：");
    listAdd(rowData,"工况：");

    QVariant totalData(rowData);
    QString position="A"+QString::number(4)+":"+"A"+QString::number(9);
    QAxObject *range=m_cWorkSheet->querySubObject("Range(QString)",position);
    range->setProperty("Value",totalData);


    QList<QVariant> rowData2;
    listAdd(rowData2,"监测时间：");
    listAdd(rowData2,"现场操作员：");
    listAdd(rowData2,"PT：");
    listAdd(rowData2,"最小短路容量：");
    listAdd(rowData2,"供电设备容量：");

    QVariant totalData2(rowData2);
    QString position2="H"+QString::number(4)+":"+"H"+QString::number(8);
    QAxObject *range2=m_cWorkSheet->querySubObject("Range(QString)",position2);
    range2->setProperty("Value",totalData2);



//*****************************************表格1结束*************************************************//


    //qDebug("mergeCellControl_1 %d\n",__LINE__);
    return ;
}

//合并表格控制2
void CExecl::mergeCellControl_2()
{
    //*****************************************表格2开始*************************************************//
    //qDebug("mergeCellControl_2 %d\n",__LINE__);
    int line=4;
    mergeCellOPeration(m_cWorkSheet2,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet2,line,3,line,7);
    mergeCellOPeration(m_cWorkSheet2,line,8,line,9);
    mergeCellOPeration(m_cWorkSheet2,line,10,line,18);

    //5_1->5_18
    line=5;
    mergeCellOPeration(m_cWorkSheet2,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet2,line,3,line,7);
    mergeCellOPeration(m_cWorkSheet2,line,8,line,9);
    mergeCellOPeration(m_cWorkSheet2,line,10,line,18);

    //6_1->6_18
    line=6;
    mergeCellOPeration(m_cWorkSheet2,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet2,line,3,line,7);
    mergeCellOPeration(m_cWorkSheet2,line,8,line,9);
    mergeCellOPeration(m_cWorkSheet2,line,10,line,13);
    mergeCellOPeration(m_cWorkSheet2,line,14,line,15,"CT：");
    mergeCellOPeration(m_cWorkSheet2,line,16,line,18);

    //7_1->7_18
    line=7;
    mergeCellOPeration(m_cWorkSheet2,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet2,line,3,line,7);
    mergeCellOPeration(m_cWorkSheet2,line,8,line,9);
    mergeCellOPeration(m_cWorkSheet2,line,10,line,18);

    //8_1->8_18
    line=8;
    mergeCellOPeration(m_cWorkSheet2,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet2,line,3,line,7);
    mergeCellOPeration(m_cWorkSheet2,line,8,line,9);
    mergeCellOPeration(m_cWorkSheet2,line,10,line,18);
    //qDebug("mergeCellControl_2 %d\n",__LINE__);
    //9_1->9_18
    line=9;
    mergeCellOPeration(m_cWorkSheet2,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet2,line,3,line,18);

    //10_1->10_18
    line=10;
    mergeCellOPeration(m_cWorkSheet2,line,1,line,18);

    //11_1->12_18
    line=11;
    mergeCellOPeration(m_cWorkSheet2,line,1,12,2,"参数");
    mergeCellOPeration(m_cWorkSheet2,line,3,line,7,"L1");
    mergeCellOPeration(m_cWorkSheet2,line,8,line,12,"L2");
    mergeCellOPeration(m_cWorkSheet2,line,13,line,17,"L3");
    mergeCellOPeration(m_cWorkSheet2,line,18,12,18,"标准");

    //13_1->13_2
    line=13;
    mergeCellOPeration(m_cWorkSheet2,line,1,line,2,"基波电流（A）");

    //14_1->37_1
    line=14;
    mergeCellOPeration(m_cWorkSheet2,line,1,37,1,"电流谐波\n有效值(A)");

    //38_1->38_2


    //39_1->39_18
    line=38;
    mergeCellOPeration(m_cWorkSheet2,line,1,line,18);

    //40_1->40_18
    line=39;
    mergeCellOPeration(m_cWorkSheet2,line,1,line,13);
    mergeCellOPeration(m_cWorkSheet2,line,14,line,18,"报告生成单位: Company1");

    //40_1->40_18
    line=40;
    mergeCellOPeration(m_cWorkSheet2,line,14,line,18,"报告生成操作员:Operator2");
    //qDebug("LINE:%d mergecel2",__LINE__);

    //优化
    QList<QVariant> rowData;

    listAdd(rowData,"监测位置：");
    listAdd(rowData,"监测线路：");
    listAdd(rowData,"电压等级：");
    listAdd(rowData,"基准短路容量：");
    listAdd(rowData,"用户协议容量：");
    listAdd(rowData,"工况：");

    QVariant totalData(rowData);
    QString position="A"+QString::number(4)+":"+"A"+QString::number(9);
    QAxObject *range=m_cWorkSheet2->querySubObject("Range(QString)",position);
    range->setProperty("Value",totalData);


    QList<QVariant> rowData2;
    listAdd(rowData2,"监测时间：");
    listAdd(rowData2,"现场操作员：");
    listAdd(rowData2,"PT：");
    listAdd(rowData2,"最小短路容量：");
    listAdd(rowData2,"供电设备容量：");

    QVariant totalData2(rowData2);
    QString position2="H"+QString::number(4)+":"+"H"+QString::number(8);
    QAxObject *range2=m_cWorkSheet2->querySubObject("Range(QString)",position2);
    range2->setProperty("Value",totalData2);
    //*****************************************表格2结束*************************************************//
    return ;
}

//合并表格控制3
void CExecl::mergeCellControl_3()
{
    //qDebug("mergeCellControl_2 %d\n",__LINE__);
    int line=4;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet3,line,3,line,6);
    mergeCellOPeration(m_cWorkSheet3,line,7,line,8);
    mergeCellOPeration(m_cWorkSheet3,line,9,line,15);

    //5_1->5_18
    line=5;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet3,line,3,line,6);
    mergeCellOPeration(m_cWorkSheet3,line,7,line,8);
    mergeCellOPeration(m_cWorkSheet3,line,9,line,15);

    //6_1->6_18
    line=6;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet3,line,3,line,6);
    mergeCellOPeration(m_cWorkSheet3,line,7,line,8);
    mergeCellOPeration(m_cWorkSheet3,line,9,line,10);
    mergeCellOPeration(m_cWorkSheet3,line,11,line,12,"CT：");
    mergeCellOPeration(m_cWorkSheet3,line,13,line,15);

    //7_1->7_18
    line=7;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet3,line,3,line,6);
    mergeCellOPeration(m_cWorkSheet3,line,7,line,8);
    mergeCellOPeration(m_cWorkSheet3,line,9,line,15);
    //qDebug("mergeCellControl_2 %d\n",__LINE__);
    //8_1->8_18
    line=8;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet3,line,3,line,6);
    mergeCellOPeration(m_cWorkSheet3,line,7,line,8);
    mergeCellOPeration(m_cWorkSheet3,line,9,line,15);

    //9_1->9_18
    line=9;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,2);
    mergeCellOPeration(m_cWorkSheet3,line,3,line,15);

    //10_1->10_18
    line=10;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,15);

    //11_1->12_18
    line=11;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,2,"参数");
    mergeCellOPeration(m_cWorkSheet3,line,3,line,4,"最大值");
    mergeCellOPeration(m_cWorkSheet3,line,5,line,6,"平均值");
    mergeCellOPeration(m_cWorkSheet3,line,7,line,8,"最小值");
    mergeCellOPeration(m_cWorkSheet3,line,9,line,10,"95％值");
    mergeCellOPeration(m_cWorkSheet3,line,11,line,12,"标准");
    mergeCellOPeration(m_cWorkSheet3,line,13,line,15,"结论");

    //12_1->12_2
    line=12;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,2,"频率（Hz）");
    line=13;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,2,"电压负序不平衡（％）");
    //qDebug("mergeCellControl_2 %d\n",__LINE__);
    //12_3->38->15
    for(int i=12;i<=38;i++)
    {
        line=i;
        mergeCellOPeration(m_cWorkSheet3,line,3,line,4);
        mergeCellOPeration(m_cWorkSheet3,line,5,line,6);
        mergeCellOPeration(m_cWorkSheet3,line,7,line,8);
        mergeCellOPeration(m_cWorkSheet3,line,9,line,10);
        mergeCellOPeration(m_cWorkSheet3,line,11,line,12);
        mergeCellOPeration(m_cWorkSheet3,line,13,line,15);
    }

    //12_1->38_1
    int colum=1;
    mergeCellOPeration(m_cWorkSheet3,14,colum,16,colum,"短时间闪变");
    mergeCellOPeration(m_cWorkSheet3,17,colum,19,colum,"长时间闪变");
    mergeCellOPeration(m_cWorkSheet3,20,colum,22,colum,"电压偏差(%)(负\n号表示下偏差)");
    mergeCellOPeration(m_cWorkSheet3,23,colum,26,colum,"有功功率(kW)");
    mergeCellOPeration(m_cWorkSheet3,27,colum,30,colum,"无功功率(kvar)");
    mergeCellOPeration(m_cWorkSheet3,31,colum,34,colum,"视在功率(kVA)");
    mergeCellOPeration(m_cWorkSheet3,35,colum,38,colum,"功率因数");

    line=39;
    mergeCellOPeration(m_cWorkSheet3,line,1,line,15);

    //40_1->40_18
    line=40;

    mergeCellOPeration(m_cWorkSheet3,line,11,line,15,"报告生成单位: Company1");

    //40_1->40_18
    line=41;
    mergeCellOPeration(m_cWorkSheet3,line,11,line,15,"报告生成操作员:Operator2");

    //优化
    QList<QVariant> rowData;

    listAdd(rowData,"监测位置：");
    listAdd(rowData,"监测线路：");
    listAdd(rowData,"电压等级：");
    listAdd(rowData,"基准短路容量：");
    listAdd(rowData,"用户协议容量：");
    listAdd(rowData,"工况：");

    QVariant totalData(rowData);
    QString position="A"+QString::number(4)+":"+"A"+QString::number(9);
    QAxObject *range=m_cWorkSheet3->querySubObject("Range(QString)",position);
    range->setProperty("Value",totalData);


    QList<QVariant> rowData2;
    listAdd(rowData2,"监测时间：");
    listAdd(rowData2,"现场操作员：");
    listAdd(rowData2,"PT：");
    listAdd(rowData2,"最小短路容量：");
    listAdd(rowData2,"供电设备容量：");

    QVariant totalData2(rowData2);
    QString position2="G"+QString::number(4)+":"+"G"+QString::number(8);
    QAxObject *range2=m_cWorkSheet3->querySubObject("Range(QString)",position2);
    range2->setProperty("Value",totalData2);
    //qDebug("LINE:%d mergecel3",__LINE__);

    //*****************************************表格3结束*************************************************//
    return ;
}
//合并标题
void CExecl::mergeTitle(QAxObject *&workSheet, const char *title, int column)
{

    QAxObject *cell_1_1 = workSheet->querySubObject("Cells(int,int)", 1,1 );
    workSheet->setProperty("HorizontalAlignment", -4108);
    workSheet->setProperty("VerticalAlignment", -4108);
    cell_1_1->setProperty("Value",title);  //设置单元格值
    QString merge_cell;
    merge_cell.append(QChar(1 - 1 + 'A'));  //初始列
    merge_cell.append(QString::number(1));  //初始行
    merge_cell.append(":");
    merge_cell.append(QChar(column - 1 + 'A'));  //终止列
    merge_cell.append(QString::number(3));  //终止行
    //qDebug()<<"merge_cell="<<merge_cell;
    QAxObject *merge_range = workSheet->querySubObject("Range(const QString&)", merge_cell);
    merge_range->setProperty("HorizontalAlignment", -4108);//水平居中
    merge_range->setProperty("VerticalAlignment", -4108);//垂直居中
    QAxObject *font=merge_range->querySubObject("Font");
    font->setProperty("Name", QStringLiteral("宋体"));  //设置单元格字体
    font->setProperty("Bold", true);  //设置单元格字体加粗
    font->setProperty("Size", 15);  //设置单元格字体大小
    merge_range->setProperty("WrapText", true);
    merge_range->setProperty("MergeCells", true);  //合并单元格
    //qDebug("LINE:%d mergeTitle\n",__LINE__);
    delete cell_1_1;
    delete merge_range;
    //qDebug("LINE:%d mergeTitle=\n",__LINE__);
    return ;
    //delete font;
}


//导出表格
int CExecl::exportForm(CVoltageQuality &voltageData,CCurrentQuality &currentData,CPowerQuality &powerData,QString name)
{
    //初始化数据
    clockid_t time1=clock();
    int  value=templateInit();
    if(value!=0)
    {
        if(-1==value)
        {
            OleUninitialize();
            return -1;
        }
        if(-2==value)
        {
            OleUninitialize();
            return -2;
        }
        if(-3==value)
        {
            return -4;
        }
    }
    clockid_t time2=clock();
    qDebug()<<"time2-time1"<<time2-time1;
    //写数据
    writeDataControl_1(voltageData);
    writeDataControl_2(currentData);
    writeDataControl_3(powerData);
    //结尾配置数据
    bool bo=EndConfiguration(name);
    clockid_t time3=clock();
    qDebug()<<"time2-time1"<<time3-time2;
    if(bo==false)
    {
        return -3;
    }
    return 0;

}

//创建一个模板
int CExecl::createATemplate(QString name)
{
    int value=initialization();
    if(value!=0)
    {
        return -1;
    }
    bool bo=EndConfiguration(name,false);
    if(bo==true)
    {
        m_cTemplateName=name;
        return 0;
    }
    else
    {
        return -2;
    }

}

int CExecl::exportFormless(CVoltageQuality &voltageData, CCurrentQuality &currentData, CPowerQuality &powerData, QString name)
{
    int value=initialization();
    if(value!=0)
    {
        return -1;
    }
    writeDataControl_1(voltageData);
    writeDataControl_2(currentData);
    writeDataControl_3(powerData);
    bool bo=EndConfiguration(name);
    if(bo==false)
    {
        return -2;
    }
    return 0;
}

void CExecl::setTemplateName(QString name)
{
    m_cTemplateName=name;
    return ;
}

//测试
CVoltageQuality CExecl::test_1()
{
    CVoltageQuality testData;
    testData.workingCondition.monitoingPsoition="place";
    testData.workingCondition.monitoingTime="2016/06/22 11:34:52 - 2016/06/22 11:37:52";
    testData.workingCondition.monitoingLine="Line1";
    testData.workingCondition.fieldOperator="e6500";
    testData.workingCondition.voltageLevel="380V";
    testData.workingCondition.dataPT="1.0000";
    testData.workingCondition.dataCT="1.0000";
    testData.workingCondition.referenceShortCircuitCpy="10.000MVA";
    testData.workingCondition.minimumShortCircuitCpy="10.000MVA";
    testData.workingCondition.userAgreementCpy="10.000MVA";
    testData.workingCondition.powerSuppluEquipmentCpy="10.000MVA";
    testData.workingCondition.workCondition="Work1";
    for(int i=0;i<26;i++)
    {
        for(int j=0;j<3;j++)
        {
            for(int k=0;k<5;k++)
            {
                if(4==k)
                {
                    static int sum=0;
                    sum++;
                    if(0==sum%2)
                        testData.voltageData[i][j][k]="合格";
                    else
                        testData.voltageData[i][j][k]="不合格";
                }
                else
                {
                    char buf[108]={0};
                    sprintf(buf,"%d.%d%d00",i,j,k);
                    testData.voltageData[i][j][k]=QString(QLatin1String(buf));
                }
            }
        }
        char buf[108]={0};
        sprintf(buf,"0%d.0%d0",i,i);
        testData.voltageDataStandard[i]=QString(QLatin1String(buf));
    }
    return testData;
}

CCurrentQuality CExecl::test_2()
{
    CCurrentQuality  testData;
    testData.workingCondition.monitoingPsoition="place";
    testData.workingCondition.monitoingTime="2016/06/22 11:34:52 - 2016/06/22 11:37:52";
    testData.workingCondition.monitoingLine="Line1";
    testData.workingCondition.fieldOperator="e6500";
    testData.workingCondition.voltageLevel="380V";
    testData.workingCondition.dataPT="1.0000";
    testData.workingCondition.dataCT="1.0000";
    testData.workingCondition.referenceShortCircuitCpy="10.000MVA";
    testData.workingCondition.minimumShortCircuitCpy="10.000MVA";
    testData.workingCondition.userAgreementCpy="10.000MVA";
    testData.workingCondition.powerSuppluEquipmentCpy="10.000MVA";
    testData.workingCondition.workCondition="Work1";
    for(int i=0;i<25;i++)
    {
        for(int j=0;j<3;j++)
        {
            for(int k=0;k<5;k++)
            {
                if(4==k)
                {
                    static int sum=0;
                    sum++;
                    if(0==sum%2)
                        testData.currentData[i][j][k]="合格";
                    else
                        testData.currentData[i][j][k]="不合格";
                }
                else
                {
                    char buf[108]={0};
                    sprintf(buf,"%d.%d%d00",i,j,k);
                    testData.currentData[i][j][k]=QString(QLatin1String(buf));
                }
            }
        }
        char buf[108]={0};
        sprintf(buf,"%d.0--%d0",i,i);
        testData.currentDataStandard[i]=QString(QLatin1String(buf));
    }
    return testData;
}

CPowerQuality CExecl::test_3()
{
    CPowerQuality testData;
    testData.workingCondition.monitoingPsoition="place";
    testData.workingCondition.monitoingTime="2016/06/22 11:34:52 - 2016/06/22 11:37:52";
    testData.workingCondition.monitoingLine="Line1";
    testData.workingCondition.fieldOperator="e6500";
    testData.workingCondition.voltageLevel="380V";
    testData.workingCondition.dataPT="1.0000";
    testData.workingCondition.dataCT="1.0000";
    testData.workingCondition.referenceShortCircuitCpy="10.000MVA";
    testData.workingCondition.minimumShortCircuitCpy="10.000MVA";
    testData.workingCondition.userAgreementCpy="10.000MVA";
    testData.workingCondition.powerSuppluEquipmentCpy="10.000MVA";
    testData.workingCondition.workCondition="Work1";
    for(int i=0;i<27;i++)
    {
        for(int j=0;j<6;j++)
        {
            if(5==j)
            {
                static int sum=0;
                sum++;
                if(0==sum%3)
                    testData.powerData[i][j]="合格";
                else
                    if(0==sum%2)
                        testData.powerData[i][j]="不合格";
                    else
                        testData.powerData[i][j]="合格率(%)96.111%";
            }
            else
            {
                char buf[108]={0};
                if(0==i%5)
                    sprintf(buf,"%d.%d00-/%d0.00",i,j,j);
                else
                    sprintf(buf,"%d.%d00",i,j);
                testData.powerData[i][j]=QString(QLatin1String(buf));
            }
        }
    }
    return testData;
}

//打开模板
int CExecl::templateInit()
{
    clockid_t time1=clock();
    HRESULT r = OleInitialize(0);
    if (r != S_OK && r != S_FALSE)
    {
        qWarning("Qt:初始化Ole失败（error %x）",(unsigned int)r);
        return -3;
    }
    m_cExcel=new QAxObject("Excel.Application");
    m_cExcel->setProperty("Visiblie", false);
    clockid_t time2=clock();
    qDebug()<<"time2-time1"<<time2-time1;
    m_cWorkBooks = m_cExcel->querySubObject("WorkBooks");
    m_cWorkBooks->dynamicCall("Add");
    m_cWorkBook= m_cWorkBooks->querySubObject("Open(QString&)",m_cTemplateName.toUtf8().data());
    if(NULL==m_cWorkBook)
    {
        m_cExcel->dynamicCall("Quit (void)");
        delete m_cExcel;
        return -1;
    }
    m_cWorkSheets = m_cWorkBook->querySubObject("WorkSheets");
    int count=m_cWorkSheets->property("Count").toInt();
    if(count!=3)
    {
        m_cWorkBook->dynamicCall("Close(Boolen)", false);
        m_cExcel->dynamicCall("Quit (void)");
        delete m_cExcel;
        return -2;
    }
    m_cWorkSheet = m_cWorkSheets->querySubObject("Item(int)", 1);
    m_cWorkSheet2 = m_cWorkSheets->querySubObject("Item(int)", 2);
    m_cWorkSheet3 = m_cWorkSheets->querySubObject("Item(int)", 3);
    return 0;
}

//设置行宽和列高
void CExecl::setWidthHeight()
{
    //1
    QAxObject *range = m_cWorkSheet->querySubObject("UsedRange");
    QAxObject * cells = range->querySubObject("Columns");
    cells->dynamicCall("AutoFit"); 
//    cells = range->querySubObject("Rows");
//    cells->dynamicCall("AutoFit");
    //2
    QAxObject *range2 = m_cWorkSheet2->querySubObject("UsedRange");
    QAxObject * cells2 = range2->querySubObject("Columns");
    cells2->dynamicCall("AutoFit");
//    cells2 = range2->querySubObject("Rows");
//    cells2->dynamicCall("AutoFit");
    //3
    QAxObject *range3 = m_cWorkSheet3->querySubObject("UsedRange");
    QAxObject * cells3 = range3->querySubObject("Columns");
    cells3->dynamicCall("AutoFit");
//    cells3 = range3->querySubObject("Rows");
//    cells3->dynamicCall("AutoFit");
    return ;
}

//结尾配置
bool CExecl::EndConfiguration(QString strName, bool adapt)
{
    if(adapt==true)
    {
        setWidthHeight();
    }
    QByteArray byteName=strName.toUtf8();
    char chName[1024]={0};
    strcpy(chName,byteName);
    QVariant value=m_cWorkBook->dynamicCall("SaveAs (const QString&, int, const QString&, const QString&, bool, bool)",
                        chName, 51, QString(""), QString(""), false, false);
    bool save=value.toBool();
    m_cWorkBook->dynamicCall("Close(Boolen)", false);
    m_cExcel->dynamicCall("Quit (void)");
    //手动释放
    delete m_cExcel;
    OleUninitialize();
    if(true==save)
    {
        return true;
    }
    return false;
}


