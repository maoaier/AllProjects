#include "unbalanceview.h"
#include <QComboBox>

CUnbalanceView::CUnbalanceView(QWidget *parent)
    : CStatViewBase(parent)
{
    m_mapFileName["pos_value_vol.dat"] = StatItemInfo("正序电压幅值", "V");
    m_mapFileName["neg_value_vol.dat"] = StatItemInfo("负序电压幅值", "V");
    m_mapFileName["zero_value_vol.dat"] = StatItemInfo("零序电压幅值", "V");

//    m_mapFileName["pos_ang_vol.dat"] = StatItemInfo("正序电压相位", "°");
//    m_mapFileName["neg_ang_vol.dat"] = StatItemInfo("负序电压相位", "°");
//    m_mapFileName["zero_ang_vol.dat"] = StatItemInfo("零序电压相位", "°");

    m_mapFileName["neg_unbalance_vol.dat"] = StatItemInfo("负序电压不平衡", "%");
    m_mapFileName["zero_unbalance_vol.dat"] = StatItemInfo("零序电压不平衡", "%");

//    m_mapFileName["pos_value_cur.dat"] = StatItemInfo("正序电流幅值", "A");
//    m_mapFileName["neg_value_cur.dat"] = StatItemInfo("负序电流幅值", "A");
//    m_mapFileName["zero_value_cur.dat"] = StatItemInfo("零序电流幅值", "A");

//    m_mapFileName["pos_ang_cur.dat"] = StatItemInfo("正序电流相位", "°");
//    m_mapFileName["neg_ang_cur.dat"] = StatItemInfo("负序电流相位", "°");
//    m_mapFileName["zero_ang_cur.dat"] = StatItemInfo("零序电流相位", "°");

//    m_mapFileName["neg_unbalance_cur.dat"] = StatItemInfo("负序电流不平衡", "%");
//    m_mapFileName["zero_unbalance_cur.dat"] = StatItemInfo("零序电流不平衡", "%");

    QMap<QString, StatItemInfo>::iterator ite = m_mapFileName.begin();
    for (; ite != m_mapFileName.end(); ++ite) {
        m_pCombDataSel->addItem(ite.value().chinese);
    }
    m_pCombDataSel->setCurrentIndex(0);
}

CUnbalanceView::~CUnbalanceView()
{

}
