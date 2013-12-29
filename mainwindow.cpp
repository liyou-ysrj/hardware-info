#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
wchar_t* WMI_CLASSNAME[]={
__TEXT("Win32_Processor"), // CPU 处理器
__TEXT("Win32_PhysicalMemory"), // 物理内存条
__TEXT("Win32_Keyboard"), // 键盘
__TEXT("Win32_PointingDevice"), // 点输入设备，包括鼠标。
__TEXT("Win32_FloppyDrive"), // 软盘驱动器
__TEXT("Win32_DiskDrive"), // 硬盘驱动器
__TEXT("Win32_CDROMDrive"), // 光盘驱动器
__TEXT("Win32_BaseBoard"), // 主板
__TEXT("Win32_BIOS"), // BIOS 芯片
__TEXT("Win32_ParallelPort"), // 并口
__TEXT("Win32_SerialPort"), // 串口
__TEXT("Win32_SerialPortConfiguration"), // 串口配置
__TEXT("Win32_SoundDevice"), // 多媒体设置，一般指声卡。
__TEXT("Win32_SystemSlot"), // 主板插槽 (ISA & PCI & AGP)
__TEXT("Win32_USBController"), // USB 控制器
__TEXT("Win32_NetworkAdapter"), // 网络适配器
__TEXT("Win32_NetworkAdapterConfiguration"), // 网络适配器设置
__TEXT("Win32_Printer"), // 打印机
__TEXT("Win32_PrinterConfiguration"), // 打印机设置
__TEXT("Win32_PrintJob"), // 打印机任务
__TEXT("Win32_TCPIPPrinterPort"), // 打印机端口
//__TEXT("Win32_POTSModem"), // MODEM
//__TEXT("Win32_POTSModemToSerialPort"), // MODEM 端口
__TEXT("Win32_DesktopMonitor"), // 显示器
__TEXT("Win32_DisplayConfiguration"), // 显卡
__TEXT("Win32_DisplayControllerConfiguration"), // 显卡设置
__TEXT("Win32_VideoController"), // 显卡细节。
__TEXT("Win32_VideoSettings"), // 显卡支持的显示模式。

// 操作系统
__TEXT("Win32_TimeZone"), // 时区
__TEXT("Win32_SystemDriver"), // 驱动程序
__TEXT("Win32_DiskPartition"), // 磁盘分区
__TEXT("Win32_LogicalDisk"), // 逻辑磁盘
__TEXT("Win32_LogicalDiskToPartition"), // 逻辑磁盘所在分区及始末位置。
__TEXT("Win32_LogicalMemoryConfiguration"), // 逻辑内存配置
__TEXT("Win32_PageFile"), // 系统页文件信息
__TEXT("Win32_PageFileSetting"), // 页文件设置
__TEXT("Win32_BootConfiguration"), // 系统启动配置
__TEXT("Win32_ComputerSystem"), // 计算机信息简要
__TEXT("Win32_OperatingSystem"), // 操作系统信息
__TEXT("Win32_StartupCommand"), // 系统自动启动程序
__TEXT("Win32_Service"), // 系统安装的服务
__TEXT("Win32_Group"), // 系统管理组
__TEXT("Win32_GroupUser"), // 系统组帐号
__TEXT("Win32_UserAccount"), // 用户帐号
__TEXT("Win32_Process"), // 系统进程
__TEXT("Win32_Thread"), // 系统线程
__TEXT("Win32_Share"), // 共享
__TEXT("Win32_NetworkClient"), // 已安装的网络客户端
__TEXT("Win32_NetworkProtocol")// 已安装的网络协议
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->listWidget,SIGNAL(itemSelectionChanged()),this,SLOT(myshowshowpropertyvalue()));
    connect(ui->listWidget_2,SIGNAL(itemSelectionChanged()),this,SLOT(myshowpropertysname()));
    HRESULT hr;
    hr=CoInitializeEx(NULL,COINIT_MULTITHREADED);
    if(SUCCEEDED(hr)||RPC_E_CHANGED_MODE==hr)
    {
        hr=CoInitializeSecurity(NULL,\
                                -1,\
                                NULL,\
                                NULL,\
                                RPC_C_AUTHN_LEVEL_PKT,\
                                RPC_C_IMP_LEVEL_IMPERSONATE,\
                                NULL,\
                                EOAC_NONE,\
                                NULL);
        hr=CoCreateInstance(CLSID_WbemLocator,\
                            0,\
                            CLSCTX_INPROC_SERVER,\
                            IID_IWbemLocator,\
                            (LPVOID*)&m_pLocator);

        hr=m_pLocator->ConnectServer(__TEXT("ROOT\\CIMV2"),\
                                     NULL,\
                                     NULL,\
                                     0,\
                                     NULL,\
                                     0,\
                                     0,\
                                     &m_pService);
        hr=CoSetProxyBlanket(m_pService,\
                             RPC_C_AUTHN_WINNT,\
                             RPC_C_AUTHZ_NONE,\
                             NULL,\
                             RPC_C_AUTHN_LEVEL_CALL,\
                             RPC_C_IMP_LEVEL_IMPERSONATE,\
                             NULL,\
                             EOAC_NONE);

    }

    for(int n=0;n<sizeof(WMI_CLASSNAME)/sizeof(wchar_t*);n++)
    {
        wchar_t** classname=WMI_CLASSNAME;
        ui->listWidget_2->addItem(new QListWidgetItem(QString().fromWCharArray(classname[n])));
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::myshowshowpropertyvalue()
{

    IEnumWbemClassObject* m_pEnumClassObject;
    QListWidgetItem* item=ui->listWidget->currentItem();
    HRESULT hr;
    hr=m_pService->ExecQuery(__TEXT("WQL"),\
                          (wchar_t*)m_classname.toStdWString().c_str(),\
                          WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,\
                          NULL,\
                          &m_pEnumClassObject);
   if(SUCCEEDED(hr))
   {
       IWbemClassObject* m_pClassObject;
       ULONG uret;
       hr=m_pEnumClassObject->Next(WBEM_INFINITE,1,&m_pClassObject,&uret);
       if(uret==0)
       {
           QMessageBox(QMessageBox::Critical,QString("错误"),QString("选定的类没有设备"),QMessageBox::Ok,this);
           return;
       }
       if(SUCCEEDED(hr))
       {
          VARIANT vt_pro;

           hr=m_pClassObject->Get(item->text().toStdWString().c_str(),NULL,&vt_pro,NULL,NULL);
           switch(vt_pro.vt)
           {
           case VT_I8:
               ui->textEdit->setText(QString().number(vt_pro.llVal));
               break;
           case VT_I4:
               ui->textEdit->setText(QString().number(vt_pro.uintVal));
               break;
           case VT_BSTR:
               ui->textEdit->setText(QString().fromUtf16((const ushort*)vt_pro.bstrVal));
               break;
           default:
               break;
           }
       }
       m_pClassObject->Release();
   }
   m_pEnumClassObject->Release();
}
void MainWindow::myshowpropertysname()
{
    ui->listWidget->clear();
    m_classname=ui->listWidget_2->currentItem()->text();
    m_classname.insert(0,("SELECT * FROM "));
    IEnumWbemClassObject* m_pEnumClassObject;
    HRESULT hr;
    hr=m_pService->ExecQuery(__TEXT("WQL"),\
                          (const BSTR)m_classname.toStdWString().c_str(),\
                          WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,\
                          NULL,\
                          &m_pEnumClassObject);
   if(SUCCEEDED(hr))
   {
       IWbemClassObject* m_pClassObject;
       ULONG uret;
       hr=m_pEnumClassObject->Next(WBEM_INFINITE,1,&m_pClassObject,&uret);
       if(uret==0)
       {
           QMessageBox::critical(this,QString("error"),QString("no device under selected class"),QMessageBox::Ok);
           return;
       }
       if(SUCCEEDED(hr))
       {
           SAFEARRAY* myname;
           hr=m_pClassObject->GetNames(NULL,NULL,NULL,&myname);
           for(UINT n=myname->rgsabound->lLbound;n<myname->rgsabound->cElements;n++)
           {
               ui->listWidget->addItem(new QListWidgetItem(QString().fromUtf16(reinterpret_cast<const ushort*>(((wchar_t**)myname->pvData)[n]))));

           }
       }
       m_pClassObject->Release();
   }
   m_pEnumClassObject->Release();
}
