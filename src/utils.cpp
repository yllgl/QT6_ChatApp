#include "utils.h"
#include <cstdint>
#include <QMainWindow>
#include <QWidget>
// 将指针转换为 QString
QString pointerToString(void* ptr) {
    uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
    return QString::number(address);
}

// 将 QString 转换回指针
void* stringToPointer(const QString& str) {
    bool ok;
    uintptr_t address = str.toULongLong(&ok, 10);  // 使用16进制解析
    if (ok) {
        return reinterpret_cast<void*>(address);
    }
    return nullptr;  // 如果转换失败，返回 nullptr
}
QMainWindow* findMainWindow(QWidget* widget){
    while (widget) {
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(widget);
        if (mainWindow) {
            return mainWindow;
        }
        widget = widget->parentWidget();
    }
    return nullptr;
}