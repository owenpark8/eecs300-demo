#pragma once

#include <QMainWindow>

#include "console.h"
#include "settingsdialog.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow() override;

public slots:
    void setCounter(std::size_t value);
    void resetCounter();

private slots:
    void settingsApplied();
    void openSerialPort();
    void closeSerialPort();
    void readData();

private:
    void processLine(QByteArray const& line);

private:
    Console* mConsole;
    SettingsDialog* mSettings;
    QSerialPort* mSerial = nullptr;
    QByteArray mRxBuf;
    QLabel* mCounterLabel;
    QLabel* mDeltaLabel;
    std::size_t mCounterValue;
};
