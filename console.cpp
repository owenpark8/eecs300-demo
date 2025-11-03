#include "console.h"

#include "QDateTime"
#include "QScrollBar"

Console::Console(QWidget* parent) : QPlainTextEdit(parent) {
    setReadOnly(true);
}

void Console::printData(QByteArray const& data) {
    QScrollBar* bar = verticalScrollBar();
    bool isAtMaxScroll = (bar->value() == bar->maximum());

    // Print a new line before every line except the first one
    // (avoids having two trailing new lines since QPlainTextEdit already adds one)
    if (!mFirstLine) {
        insertPlainText("\n");
    } else {
        mFirstLine = false;
    }

    if (mIsTimestampEnabled) {
        auto const timestamp = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss.zzz] ");
        insertPlainText(timestamp);
    }

    insertPlainText(data.trimmed());

    if (isAtMaxScroll) {
        bar->setValue(bar->maximum());
    }
}

void Console::printLine(QString const& line) {
    printData(line.toUtf8());
}
