// Include the necessary Qt headers
#include <QMessageBox>
#include <QProcess>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include "connectivitytest.h"
#include "ui_connectivitytest.h"

ConnectivityTest::ConnectivityTest(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ConnectivityTest)
{
    ui->setupUi(this);

    ui->passwordInput->setEchoMode(QLineEdit::Password);

    // Connect the signals to the slots
    connect(ui->startTest, &QPushButton::clicked, this, &ConnectivityTest::on_startTest_clicked);
    connect(ui->noLogin, &QCheckBox::stateChanged, this, &ConnectivityTest::on_noLogin_stateChanged);
    connect(ui->outputToFile, &QCheckBox::stateChanged, this, &ConnectivityTest::on_outputToFile_stateChanged);
}

void ConnectivityTest::writeOutputToFile(const QString &text)
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString outputPath = desktopPath + "/ConnectivityTestOutput.txt";
    QFile outputFile(outputPath);

    if (outputFile.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&outputFile);
        out << text << "\n";
        outputFile.close();
    }
    else
    {
        QMessageBox::warning(this, "Error", "Unable to open output file for writing.");
    }
}

void ConnectivityTest::appendOutput(const QString &text)
{
    QTextCursor cursor = ui->outputTextEdit->textCursor();
    QTextCharFormat originalFormat = cursor.charFormat();
    QStringList lines = text.split("\n");

    for (const QString &line : lines) {
        if (line.isEmpty() || line == m_lastReceivedLine) {
            continue;
        }
        m_lastReceivedLine = line;
        if (line.contains("SUCCESS")) {
            int index = line.indexOf("SUCCESS");
            cursor.insertText(line.left(index));
            QTextCharFormat successFormat;
            successFormat.setForeground(QColor(Qt::green));
            successFormat.setFontWeight(QFont::Bold);
            cursor.setCharFormat(successFormat);
            cursor.insertText("SUCCESS");
            cursor.setCharFormat(originalFormat);
            cursor.insertText(line.mid(index + 7));
        }
        else if (line.contains("FAILED")) {
            int index = line.indexOf("FAILED");
            cursor.insertText(line.left(index));
            QTextCharFormat failedFormat;
            failedFormat.setForeground(QColor(Qt::red));
            failedFormat.setFontWeight(QFont::Bold);
            cursor.setCharFormat(failedFormat);
            cursor.insertText("FAILED");
            cursor.setCharFormat(originalFormat);
            cursor.insertText(line.mid(index + 6));
        }
        else {
            cursor.insertText(line);
        }
    }

    ui->outputTextEdit->setTextCursor(cursor);
}

void ConnectivityTest::on_startTest_clicked()
{
    // Disable the Test button
    ui->startTest->setEnabled(false);

    QString username = ui->loginInput->text();
    QString password = ui->passwordInput->text();

    if (username.isEmpty() && !ui->noLogin->isChecked())
    {
        QMessageBox::warning(this, "Error", "Please enter a valid username.");
        ui->startTest->setEnabled(true); // Re-enable the Test button before returning
        return;
    }
    if (password.isEmpty() && !ui->noLogin->isChecked())
    {
        QMessageBox::warning(this, "Error", "Please enter a valid password.");
        ui->startTest->setEnabled(true); // Re-enable the Test button before returning
        return;
    }

    QProcess *process = new QProcess(this);
    QString program = "account_control_test.exe";
    QStringList arguments;
    if (ui->noLogin->isChecked())
    {
        arguments << "/connectivity_test" << "/no_login";
    }
    else
    {
        arguments << "/connectivity_test" << QString("/account:%1").arg(username) << QString("/password:%1").arg(password);
    }
    connect(process, &QProcess::readyReadStandardOutput, [this, process]() {
        QString output = process->readAllStandardOutput();
        appendOutput(output);
        if (ui->outputToFile->isChecked()) {
            writeOutputToFile(output);
        }
    });
    connect(process, &QProcess::readyReadStandardError, [this, process]() {
        QString output = process->readAllStandardError();
        appendOutput(output);
        if (ui->outputToFile->isChecked()) {
            writeOutputToFile(output);
        }
    });
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this](int exitCode, QProcess::ExitStatus exitStatus) {
        // Handle the process finished signal

        // Re-enable the Test button
        ui->startTest->setEnabled(true);
    });
    process->start(program, arguments);
}

void ConnectivityTest::on_noLogin_stateChanged(int state)
{
    bool enabled = (state != Qt::Checked);
    ui->loginInput->setEnabled(enabled);
    ui->passwordInput->setEnabled(enabled);
}

void ConnectivityTest::on_outputToFile_stateChanged(int state)
{
    if (state == Qt::Checked)
    {
        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString header = QString("\n\n===== Connectivity Test Output - %1 =====\n").arg(currentTime);
        writeOutputToFile(header);
    }
}

ConnectivityTest::~ConnectivityTest()
{
    delete ui;
}
