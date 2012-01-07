#ifndef STICKLISTENER_H_
#define STICKLISTENER_H_
#include <QtCore>
#include <QtNetwork>
#include <iostream>
#include <termios.h>
#include <fcntl.h>
//#include <linux/joystick.h>
#define NAME_LENGTH 128

//#define JOYSTICK_DEVICE "/dev/input/js0" // sur hp6910, js0 est l'accelerometre !!
#define JOYSTICK_DEVICE "/dev/uhid0" // sur hp6910, js0 est l'accelerometre !!

#define LOGITECHDUALACTION "Logitech Dual Action" 	// 6 axes, 12 boutons
#define LOGITECHATK3  "Logitech Attack 3"  			// 3 axes, 11 boutons
#define LOGITECHEXT3D "Logitech Extreme 3D"			// 6 axes, 12 boutons

class StickListener  : public QObject
{
	Q_OBJECT
private:
    int fd;
    unsigned char axes;
    unsigned char nombreboutons;
    int version;
    char name[NAME_LENGTH];
	QSocketNotifier *socketNotifier;

public:
	StickListener();
	virtual ~StickListener();

private slots:
	void readStick(int fdesc);
};

#endif /* STICKLISTENER_H_ */
