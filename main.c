//미완

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wiringPi.h>
#include "iotmakers.h"
#include "gps.h"

#define butPin 5

static int local_loop = (0);
static void SigHandler(int sig)
{
switch(sig)
{
case SIGTERM :
case SIGINT :
printf("accept signal SIGINT[%d]\n", sig);
im_stop_service();
local_loop = (0);
break;
default :
;
};
return;
}
static void set_SigHandler()
{
signal(SIGINT, SigHandler);
signal(SIGTERM, SigHandler);
}

int main()
{
int i;
int rc;
double val = 0.0;
set_SigHandler();
printf("im_init()\n");
rc = im_init_with_config_file("./config.txt");
if ( rc < 0 ) {
printf("fail im_init()\n");
return -1;
}
im_set_loglevel(LOG_LEVEL_DEBUG);
printf("im_start_service()...\n");
rc = im_start_service();
if ( rc < 0 ) {
printf("fail im_start_service()\n");
im_release();
return -1;
}
local_loop = (1);

//
wiringPiSetupGpio();
gps_init();
loc_t data;
gps_location(&data);
double lati = data.latitude;
double longi = data.longitude;
pinMode(butPin, INPUT);
pullUpDnControl(butPin, PUD_UP); 
int ret;

val = 0;
while (local_loop == (1))
{
rc = im_init_complexdata();
if ( rc < 0 ) {
printf("ErrCode[%d]\n", im_get_LastErrCode());
break;
}

//
ret=digitalRead(butPin);
if (!ret)
{
rc = im_add_complex_data_number("lat", lati);
if ( rc < 0 ) {
printf("ErrCode[%d]\n", im_get_LastErrCode());
break;
}
rc = im_add_complex_data_number("lon", longi);
if ( rc < 0 ) {
printf("ErrCode[%d]\n", im_get_LastErrCode());
break;
}
printf("im_send_complexdata()...\n");
rc = im_send_complexdata(0);
if ( rc < 0 ) {
printf("ErrCode[%d]\n", im_get_LastErrCode());
return -1;
}

}

sleep(5);
}
printf("im_stop_service()\n");
im_stop_service();
printf("im_release()\n");
im_release();
return 0;
}
