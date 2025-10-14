#include "console.h"

#include "QDateTime"
#include "QScrollBar"

Console::Console(QWidget* parent) : QPlainTextEdit(parent) {
    setReadOnly(true);
}

void Console::printData(QByteArray const& data) {
    printTimestampIfEnabled();
    insertPlainText(data);

    QScrollBar* bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::printLine(QString const& line) {
    printTimestampIfEnabled();
    insertPlainText(line + '\n');

    QScrollBar* bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::printTimestampIfEnabled() {
    if (mIsTimestampEnabled) {
        auto const timestamp = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss.zzz] ");
        insertPlainText(timestamp);
    }
}
