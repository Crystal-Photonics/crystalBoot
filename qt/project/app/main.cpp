#include "mainwindow.h"
#include <QApplication>

#include <thread>
#include <iostream>

void call_from_thread() {
    //while(1){
    std::cout << "Hello, World" << std::endl;
//}
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    std::thread t1(call_from_thread);
    w.show();
    int result = a.exec();
    t1.join();
    return result;

}
