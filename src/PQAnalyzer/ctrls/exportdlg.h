#ifndef CEXPORTDLG_H
#define CEXPORTDLG_H

#include <QDialog>
#include <QDateTime>

class QLabel;
class QLineEdit;
class DoubleSlider;

class CExportDlg : public QDialog
{
    Q_OBJECT
public:
    explicit CExportDlg(QWidget *parent = 0);
    virtual ~CExportDlg();

public slots:
    void maxSliderChanged(float val);
    void minSliderChanged(float val);
    void btnPress();

protected:
    void _initCtrls();
    void _updateTimeLab();

protected:
    QLabel *m_pTimeLab;
    QLineEdit *m_pPathEdit;
    DoubleSlider *m_pDoubSlider;
    QDateTime m_startTime;
    QDateTime m_endTime;
};

#endif // CEXPORTDLG_H
