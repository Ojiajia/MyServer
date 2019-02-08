#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>
#include <myserver.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    int portNumber;
    if (argc != 2) {
        //printf(argc);
        printf("Error: found %d arguments. Needs exactly 1", argc-1);
        exit(1);
    }
    portNumber = atoi(argv[1]);

    MyServer     server(portNumber, 0);

    server.show();

    return app.exec();

}
