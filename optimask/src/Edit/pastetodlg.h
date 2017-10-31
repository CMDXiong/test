#ifndef PASTETODLG_H
#define PASTETODLG_H

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;

class PasteToDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PasteToDlg(QDialog *parent=0);
    ~PasteToDlg();

//    QBrush brush() const { return m_brush; }

//public slots:
//    void setBrush(const QBrush &brush);

//signals:
//    void brushChanged(const QBrush &brush);

//private slots:
//    void updateColor(int index);
//    void updateStyle(int index);

private:
    void createWidgets();
    void createLayout();
//    void createConnections();
//    void updateSwatches();

    QDoubleSpinBox *cellLayerSpinBox;
    QDoubleSpinBox *goalLayerSpinBox;
    QDoubleSpinBox *xSpinBox;
    QDoubleSpinBox *ySpinBox;
};

#endif // PASTETODLG_H
