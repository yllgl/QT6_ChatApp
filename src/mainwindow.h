#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget>
#include <QStatusBar>
#include <QLabel>
#include <QVBoxLayout>
#include "View.h"
class AbstractChatItem;
class MainWindow : public QMainWindow,public View {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr,QString username = "");
    void initUI(QString username="");
    void addChatItem(AbstractChatItem &item);
    void removeChatItem(const int index);
    void resizeEvent(QResizeEvent *event);
    void updateUI(const QJsonObject &data) override;
private:
    QLabel *titleLabel;
    QLineEdit *searchBox;
    QListWidget *listWidget;
    QStatusBar *statusBar;
    QVBoxLayout *mainLayout;
    QIcon createMenuIcon();
    void add_user();
    void add_group();
signals:
    void resized();
};

#endif // MAINWINDOW_H
