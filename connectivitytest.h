#ifndef CONNECTIVITYTEST_H
#define CONNECTIVITYTEST_H

#include <QMainWindow>
#include "ui_connectivitytest.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ConnectivityTest; }
QT_END_NAMESPACE

class ConnectivityTest : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConnectivityTest(QWidget *parent = nullptr);
    ~ConnectivityTest();

private slots:
    void on_startTest_clicked();
    void on_noLogin_stateChanged(int state);
    void on_outputToFile_stateChanged(int state);

private:
    Ui::ConnectivityTest *ui;
    QString m_lastReceivedLine;
    void writeOutputToFile(const QString &text);
    void appendOutput(const QString &text);
};

#endif // CONNECTIVITYTEST_H
