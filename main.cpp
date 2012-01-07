#include <QCoreApplication>
#include "StickListener.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    StickListener stick;
	
    return a.exec();
}
