#include "studentwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

StudentWidget::StudentWidget(const QString &studentName, QWidget *parent)
    : QWidget(parent), student(studentName)
{
    auto *lay = new QVBoxLayout(this);
    lay->addWidget(new QLabel(QString("Student: <b>%1</b>").arg(student)));

    listWidget = new QListWidget();
    lay->addWidget(new QLabel("Available assignments:"));
    lay->addWidget(listWidget);

    auto *h = new QHBoxLayout();
    countdownLabel = new QLabel("Select assignment to view remaining time");
    submitBtn = new QPushButton("Submit PDF");
    h->addWidget(countdownLabel);
    h->addWidget(submitBtn);
    lay->addLayout(h);

    timer = new QTimer(this);
    timer->start(1000);
    connect(timer, &QTimer::timeout, this, &StudentWidget::updateCountdown);
    connect(submitBtn, &QPushButton::clicked, this, &StudentWidget::onSubmit);
    connect(listWidget, &QListWidget::currentRowChanged, this, &StudentWidget::onSelectionChanged);
}

void StudentWidget::setAssignments(const QVector<Assignment> &list) {
    assignments = list;
    listWidget->clear();
    for (const Assignment &a : assignments) {
        QString status = a.submittedBy.isEmpty() ? "Open" : ("Submitted by " + a.submittedBy);
        QString line = QString("[%1] %2 | Due: %3 | %4")
                           .arg(a.id).arg(a.title).arg(a.deadline.toString("yyyy-MM-dd HH:mm")).arg(status);
        listWidget->addItem(line);
    }
    updateCountdown();
}

void StudentWidget::onSubmit() {
    int idx = listWidget->currentRow();
    if (idx < 0 || idx >= assignments.size()) {
        QMessageBox::warning(this, "Select", "Choose an assignment first");
        return;
    }
    const Assignment &a = assignments[idx];
    if (!a.submittedBy.isEmpty()) {
        if (QMessageBox::question(this, "Already submitted", "This assignment already has a submission. Resubmit?") != QMessageBox::Yes)
            return;
    }

    QString file = QFileDialog::getOpenFileName(this, "Select PDF to submit", QString(), "PDF Files (*.pdf)");
    if (file.isEmpty()) return;

    emit submitRequested(a.id, file);
}

void StudentWidget::onSelectionChanged() {
    updateCountdown();
}

void StudentWidget::updateCountdown() {
    int idx = listWidget->currentRow();
    if (idx < 0 || idx >= assignments.size()) {
        countdownLabel->setText("Select assignment to view remaining time");
        return;
    }
    QDateTime now = QDateTime::currentDateTime();
    QDateTime ddl = assignments[idx].deadline;
    qint64 secs = now.secsTo(ddl);
    if (secs > 0) {
        qint64 days = secs / 86400;
        qint64 hours = (secs % 86400) / 3600;
        qint64 minutes = (secs % 3600) / 60;
        qint64 seconds = secs % 60;
        countdownLabel->setText(QString("Remaining: %1d %2h %3m %4s").arg(days).arg(hours).arg(minutes).arg(seconds));
    } else {
        countdownLabel->setText("Deadline passed");
    }
}
