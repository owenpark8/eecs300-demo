#pragma once

#include "QPlainTextEdit"

class Console : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit Console(QWidget* parent = nullptr);

    [[nodiscard]] auto sizeHint() const -> QSize override { return {600, 200}; }
    void setTimestampEnabled(bool enabled) { mIsTimestampEnabled = enabled; }
    [[nodiscard]] auto isTimestampEnabled() const -> bool { return mIsTimestampEnabled; }

public slots:
    void printData(QByteArray const& data);
    void printLine(QString const& line);

private:
    bool mIsTimestampEnabled = false;
    bool mFirstLine = true;
};
