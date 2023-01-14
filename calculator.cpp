#include "math.h"
#include "calculator.h"
#include "./ui_calculator.h"
#include <QStack>
#include <QQueue>
#include <QRegularExpression>

bool resetDisplay = false;
bool isDecimal = false;
bool errorState = false;
bool calculatedState = false;

QQueue<QString> outputQueue = {};
QStack<QString> operationsStack = {};

Calculator::Calculator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);
    ui->display->setText("");
    QPushButton *numButtons[10];
    for (int i = 0; i < 10; ++i) {
        QString buttonName = "button" + QString::number(i);
        numButtons[i] = Calculator::findChild<QPushButton *>(buttonName);
        connect(numButtons[i], SIGNAL(released()), this, SLOT(NumPressed()));
    }
    connect(ui->buttonDecimal, SIGNAL(released()), this, SLOT(DecimalButton()));
    connect(ui->buttonPlus, SIGNAL(released()), this, SLOT(MathButtons()));
    connect(ui->buttonMinus, SIGNAL(released()), this, SLOT(MathButtons()));
    connect(ui->buttonDiv, SIGNAL(released()), this, SLOT(MathButtons()));
    connect(ui->buttonTimes, SIGNAL(released()), this, SLOT(MathButtons()));
    connect(ui->buttonSign, SIGNAL(released()), this, SLOT(SignButton()));
    connect(ui->buttonParLeft, SIGNAL(released()), this, SLOT(OpenBracketButton()));
    connect(ui->buttonParRight, SIGNAL(released()), this, SLOT(CloseBracketButton()));
    connect(ui->buttonEquals, SIGNAL(released()), this, SLOT(EqualButton()));
    connect(ui->buttonClear, SIGNAL(released()), this, SLOT(ClearButton()));
    connect(ui->buttonClearAll, SIGNAL(released()), this, SLOT(ClearAllButton()));
    connect(ui->buttonPowerTwo, SIGNAL(released()), this, SLOT(PowerTwoButton()));
    connect(ui->buttonPowerAny, SIGNAL(released()), this, SLOT(PowerAnyButton()));
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::NumPressed() {
    if (errorState) {
        ui->display->setText("");
        ui->displayOperations->setText("");
        errorState = false;
    }

    if (calculatedState) {
        ui->display->setText("");
        ui->displayOperations->setText("");
        calculatedState = false;
    }

    if (resetDisplay) {
        qDebug() << "reseting display";
        ui->display->setText("");
    }

    QPushButton *button = (QPushButton *)sender();
    QString buttonValue = button->text();
    QString displayValue = ui->display->text();
    QString operationsValue = ui->displayOperations->text();
    QString newValue = displayValue + buttonValue;

    double newValueDouble = newValue.toDouble();
    ui->display->setText(QString::number(newValueDouble, 'g', 16));

//    operationsValue.append(buttonValue);
//    ui->displayOperations->setText(operationsValue);

    qDebug() << "num pressed: " << buttonValue;
    qDebug() << "output stack: " << outputQueue;
    qDebug() << "operations stack: " << operationsStack;
}

void Calculator::DecimalButton() {
    if (isDecimal) {
        return;
    }

    qDebug() << "decimal pressed";
    isDecimal = true;
    QString string;

    if (calculatedState) {
        calculatedState = false;
        ui->display->setText("0.");
        ui->displayOperations->setText("");
        return;
    }

    if (ui->display->text() == "") {
        string = "0.";
    } else {
        string = ".";
    }

    QString displayValue = ui->display->text();
//    QString operationsValue = ui->displayOperations->text();

    ui->display->setText(displayValue + string);
//    ui->displayOperations->setText(operationsValue + string);

}

void Calculator::SignButton() {
    QString displayValue = ui->display->text();
    ui->display->setText("-" + displayValue);
}

void Calculator::MathButtons() {    
    if (errorState) {
        ui->display->setText("");
        ui->displayOperations->setText("");
        errorState = false;
    }

    if (calculatedState) {
        calculatedState = false;
    }

    storeNumber();

    if (resetDisplay) {
        ui->display->setText("");
    }    

    QPushButton *button = (QPushButton *)sender();
    QString buttonValue = button->text();
    QString operationsValue = ui->displayOperations->text();

    operationsValue.append(buttonValue);
    ui->displayOperations->setText(operationsValue);
    pushOperation(buttonValue);

    qDebug() << "math pressed: " << buttonValue;
    qDebug() << "output stack: " << outputQueue;
    qDebug() << "operations stack: " << operationsStack;
}

void Calculator::EqualButton() {
    storeNumber();

    while (!operationsStack.empty()) {
        if (QString::compare(operationsStack.top(), "(") == 0) {
            handleError();
            return;
        }
        outputQueue.enqueue(operationsStack.pop());
    }

    qDebug() << "output stack: " << outputQueue;
    qDebug() << "operations stack: " << operationsStack;
    QStack<QString> calculationStack = {};

    while (!outputQueue.empty()) {
        QString value = outputQueue.dequeue();
        QRegularExpression re("^[-+*/^]$");
        QRegularExpressionMatch match = re.match(value);

        if (match.hasMatch()) {
            if (calculationStack.empty()) {
                handleError();
                return;
            }
            double y = calculationStack.pop().toDouble();
            if (calculationStack.empty()) {
                handleError();
                return;
            }
            double x = calculationStack.pop().toDouble();
            double temp;
            qDebug() << "calculating: " << x << value << y;
            if ((QString::compare(value, "^", Qt::CaseInsensitive) == 0)) {
                temp = pow(x, y);
            } else if ((QString::compare(value, "/", Qt::CaseInsensitive) == 0)) {
                temp = x / y;
            } else if ((QString::compare(value, "*", Qt::CaseInsensitive) == 0)) {
                temp = x * y;
            } else if ((QString::compare(value, "+", Qt::CaseInsensitive) == 0)) {
                temp = x + y;
            } else if ((QString::compare(value, "-", Qt::CaseInsensitive) == 0)) {
                temp = x - y;
            } else {
                handleError();
                return;
            }
            calculationStack.push(QString::number(temp));
            qDebug() << "pushing result: " << temp;

        } else {
            calculationStack.push(value);
            qDebug() << "pushing " << value;

        }
        qDebug() << "calculation stack: " << calculationStack;
    }

    // update display
    QString operationsValue = ui->displayOperations->text();
    operationsValue.append(QString("="));
    ui->displayOperations->setText(operationsValue);
    if (calculationStack.empty()) {
        handleError();
        return;
    }
    ui->display->setText(calculationStack.pop());    

    if (calculationStack.empty()) {
        calculatedState = true;
    } else {
        ui->display->setText("ERROR");
        errorState = true;
    }


    qDebug() << "output stack: " << outputQueue;
    qDebug() << "operations stack: " << operationsStack;
    qDebug() << "calculation stack: " << calculationStack;
}

void Calculator::ClearButton() {
    qDebug() << "Clear button pressed";
    isDecimal = false;
    calculatedState = false;

    ui->display->setText("");

}

void Calculator::ClearAllButton() {
    qDebug() << "Clear all button pressed";
    operationsStack.clear();
    outputQueue.clear();
    isDecimal = false;
    calculatedState = false;

    ui->display->setText("");
    ui->displayOperations->setText("");
}

void Calculator::OpenBracketButton() {    
    if (calculatedState) {
        ui->display->setText("");
        ui->displayOperations->setText("");
        calculatedState = false;
    }

    // push onto the stack
    QPushButton *button = (QPushButton *)sender();
    QString buttonValue = button->text();
    operationsStack.push(buttonValue);

    // update display
    QString operationsValue = ui->displayOperations->text();
    operationsValue.append(buttonValue);
    ui->displayOperations->setText(operationsValue);

    qDebug() << "left bracket button";
    qDebug() << "output stack: " << outputQueue;
    qDebug() << "operations stack: " << operationsStack;
}

void Calculator::CloseBracketButton() {
    storeNumber();
    // push onto the stack
    QPushButton *button = (QPushButton *)sender();
    QString buttonValue = button->text();

    // push all operators to output until open bracket found
    while (!operationsStack.empty() && QString::compare(operationsStack.top(), "(") != 0) {
        outputQueue.enqueue(operationsStack.pop());
    }

    if (operationsStack.empty()) {
        handleError();
        return;
    }

    // remove open bracket from the stack
    if (QString::compare(operationsStack.top(), "(") == 0) {
        operationsStack.pop();
    }
    // update display
    QString operationsValue = ui->displayOperations->text();
    operationsValue.append(buttonValue);
    ui->displayOperations->setText(operationsValue);

    qDebug() << "right bracket button";
    qDebug() << "output stack: " << outputQueue;
    qDebug() << "operations stack: " << operationsStack;
}

void Calculator::PowerAnyButton() {
    storeNumber();
    QString op = "^";
    pushOperation(op);

    // update display
    QString operationsValue = ui->displayOperations->text();
    operationsValue.append(op);
    ui->displayOperations->setText(operationsValue);
    calculatedState = false;
    qDebug() << "output stack: " << outputQueue;
    qDebug() << "operations stack: " << operationsStack;
}

void Calculator::PowerTwoButton() {
    storeNumber();
    QString op = "^";
    pushOperation(op);
    QString num = "2";
    outputQueue.enqueue(num);

    // update display
    QString operationsValue = ui->displayOperations->text();
    operationsValue.append(op).append(num);
    ui->displayOperations->setText(operationsValue);
//    ui->display->setText(QString("2"));
    qDebug() << "output stack: " << outputQueue;
    qDebug() << "operations stack: " << operationsStack;
}

void Calculator::pushOperation(QString &operation) {
    while (!operationsStack.empty() &&
       QString::compare(operationsStack.top(), "(") != 0
        && (
               getOperationPrecedence(operationsStack.top()) > getOperationPrecedence(operation)
                ||
               (
                getOperationPrecedence(operationsStack.top()) == getOperationPrecedence(operation) && getOperationAssociativeness(operation) == 1
               )
             )
    ) {
        outputQueue.enqueue(operationsStack.pop());
    }
    operationsStack.push(operation);
}

int Calculator::getOperationPrecedence(QString &operation) {
    if ((QString::compare(operation, "^", Qt::CaseInsensitive) == 0)) {
        return 4;
    }
    if ((QString::compare(operation, "/", Qt::CaseInsensitive) == 0)) {
        return 3;
    }
    if ((QString::compare(operation, "*", Qt::CaseInsensitive) == 0)) {
        return 3;
    }
    if ((QString::compare(operation, "+", Qt::CaseInsensitive) == 0)) {
        return 2;
    }
    if ((QString::compare(operation, "-", Qt::CaseInsensitive) == 0)) {
        return 2;
    }

    return 1;
}

int Calculator::getOperationAssociativeness(QString &operation) {
    if ((QString::compare(operation, "^", Qt::CaseInsensitive) == 0)) {
        return 2;
    }

    return 1;
}

void Calculator::storeNumber() {
    if (errorState) {
        qDebug() << "error state reset";
        errorState = false;
        return;
    }
    QString displayValue = ui->display->text();
    if (displayValue == "") {
        return;
    }
    outputQueue.enqueue(displayValue);
    ui->display->setText("");
    QString operationsValue = ui->displayOperations->text();
    if (displayValue.toDouble() < 0) {
        ui->displayOperations->setText(operationsValue + "(" + displayValue + ")");
    } else {
        ui->displayOperations->setText(operationsValue + displayValue);
    }

    isDecimal = false;
}

void Calculator::handleError() {
    ui->display->setText("ERROR");
    errorState = true;
    resetDisplay = true;
    operationsStack.clear();
    outputQueue.clear();
}
