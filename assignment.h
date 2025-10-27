#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

struct Assignment {
    int id = 0;
    QString title;
    QString description;
    QDateTime deadline;
    QString createdBy;      // teacher name
    QString submittedBy;    // student name
    QString submissionFile; // path to copied pdf
    QDateTime submittedOn;
    int grade = -1;

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"] = id;
        o["title"] = title;
        o["description"] = description;
        o["deadline"] = deadline.toString(Qt::ISODate);
        o["createdBy"] = createdBy;
        o["submittedBy"] = submittedBy;
        o["submissionFile"] = submissionFile;
        o["submittedOn"] = submittedOn.toString(Qt::ISODate);
        o["grade"] = grade;
        return o;
    }

    static Assignment fromJson(const QJsonObject &o) {
        Assignment a;
        a.id = o["id"].toInt();
        a.title = o["title"].toString();
        a.description = o["description"].toString();
        a.deadline = QDateTime::fromString(o["deadline"].toString(), Qt::ISODate);
        a.createdBy = o["createdBy"].toString();
        a.submittedBy = o["submittedBy"].toString();
        a.submissionFile = o["submissionFile"].toString();
        a.submittedOn = QDateTime::fromString(o["submittedOn"].toString(), Qt::ISODate);
        a.grade = o["grade"].toInt(-1);
        return a;
    }
};

#endif // ASSIGNMENT_H
