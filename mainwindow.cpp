#include "mainwindow.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QLabel>
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow() {
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    mSettings = new SettingsDialog(this);
    connect(mSettings, &SettingsDialog::settingsChanged, this, &MainWindow::openSerialPort, Qt::QueuedConnection);

    auto* consoleDock = new QDockWidget(tr("Log"), this);
    consoleDock->setFeatures(QDockWidget::DockWidgetClosable |
                             QDockWidget::DockWidgetMovable |
                             QDockWidget::DockWidgetFloatable);
    mConsole = new Console(consoleDock);
    consoleDock->setWidget(mConsole);
    addDockWidget(Qt::BottomDockWidgetArea, consoleDock);

    mSerial = new QSerialPort(this);
    QObject::connect(mSerial, &QSerialPort::readyRead, [&]() {
        while (mSerial->canReadLine()) {
            QByteArray line = mSerial->readLine();
            processLine(line);
        }
    });

    mCounterLabel = new QLabel("0");
    mCounterValue = 0;
    QFont font = mCounterLabel->font();
    font.setPointSize(48);
    mCounterLabel->setFont(font);
    mCounterLabel->setAlignment(Qt::AlignCenter);

    mDeltaLabel = new QLabel("+0");

    auto* subLayout = new QHBoxLayout;
    subLayout->addWidget(mCounterLabel, 0, Qt::AlignRight);
    subLayout->addWidget(mDeltaLabel, 0, Qt::AlignLeft | Qt::AlignTop);

    auto* layout = new QVBoxLayout;
    layout->addStretch();
    layout->addLayout(subLayout);
    layout->addStretch();

    auto* central = new QWidget;
    setCentralWidget(central);
    central->setLayout(layout);

    QToolBar* fileToolbar = addToolBar(tr("Actions"));

    auto const settingsIcon = QIcon::fromTheme("document-properties", QIcon("./images/settings.png"));
    auto* settingsAction = new QAction(settingsIcon, tr("&Settings"), this);
    settingsAction->setStatusTip(tr("Open settings"));
    fileToolbar->addAction(settingsAction);
    connect(settingsAction, &QAction::triggered, mSettings, &SettingsDialog::show);

    QIcon const showIcon = QIcon("./images/show-console.png");
    QIcon const hideIcon = QIcon("./images/hide-console.png");
    auto* consoleDockToggleViewAct = consoleDock->toggleViewAction();
    consoleDockToggleViewAct->setIcon(hideIcon);
    connect(consoleDock->toggleViewAction(), &QAction::toggled, this, [=](bool checked) {
        consoleDockToggleViewAct->setIcon(checked ? hideIcon : showIcon);
        consoleDockToggleViewAct->setText(checked ? tr("Hide Console") : tr("Show Console"));
    });
    fileToolbar->addAction(consoleDock->toggleViewAction());

    auto const clearIcon = QIcon("./images/clear.svg");
    auto* clearAct = new QAction(clearIcon, tr("&Clear"), this);
    clearAct->setShortcuts(QKeySequence::New);
    clearAct->setStatusTip(tr("Clear counter"));
    fileToolbar->addAction(clearAct);
    connect(clearAct, &QAction::triggered, this, [this]() { resetCounter(); });
}

MainWindow::~MainWindow() {
    delete mSettings;
    delete mCounterLabel;
    delete mSerial;
}

void MainWindow::setCounter(std::size_t value) {
    if (value == mCounterValue) {
        return;
    }

    qsizetype const diff = static_cast<qsizetype>(value) - static_cast<qsizetype>(mCounterValue);
    QString const text = QString("%1%2").arg((diff >= 0 ? "+" : "")).arg(diff);
    mDeltaLabel->setText(text);

    mCounterValue = value;
    mCounterLabel->setText(QString::number(mCounterValue));
}

void MainWindow::resetCounter() {
    mCounterValue = 0;
    mCounterLabel->setText("0");
    mDeltaLabel->setText("+0");
}

void MainWindow::openSerialPort() {
    if (mSerial->isOpen()) {
        mSerial->close();
    }

    SettingsDialog::Settings const p = mSettings->settings();
    mConsole->setTimestampEnabled(p.isTimestampEnabled);
    mSerial->setPortName(p.name);
    mSerial->setBaudRate(p.baudRate);
    mSerial->setDataBits(p.dataBits);
    mSerial->setParity(p.parity);
    mSerial->setStopBits(p.stopBits);
    mSerial->setFlowControl(p.flowControl);
    if (mSerial->open(QIODevice::ReadWrite)) {
        mConsole->printLine(tr("Connected to %1").arg(p.name));
    } else {
        QMessageBox::critical(this, tr("Error"), mSerial->errorString());
        mConsole->printLine(tr("Error opening %1, error: %2").arg(p.name).arg(mSerial->errorString()));
    }
}

void MainWindow::closeSerialPort() {
    if (mSerial->isOpen()) {
        mSerial->close();
        mConsole->printLine(tr("Disconnected"));
    }
}

void MainWindow::processLine(QByteArray const& line) {
    mConsole->printData(line);

    bool ok = false;
    std::size_t value = line.trimmed().toUInt(&ok);
    if (ok) {
        setCounter(value);
    } else {
        mConsole->printLine("Invalid data received");
    }
}
