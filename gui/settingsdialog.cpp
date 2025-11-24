#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QDebug>
#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPortInfo>

static char const BLANK_STRING[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent), mUi(new Ui::SettingsDialog), mIntValidator(new QIntValidator(0, 4000000, this)) {
    mUi->setupUi(this);
    // Prevents the enter key from hitting the apply button (people will hit enter after typing a custom parameter)
    mUi->applyButton->setAutoDefault(false);
    mUi->applyButton->setDefault(false);

    mUi->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(mUi->applyButton, &QPushButton::clicked,
            this, &SettingsDialog::apply);
    connect(mUi->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::showPortInfo);
    connect(mUi->baudRateBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomBaudRatePolicy);
    connect(mUi->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomDevicePathPolicy);

    fillPortsParameters();
    fillPortsInfo();
}

SettingsDialog::~SettingsDialog() {
    delete mUi;
}

[[nodiscard]] auto SettingsDialog::settings() const -> Settings { return mCurrentSettings; }

void SettingsDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    onShown();
}

void SettingsDialog::showPortInfo(int idx) {
    if (idx == -1)
        return;

    QString const blankString = tr(::BLANK_STRING);

    QStringList const list = mUi->serialPortInfoListBox->itemData(idx).toStringList();
    mUi->descriptionLabel->setText(tr("Description: %1").arg(list.value(1, blankString)));
    mUi->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.value(2, blankString)));
    mUi->serialNumberLabel->setText(tr("Serial number: %1").arg(list.value(3, blankString)));
    mUi->locationLabel->setText(tr("Location: %1").arg(list.value(4, blankString)));
    mUi->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.value(5, blankString)));
    mUi->pidLabel->setText(tr("Product Identifier: %1").arg(list.value(6, blankString)));
}

void SettingsDialog::apply() {
    emit applyClicked();
    updateSettings();
    hide();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx) {
    bool const isCustomBaudRate = !mUi->baudRateBox->itemData(idx).isValid();
    mUi->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        mUi->baudRateBox->clearEditText();
        QLineEdit* edit = mUi->baudRateBox->lineEdit();
        edit->setValidator(mIntValidator);
    }
}

void SettingsDialog::checkCustomDevicePathPolicy(int idx) {
    bool const isCustomPath = !mUi->serialPortInfoListBox->itemData(idx).isValid();
    mUi->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath) {
        mUi->serialPortInfoListBox->clearEditText();
    }
}

void SettingsDialog::onShown() {
    fillPortsInfo();
}

void SettingsDialog::fillPortsParameters() {
    mUi->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    mUi->baudRateBox->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    mUi->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    mUi->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    mUi->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    mUi->baudRateBox->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    mUi->baudRateBox->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    mUi->baudRateBox->addItem(QStringLiteral("1200"), QSerialPort::Baud1200);
    mUi->baudRateBox->addItem(tr("Custom"));
}

void SettingsDialog::fillPortsInfo() {
    mUi->serialPortInfoListBox->clear();
    QString const blankString = tr(::BLANK_STRING);
    auto const infos = QSerialPortInfo::availablePorts();

    qDebug() << infos.count() << "serial ports found";

    for (QSerialPortInfo const& info: infos) {
        QStringList list;
        QString const description = info.description();
        QString const manufacturer = info.manufacturer();
        QString const serialNumber = info.serialNumber();
        auto const vendorId = info.vendorIdentifier();
        auto const productId = info.productIdentifier();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (vendorId ? QString::number(vendorId, 16) : blankString)
             << (productId ? QString::number(productId, 16) : blankString);

        mUi->serialPortInfoListBox->addItem(list.constFirst(), list);
    }

    mUi->serialPortInfoListBox->addItem(tr("Custom"));
}

void SettingsDialog::updateSettings() {
    Settings const old = mCurrentSettings;

    mCurrentSettings.name = mUi->serialPortInfoListBox->currentText();

    if (mUi->baudRateBox->currentIndex() == 4) {
        mCurrentSettings.baudRate = mUi->baudRateBox->currentText().toInt();
    } else {
        auto const baudRateData = mUi->baudRateBox->currentData();
        mCurrentSettings.baudRate = baudRateData.value<QSerialPort::BaudRate>();
    }
    mCurrentSettings.stringBaudRate = QString::number(mCurrentSettings.baudRate);

    mCurrentSettings.isTimestampEnabled = mUi->timestampCheckBox->isChecked();


    mSettingsChangedOnLastApply = old != mCurrentSettings;
    logSettings();
}

void SettingsDialog::logSettings() const {
    qDebug().noquote() << "=== Serial Port Settings ===";
    qDebug().noquote() << "Port Name:       " << mCurrentSettings.name;
    qDebug().noquote() << "Baud Rate:       " << mCurrentSettings.stringBaudRate;
    qDebug().noquote() << "============================";
}
