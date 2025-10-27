#ifndef STUDENTWIDGET_H
#define STUDENTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include "assignment.h"

class StudentWidget : public QWidget {
    Q_OBJECT
public:
    explicit StudentWidget(const QString &studentName, QWidget *parent = nullptr);
    void setAssignments(const QVector<Assignment> &list);

signals:
    void submitRequested(int assignmentId, const QString &filePath);

private slots:
    void onSubmit();
    void onSelectionChanged();
    void updateCountdown();

private:
    QString student;
    QVector<Assignment> assignments;
    QListWidget *listWidget;
    QLabel *countdownLabel;
    QPushButton *submitBtn;
    QTimer *timer;
};

#endif // STUDENTWIDGET_H
