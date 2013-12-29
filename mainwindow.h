#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <Windows.h>
#include <WbemIdl.h>
#pragma comment(lib,"wbemuuid.lib")
#pragma comment(lib,"ole32.lib")
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public Q_SLOTS:
    void myshowshowpropertyvalue();
    void myshowpropertysname();
private:
    Ui::MainWindow *ui;
    IWbemLocator* m_pLocator;
    IWbemServices* m_pService;
    QString m_classname;
};

#endif // MAINWINDOW_H
