#include <sys/types.h>
//#include <sys/input.h>
#include <sys/ioctl.h>
#include <termios.h>
//#include <error.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <QtCore/QtCore>
#include "StickListener.h"



int bouton_appuiye = 0;


StickListener::StickListener()
{
	//fd=open(JOYSTICK_DEVICE, O_RDONLY|O_NOCTTY|O_NDELAY);
	fd=open(JOYSTICK_DEVICE, O_RDONLY|O_NOCTTY);
        if( fd < 0 )
        {
       	 perror("Error on fd ");
            return;
        }
         // flushing is to be done after opening. This prevents first read and write to be spam'ish.
         tcflush(fd, TCIOFLUSH);

         // http://doc.qt.nokia.com/latest/qsocketnotifier.html#details
         socketNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
         connect(socketNotifier, SIGNAL(activated(int)), this, SLOT(readStick(int)));

      //  Sizeof struct js_event is 64
      //  std::cout << "Stick Listener Started\n";
      //  std::cout << "Sizeof JS Struct = " << sizeof(struct js_event) << "\n";

     	//ioctl(fd, JSIOCGVERSION, &version);
        ioctl(fd, _IOR('j', 0x01, int), &version);
     	ioctl(fd, _IOR('j', 0x11, short), &axes);
     	ioctl(fd, _IOR('j', 0x12, short), &nombreboutons);
     	//ioctl(fd, _IOC(_IOC_READ, 'j', 0x13, NAME_LENGTH), name);
     	strcpy(name,"TODO");

     	printf("Joystick [%s] has [%d] axes and [%d] buttons, driver version is [%d]\n",name,axes,nombreboutons,version);

}

void StickListener::readStick(int fdesc)
{
 //__u32 jstimestamp; // on ne fait rien avec le timestamp
/* __s16 jsvalue;
 __u8  jstype;
 __u8  jsnumber;
*/

printf("");

__int16_t jsvalue;
__uint8_t jstype;
__uint8_t  jsnumber;

 socketNotifier->setEnabled(false); // Qt instruction: disable pendant le traitement - voir url

	if ( fd != fdesc )  // rejete si pas le bon fd
	{
	  socketNotifier->setEnabled(true);
	  return;
	}

	if ( fd == 0 )  // Deconnexion
	{
		std::cout << "!DISCONNECTED\n";
		socketNotifier->setEnabled(true);
		return;
	}

	// Sans la structure joystick.h de Linux
	char buf[8];
	void *ptr;
	ptr = buf;
	//unsigned taille = 8;   // stucture joystick Linux sizeof(struct js_event) = 8 octets
	size_t taille = 5;
	
	int what = read(fd, buf, taille);
	
	int x    = (int)buf[0] & 0xff;
	int y    = (int)buf[1] & 0xff;
	int z    = (int)buf[2] & 0xff;
	int b1_8 = (int)buf[3] & 0xff;
	int b9_10= (int)buf[4] & 0xff;
	
	printf("what=%d X=%d Y=%d Z=%d b1=%d b2=%d\n", what, x, y, z, b1_8, b9_10);
	
	if ( b1_8 & 1 )
	{ 
	  bouton_appuiye = 1;
	  printf(" B1 down\n ");
	}
	if ( b1_8 & 2 )
	{ 
	  bouton_appuiye = 1;
	  printf(" B2 down\n ");
	}
	if ( b1_8 & 4 )
	{ 
	  bouton_appuiye = 1;
	  printf(" B3 down\n ");
	}
	if ( b1_8 & 8 )
	{ 
	  bouton_appuiye = 1;
	  printf(" B4 down\n ");
	}
	if ( b1_8 & 16 )
	{ 
	  bouton_appuiye = 1;
	  printf(" B5 down\n ");
	}

	
	if ( ( b1_8 == 0 ) && ( b9_10 == 0 ) && ( bouton_appuiye == 1) )
	{
	  bouton_appuiye = 0;
	  printf(" boutons relaches\n");
	} 
	
	/*
	if ( read(fd, ptr, taille) != 6 )
	{
		perror("! Error while reading Joystick !");
		//printf("Error Number = %d\n", errno);
	   return ;
	}
	*/
 //LINUX
	//__s16 xyzvalue;
	__int16_t xyzvalue;
	char bufswap[2];

	//jstimestamp = (__u32)buf[0];
	jsvalue     = (__int16_t)buf[4];
	jstype      = (__uint8_t)buf[6];
	jsnumber    = (__uint8_t)buf[7];

	// swap des bytes pour avoir une valeur de -128 a 127 sur les axes X, Y et Z
	bufswap[0] = buf[5];
	bufswap[1] = buf[4];
	//xyzvalue = (__s16)bufswap[0];
	xyzvalue = (__int16_t)bufswap[0];

	// AXES
	if ( (jstype == 2) && (jsnumber == 0) )
		printf(" -> X = % d\n",xyzvalue);
	if ( (jstype == 2) && (jsnumber == 1) )
		printf(" -> Y = % d\n",xyzvalue);
	if ( (jstype == 2) && (jsnumber == 2) )
		printf(" -> Z = % d\n",xyzvalue);

	// BOUTON APPUIYE ?
	if ( (jsvalue == 1) && (jstype == 1) )
		switch( jsnumber )
		{
		 case 0 : printf(" button 1 down\n"); break;
		 case 1 : printf(" button 2 down\n"); break;
		 case 2 : printf(" button 3 down\n"); break;
		 case 3 : printf(" button 4 down\n"); break;
		 case 4 : printf(" button 5 down\n"); break;
		 case 5 : printf(" button 6 down\n"); break;
		 case 6 : printf(" button 7 down\n"); break;
		 case 7 : printf(" button 8 down\n"); break;
		 case 8 : printf(" button 9 down\n"); break;
		 case 9 : printf(" button 10 down\n"); break;
		 case 10 : printf(" button 11 down\n"); break;
		 default : printf(" ! Unknown button !\n"); break;
		}
	// BOUTON RELACHE
	if ( (jsvalue == 0) && (jstype == 1) )
		switch( jsnumber )
		{
		 case 0 : printf(" button 1 up\n"); break;
		 case 1 : printf(" button 2 up\n"); break;
		 case 2 : printf(" button 3 up\n"); break;
		 case 3 : printf(" button 4 up\n"); break;
		 case 4 : printf(" button 5 up\n"); break;
		 case 5 : printf(" button 6 up\n"); break;
		 case 6 : printf(" button 7 up\n"); break;
		 case 7 : printf(" button 8 up\n"); break;
		 case 8 : printf(" button 9 up\n"); break;
		 case 9 : printf(" button 10 up\n"); break;
		 case 10 : printf(" button 11 up\n"); break;
		 default : printf(" ! Unknown button !\n"); break;
		}

  socketNotifier->setEnabled(true);
}

StickListener::~StickListener()
{
	// TODO Auto-generated destructor stub
}


