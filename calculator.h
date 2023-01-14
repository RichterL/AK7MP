#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
QT_END_NAMESPACE

class Calculator : public QMainWindow
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);
    ~Calculator();

private:
    Ui::Calculator *ui;
    int getOperationPrecedence(QString &operation);
    int getOperationAssociativeness(QString &operation);
    void pushOperation(QString &operation);
    void handleError();
    void storeNumber();

private slots:
    void NumPressed();
    void DecimalButton();
    void MathButtons();
    void EqualButton();
    void SignButton();
    void ClearButton();
    void ClearAllButton();
    void OpenBracketButton();
    void CloseBracketButton();
    void PowerTwoButton();
    void PowerAnyButton();
};
#endif // CALCULATOR_H
