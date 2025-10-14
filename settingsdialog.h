#pragma once

#include "QDialog"
#include "QSerialPort"

QT_BEGIN_NAMESPACE
namespace Ui {
    class SettingsDialog;
}

class QIntValidator;
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;

        bool isTimestampEnabled;

        auto operator==(Settings const&) const -> bool = default;
    };

    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog() override;

    [[nodiscard]] auto settings() const -> Settings;

protected:
    void showEvent(QShowEvent* event) override;

signals:
    void settingsChanged();

private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

private:
    void onShown();
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();
    void logSettings() const;

private:
    Ui::SettingsDialog* mUi = nullptr;
    Settings mCurrentSettings{};
    QIntValidator* mIntValidator = nullptr;
};
