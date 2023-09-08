#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QMainWindow>
#include <QWidget>
QString pointerToString(void* ptr);
void* stringToPointer(const QString& str);
QMainWindow* findMainWindow(QWidget* widget);
#endif // UTILS_H