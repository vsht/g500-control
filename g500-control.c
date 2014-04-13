/*
 * g500_control - configure Logitech G500's internal profile under Linux
 * Copyright (C) 2011 Vladyslav Shtabovenko <DFEW.Entwickler@googlemail.com>
 * License: GPLv2
 * based on Andreas Schneider's g_hack.c posted on http://blog.cryptomilk.org/2011/02/22/logitech-linux-mouse-support/
 * HIGHLY EXPERIMENTAL! USE AT YOUR OWN RISK!
 *
 * Big thanks to Andreas Schneider and Marian Kyral
 * this program contains code from http://www.lomoco.org/vx_hack.c
 * thanks to the usbsnoop (http://sourceforge.net/projects/usbsnoop/) developers for providing a free, simple and reliable tool
 * for sniffing G500's communication with SetPoint.
 * This project does not have any affiliation with Logitech.
 */

/*
 * gcc -o g_hack g_hack.c
 * ./g_hack /dev/hidraw0
 *
 * g_hack - proof of conecpt code
 *
 *
 * Copyright (c) 2006-2009 Andreas Schneider <mail@cynapses.org>
 * Copyright (c) 2006-2007 Peter Feuerer <piie@piie.net>
 *
 * License: GPLv2 or later
 *
 * Additional Info:
 *
 * the G-series has two modes:
 *
 * 1. mode: no driver
 * in this mode there is a list of resolutions implemented in the mouse
 * and you can switch between them using the buttons
 *
 * 2. mode is: windows driver
 * in this mode the buttons talk to the driver
 * they have absolute no effect on the hardware in first place
 * the driver gets the button event looks in its table, which resolution to use
 * sends it to the mouse sets the leds and the resolution
 */

#define _POSIX_C_SOURCE 199309L

#ifndef G500_CONTROL_C
#define G500_CONTROL_C

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/hiddev.h>
#include "g500-control.h"



/*modified send_report since G500 also uses a 20 byte long buffer*/
static int send_report20(int fd, const unsigned char *buf, size_t size) {
  struct hiddev_report_info rinfo;
  struct hiddev_usage_ref uref;
  int i, err;

  for (i = 0; i < size; i++) {
    memset(&uref, 0, sizeof(uref));
    uref.report_type = HID_REPORT_TYPE_OUTPUT;
    uref.report_id   = 0x11;
    uref.field_index = 0;
    uref.usage_index = i;
    uref.usage_code  = 0xff000001;
    uref.value       = buf[i];

    err = ioctl(fd, HIDIOCSUSAGE, &uref);
    if (err < 0)
      return err;
  }

  memset(&rinfo, 0, sizeof(rinfo));
  rinfo.report_type = HID_REPORT_TYPE_OUTPUT;
  rinfo.report_id   = 0x11;
  rinfo.num_fields  = 1;
  err = ioctl(fd, HIDIOCSREPORT, &rinfo);

  return err;
}

static int send_report(int fd, const unsigned char *buf, size_t size) {
  struct hiddev_report_info rinfo;
  struct hiddev_usage_ref uref;
  int i, err;

  for (i = 0; i < size; i++) {
    memset(&uref, 0, sizeof(uref));
    uref.report_type = HID_REPORT_TYPE_OUTPUT;
    uref.report_id   = 0x10;
    uref.field_index = 0;
    uref.usage_index = i;
    uref.usage_code  = 0xff000001;
    uref.value       = buf[i];

    err = ioctl(fd, HIDIOCSUSAGE, &uref);
    if (err < 0)
      return err;
  }

  memset(&rinfo, 0, sizeof(rinfo));
  rinfo.report_type = HID_REPORT_TYPE_OUTPUT;
  rinfo.report_id   = 0x10;
  rinfo.num_fields  = 1;
  err = ioctl(fd, HIDIOCSREPORT, &rinfo);

  return err;
}

void send_msg(int fd, const unsigned char c0,
                      const unsigned char c1,
                      const unsigned char c2,
                      const unsigned char c3,
                      const unsigned char c4,
                      const unsigned char c5)
{
  unsigned char b[6];
  b[0]=c0;
  b[1]=c1;
  b[2]=c2;
  b[3]=c3;
  b[4]=c4;
  b[5]=c5;
  if(send_report(fd,b,6)<0)
  {
    perror("error sending to device");
    close(fd);
    exit(1);
  }
}
/*modified send_msg since G500 also uses a 20 byte long buffer*/
void send_msg20(int fd, const unsigned char c0,
                        const unsigned char c1,
                        const unsigned char c2,
                        const unsigned char c3,
                        const unsigned char c4,
                        const unsigned char c5,
                        const unsigned char c6,
                        const unsigned char c7,
                        const unsigned char c8,
                        const unsigned char c9,
                        const unsigned char c10,
                        const unsigned char c11,
                        const unsigned char c12,
                        const unsigned char c13,
                        const unsigned char c14,
                        const unsigned char c15,
                        const unsigned char c16,
                        const unsigned char c17,
                        const unsigned char c18 )
{
  unsigned char b[19];
  b[0]=c0;
  b[1]=c1;
  b[2]=c2;
  b[3]=c3;
  b[4]=c4;
  b[5]=c5;
  b[6]=c6;
  b[7]=c7;
  b[8]=c8;
  b[9]=c9;
  b[10]=c10;
  b[11]=c11;
  b[12]=c12;
  b[13]=c13;
  b[14]=c14;
  b[15]=c15;
  b[16]=c16;
  b[17]=c17;
  b[18]=c18;
  if(send_report20(fd,b,19)<0)
  {
    perror("error sending to device");
    close(fd);
    exit(1);
  }
}

/* function borrowed from http://www.lomoco.org/vx_hack.c */
static int query_report(int fd, int id, unsigned int *buf, size_t size) {
  struct hiddev_usage_ref_multi uref;
  struct hiddev_report_info rinfo;
  int i = 0, rc = -1;

  rinfo.report_type = HID_REPORT_TYPE_INPUT;
  rinfo.report_id = id;
  rinfo.num_fields = 1;
  rc = ioctl(fd, HIDIOCGREPORT, &rinfo);
  if (rc < 0) {
    perror("HIDIOCGREPORT");
    return rc;
  }

  uref.uref.report_type = HID_REPORT_TYPE_INPUT;
  uref.uref.report_id = id;
  uref.uref.field_index = 0;
  uref.uref.usage_index = 0;
  uref.num_values = size;
  rc = ioctl(fd, HIDIOCGUSAGES, &uref);
  if (rc < 0) {
    perror("HIDIOCGUSAGES");
    return rc;
  }

  for (i = 0; i < size; i++) {
    buf[i] = uref.values[i];
  }

  return rc;
}

/* new function hid_reply to check G500's replies. Contains code from vx_hack.c*/
void hid_reply(int fd) {
  unsigned int buf[6] = {0};
  int i;
  if (query_report(fd, 0x10, buf, 6) < 0) {
      perror("query failed");
      close(fd);
      exit(1);
  }
    printf("query result: ");
    for (i = 0; i < 6; i++) {
      printf(" %02x", buf[i]);
  }
    printf("\n");
}

void g500_reply (int fd, const int debug_infos) {
/* g500_reply(0) simply gives the mouse enough time to proceed the recieved information
   g500_reply(1) displays G500's reply to the recieved information */
 struct timespec req = {0,0};
 req.tv_sec = 0;
 req.tv_nsec = 100000000L;
 nanosleep(&req, (struct timespec *)NULL);
 if (debug_infos!=0)
  hid_reply(fd);

}

int dpi_convert (const int dpi_value) {
  int dpi_hex=0;
  if ((dpi_value!=0) && (200 <=dpi_value) && (dpi_value <= 5700) && (dpi_value % 100 == 0) ) {
    int rest = dpi_value % 400;
    int add_value=0;
    switch (rest){
      case 0:
        add_value=0;
      break;
      case 100:
        add_value=4;
      break;
      case 200:
        add_value=9;
      break;
      case 300:
        add_value=13;
      break;
      default:
      printf("%i is not a valid DPI value",dpi_value);
      exit(1);
    }
    dpi_hex = (dpi_value/400)*17+add_value;
  }
  else {
    printf("%i is not a valid DPI value",dpi_value);
    exit(1);
  }
  return dpi_hex;
}
void display_help() {
  printf("Usage:  g500_control HIDDEVICE URR AS DPI_MODE OPTIONS\n\n");
  printf("Please note that all arguments are necessary!\n");
  printf("The number of parameters in OPTIONS depends on the DPI_MODE you choose.\n");
  printf("The only exception is that when using --dpi_default for DPI_MODE, you don't need OPTIONS at all.\n");
  printf("If you get a 'hiddev open: Permission denied' error, run g500_control with sudo.\n\n");
  printf("HIDDEVICE - probably /dev/usb/hiddev0 or /dev/hidraw0\n");
  printf("URR - set G500's USB report rate\n");
  printf("  --urr_default   Set USB report rate equal 500 which is G500's default behavior\n");
  printf("  --125     Set USB report rate equal 125\n");
  printf("  --200     Set USB report rate equal 200\n");
  printf("  --250     Set USB report rate equal 250\n");
  printf("  --333     Set USB report rate equal 333\n");
  printf("  --500     Set USB report rate equal 500\n");
  printf("  --1000      Set USB report rate equal 1000\n");
  printf("AS - enable or disable angle snapping for drawing perfectly straight lines\n");
  printf("  --as_default    Disable angle snapping which is G500's default behavior\n");
  printf("  --as_on     Enable angle snapping\n");
  printf("  --as_off    Disable angle snapping\n");
  printf("DPI_MODE - choose the DPI mode\n");
  printf("  --dpi_default   Use three different dpi resolutions: 400, 800 and 2000 dpi.\n");
  printf("        Use dpi buttons to switch between different dpi values\n");
  printf("        which is G500's default behavior\n");
  printf("  --dpi_1     Use one dpi resolution and allow dpi buttons to be used as\n");
  printf("        normal buttons. They become visible in xev and can be mapped\n");
  printf("        to something useful via xbindkeys or other X-Window tools\n");
  printf("  --dpi_2     Use two different dpi resolutions. Use dpi buttons to\n");
  printf("        switch between different dpi values\n");
  printf("  --dpi_3     Use three different dpi resolutions. Use dpi buttons to\n");
  printf("        switch between different dpi values\n");
  printf("  --dpi_4     Use four different dpi resolutions. Use dpi buttons to\n");
  printf("        switch between different dpi values\n");
  printf("  --dpi_5     Use five different dpi resolutions. Use dpi buttons to\n");
  printf("        switch between different dpi values\n");
  printf("OPTIONS - set options according to the chosen DPI mode\n");
  printf("  For dpi_default no additional options are needed\n");
  printf("  For dpi_1 following options must be set\n");
  printf("  dpi_x     specify dpi resolution for the x axis. Valid values range from \n");
  printf("        200 to 5700 in 100 dpi steps, e.g. 200, 300, 400, 500, 600 etc.\n");
  printf("  dpi_y     specify dpi resolution for the y axis. Valid values range from\n");
  printf("        200 to 5700 in 100 dpi steps, e.g. 200, 300, 400, 500, 600 etc.\n");
  printf("  For dpi_2 following options must be set\n");
  printf("  dpi_1_x     specify 1st dpi resolution for the x axis.\n");
  printf("  dpi_1_y     specify 1st dpi resolution for the y axis.\n");
  printf("  dpi_2_x     specify 2nd dpi resolution for the x axis.\n");
  printf("  dpi_2_y     specify 2nd dpi resolution for the y axis.\n");
  printf("  dpi_btn     specify whether +/- (on the upper side of the mouse) or \n");
  printf("        forwards/backwards (on the left side of the mouse) buttons act as dpi\n");
  printf("        buttons. Valid values are dpi_btn_up and dpi_btn_side.\n");
  printf("  For dpi_3 following options must be set\n");
  printf("  dpi_1_x     specify 1st dpi resolution for the x axis.\n");
  printf("  dpi_1_y     specify 1st dpi resolution for the y axis.\n");
  printf("  dpi_2_x     specify 2nd dpi resolution for the x axis.\n");
  printf("  dpi_2_y     specify 2nd dpi resolution for the y axis.\n");
  printf("  dpi_3_x     specify 3rd dpi resolution for the x axis.\n");
  printf("  dpi_3_y     specify 3rd dpi resolution for the y axis.\n");
  printf("  dpi_btn     specify whether +/- (on the upper side of the mouse) or \n");
  printf("        forwards/backwards (on the left side of the mouse) buttons act as dpi\n");
  printf("        buttons. Valid values are dpi_btn_up and dpi_btn_side.\n");
  printf("  For dpi_4 following options must be set\n");
  printf("  dpi_1_x     specify 1st dpi resolution for the x axis.\n");
  printf("  dpi_1_y     specify 1st dpi resolution for the y axis.\n");
  printf("  dpi_2_x     specify 2nd dpi resolution for the x axis.\n");
  printf("  dpi_2_y     specify 2nd dpi resolution for the y axis.\n");
  printf("  dpi_3_x     specify 3rd dpi resolution for the x axis.\n");
  printf("  dpi_3_y     specify 3rd dpi resolution for the y axis.\n");
  printf("  dpi_4_x     specify 4th dpi resolution for the x axis.\n");
  printf("  dpi_4_y     specify 4th dpi resolution for the y axis.\n");
  printf("  dpi_btn     specify whether +/- (on the upper side of the mouse) or \n");
  printf("        forwards/backwards (on the left side of the mouse) buttons act as dpi\n");
  printf("        buttons. Valid values are dpi_btn_up and dpi_btn_side.\n");
  printf("  For dpi_5 following options must be set\n");
  printf("  dpi_1_x     specify 1st dpi resolution for the x axis.\n");
  printf("  dpi_1_y     specify 1st dpi resolution for the y axis.\n");
  printf("  dpi_2_x     specify 2nd dpi resolution for the x axis.\n");
  printf("  dpi_2_y     specify 2nd dpi resolution for the y axis.\n");
  printf("  dpi_3_x     specify 3rd dpi resolution for the x axis.\n");
  printf("  dpi_3_y     specify 3rd dpi resolution for the y axis.\n");
  printf("  dpi_4_x     specify 4th dpi resolution for the x axis.\n");
  printf("  dpi_4_y     specify 4th dpi resolution for the y axis.\n");
  printf("  dpi_5_x     specify 5th dpi resolution for the x axis.\n");
  printf("  dpi_5_y     specify 5th dpi resolution for the y axis.\n\n");
  printf("  dpi_btn     specify whether +/- (on the upper side of the mouse) or \n");
  printf("        forwards/backwards (on the left side of the mouse) buttons act as dpi\n");
  printf("        buttons. Valid values are dpi_btn_up and dpi_btn_side.\n");
  printf("Examples:\n");
  printf("  ./g500_control /dev/usb/hiddev0 --urr_default --as_default --dpi_default\n");
  printf("    this corresponds to G500's factory settings with 500 USB report rate,\n");
  printf("    angle snapping disabled and 3 dpi resolutions (400, 800 and 2000).\n");
  printf("    dpi buttons are used to switch between the 3 dpi values.\n");
  printf("  ./g500_control /dev/usb/hiddev0 --urr_default --as_default --dpi_1 1600 1600\n");
  printf("    here, only one dpi resolution (1600 dpi for both axes) is set. dpi buttons\n");
  printf("    can be mapped to anything you want via xbindkeys or similar tools.\n");
  printf("  ./g500_control /dev/usb/hiddev0 --1000 --as_on --dpi_1 1600 1600\n");
  printf("    same as above, but now we increase USB report rate from 500 to 1000 and\n");
  printf("    enable angle snapping.\n");
  printf("  ./g500_control /dev/usb/hiddev0 --1000 --as_on --dpi_1 3200 4000\n");
  printf("    same as above, but now we use different dpi resolutions for each axis.\n");
  printf("    for the x axis it's 3200 and for the y axis it's 4000.\n");
  printf("  ./g500_control /dev/usb/hiddev0 --500 --as_off --dpi_3 800 800 1600 1600 5700 5700 dpi_btn_up\n");
  printf("    here, we set 3 different dpi resolutions: 800, 1600 and 5700 (equal for both axes).\n");
  printf("    The dpi buttons (+/-) are used to switch between these. Moreover, we explicitly set USB\n");
  printf("    repeat rate to 500 and disable angle snapping\n");
  printf("  ./g500_control /dev/usb/hiddev0 --500 --as_off --dpi_3 800 800 1600 1600 5700 5700 dpi_btn_side\n");
  printf("    same as above but here we use (<-/->) as dpi buttons.\n");
  printf("  ./g500_control /dev/usb/hiddev0 --333 --as_on --dpi_5 800 800 1600 1600 3200 3200 4800 4800 5700 5700 dpi_btn_up\n");
  printf("    here, we set 5 different dpi resolutions: 800, 1600, 3200, 4800 and 5700\n");
  printf("    (equal for both axes). The dpi buttons (+/-) are used to switch between these. \n");
  printf("    Moreover, we explicitly set USB repeat rate to 333 and enable angle snapping\n");
  printf("  ./g500_control /dev/usb/hiddev0 --333 --as_on --dpi_5 800 800 1600 1000 3200 3200 4800 4800 5700 5700 dpi_btn_up\n");
  printf("    similar to the above, but the second resolution has now different values for the axes:\n");
  printf("    1600 dpi for the x axis and 1000 dpi for the y axis.\n");
}




int main (int argc, char **argv) {
  int fd = -1;
  int version;
  struct hiddev_devinfo device_info;

  int urr=0x02;
  int as=0x01;
  int dpi_1_x=0;
  int dpi_1_y=0;
  int dpi_2_x=0;
  int dpi_2_y=0;
  int dpi_3_x=0;
  int dpi_3_y=0;
  int dpi_4_x=0;
  int dpi_4_y=0;
  int dpi_5_x=0;
  int dpi_5_y=0;
  int dpi_btn_1=0;
  int dpi_btn_2=0;
  int dpi_btn_3=0;
  int dpi_btn_4=0;
  int dpi_btn_5=0;
  int dpi_btn_6=0;
  int dpi_btn_7=0;
  int dpi_btn_8=0;
  int dpi_btn_9=0;
  int md_1=0;
  int md_2=0;
  int md_3=0;
  int md_4=0;
  int md_5=0;
  int md_7=0;
  int md_8=0;
  int md_9=0;
  int md_10=0;



  printf("\n");
  printf("%c[%d;%d;%dmWARNING!", 0x1B, 1,31,40);
  printf("THIS MAY POTENTIALLY BRICK YOUR LOGITECH G500 MOUSE,\n");
  printf("SO USE IT ON YOUR OWN RISK! NO LIABILITY FOR ANY DIRECT OR INDIRECT DAMAGE CAUSED\n");
  printf("BY USING THIS PROGRAM IS ACCEPTED!\n");
  printf("%c[%dm\n", 0x1B, 0);
  printf("g500_control %s is based on Andreas Schneider's g_hack.c and\n", VERSION);
  printf("allows you to change Logitech G500's internal profile under Linux\n\n");

  if (argc==1) {
    display_help();
    exit(1);
  }

  else if (((argc>1) && (argc < 5)) || (argc>16)) {
    fprintf(stderr,"g500_control error: too many or too little arguments!\n");
    printf("To display help run g500_control without any arguments.\n");
    exit(1);
  }

  /* ioctl() requires a file descriptor, so we check we got one, and then open it */
  if ((fd = open(argv[1], O_RDONLY)) < 0) {
    perror("hiddev open");
    printf("%s is not a mouse!\n",argv[1]);
    exit(1);
  }
  /* ioctl() accesses the underlying driver */
  ioctl(fd, HIDIOCGVERSION, &version);

  /* the HIDIOCGVERSION ioctl() returns an int so we unpack it and display it we create a patch */
  printf("hiddev driver version is %d.%d.%d\n", version >> 16, (version >> 8) & 0xff, version & 0xff);

  /* suck out some device information */
  ioctl(fd, HIDIOCGDEVINFO, &device_info);

  /* the HIDIOCGDEVINFO ioctl() returns hiddev_devinfo
   * structure - see <linux/hiddev.h>
   * So we work through the various elements, displaying
   * each of them
   */
  printf("vendor 0x%04hx product 0x%04hx version 0x%04hx ",
      device_info.vendor, device_info.product, device_info.version);
  printf("has %i application%s ", device_info.num_applications, (device_info.num_applications==1?"":"s"));
  printf("and is on bus: %d   devnum: %d   ifnum: %d\n", device_info.busnum, device_info.devnum, device_info.ifnum);
  /* We have a G500? */
  if((device_info.vendor == (short)VENDOR) && (device_info.product == (short)MOUSE_G500)) {
    printf("Logitech G500 Gaming Mouse detected!\n");

    if (ioctl(fd, HIDIOCINITREPORT, 0) < 0) {
      perror("hid report init failed");
      exit(1);
    }

  /* Checking the input parameters and setting the appropriate registers */

    if ((strcmp(argv[2],"--125") == 0) || (strcmp(argv[2],"--200") == 0) || (strcmp(argv[2],"--250") == 0) ||
    (strcmp(argv[2],"--333") == 0) || (strcmp(argv[2],"--500") == 0) || (strcmp(argv[2],"--1000") == 0) ||
    (strcmp(argv[2],"--urr_default") == 0)) {
      if (strcmp(argv[2],"--125") == 0)
        urr=0x08;
      else if (strcmp(argv[2],"--200") == 0)
        urr=0x05;
      else if (strcmp(argv[2],"--250") == 0)
        urr=0x04;
      else if (strcmp(argv[2],"--333") == 0)
        urr=0x03;
      else if ((strcmp(argv[2],"--500") == 0) || (strcmp(argv[2],"--urr_default") == 0))
        urr=0x02;
      else if (strcmp(argv[2],"--1000") == 0)
        urr=0x01;
    }
    else {
      printf("%s is not a valid URR value!\n",argv[2]);
      exit(1);
    }

    if ((strcmp(argv[3],"--as_on") == 0) || (strcmp(argv[3],"--as_off") == 0) || (strcmp(argv[3],"--as_default") == 0)) {
      if (strcmp(argv[3],"--as_on") == 0)
        as=0x02;
      else if ((strcmp(argv[3],"--as_off") == 0) || (strcmp(argv[3],"--as_default") == 0))
        as=0x01;
    }
    else {
      printf("%s is not a valid AS value!\n",argv[3]);
      exit(1);
    }

    if ((strcmp(argv[4],"--dpi_1") == 0) || (strcmp(argv[4],"--dpi_2") == 0) || (strcmp(argv[4],"--dpi_3") == 0) ||
    (strcmp(argv[4],"--dpi_4") == 0) ||  (strcmp(argv[4],"--dpi_5") == 0) ||  (strcmp(argv[4],"--dpi_default") == 0)) {
      if (strcmp(argv[4],"--dpi_default") == 0) {
        if (argc==5) {
          dpi_1_x=dpi_convert(400);
          dpi_1_y=dpi_convert(400);
          dpi_2_x=dpi_convert(800);
          dpi_2_y=dpi_convert(800);
          dpi_3_x=dpi_convert(2000);
          dpi_3_y=dpi_convert(2000);
          md_1=0x22;
          md_2=0x12;
          md_3=0x22;
          md_4=0x22;
          md_5=0x01;
          md_7=0x80;

          dpi_btn_1=0x83;
          dpi_btn_2=0x04;
          dpi_btn_3=0x00;
          dpi_btn_4=0x83;
          dpi_btn_5=0x08;
          dpi_btn_6=0x00;
          dpi_btn_7=0x81;
          dpi_btn_8=0x81;
          dpi_btn_9=0x10;

        }
       else {
         printf("Too much or too little parameters for the %s mode",argv[4]);
         exit(1);
       }
    }

    else if (strcmp(argv[4],"--dpi_1") == 0) {
      if (argc==7) {
        dpi_1_x=dpi_convert(atoi(argv[5]));
        dpi_1_y=dpi_convert(atoi(argv[6]));
        dpi_btn_1=0x81;
        dpi_btn_2=0x00;
        dpi_btn_3=0x01;
        dpi_btn_4=0x81;
        dpi_btn_5=0x00;
        dpi_btn_6=0x02;
        dpi_btn_7=0x81;
        dpi_btn_8=0x81;
        dpi_btn_9=0x10;
      }
      else {
        printf("Too much or too little parameters for the %s mode",argv[4]);
        exit(1);
      }
    }

    else if (strcmp(argv[4],"--dpi_2") == 0) {
      if (argc==10) {
        dpi_1_x=dpi_convert(atoi(argv[5]));
        dpi_1_y=dpi_convert(atoi(argv[6]));
        dpi_2_x=dpi_convert(atoi(argv[7]));
        dpi_2_y=dpi_convert(atoi(argv[8]));

        if ((strcmp(argv[9],"dpi_btn_up") == 0) || (strcmp(argv[9],"dpi_btn_left") == 0)) {
          if (strcmp(argv[9],"dpi_btn_up") == 0) {
            dpi_btn_1=0x83;
            dpi_btn_2=0x04;
            dpi_btn_3=0x00;
            dpi_btn_4=0x83;
            dpi_btn_5=0x08;
            dpi_btn_6=0x00;
            dpi_btn_7=0x81;
            dpi_btn_8=0x81;
            dpi_btn_9=0x10;
          }
          else if (strcmp(argv[9],"dpi_btn_left") == 0) {
            dpi_btn_1=0x81;
            dpi_btn_2=0x00;
            dpi_btn_3=0x01;
            dpi_btn_4=0x81;
            dpi_btn_5=0x00;
            dpi_btn_6=0x02;
            dpi_btn_7=0x83;
            dpi_btn_8=0x83;
            dpi_btn_9=0x04;
          }
        }
        else {
          printf("%s is not a valid dpi button parameter",argv[9]);
          exit(1);
        }
        md_1=0x22;
        md_2=0x12;
        md_5=0x01;

      }
      else {
        printf("Too much or too little parameters for the %s mode",argv[4]);
        exit(1);
      }
    }

    else if (strcmp(argv[4],"--dpi_3") == 0) {
      if (argc==12) {
        dpi_1_x=dpi_convert(atoi(argv[5]));
        dpi_1_y=dpi_convert(atoi(argv[6]));
        dpi_2_x=dpi_convert(atoi(argv[7]));
        dpi_2_y=dpi_convert(atoi(argv[8]));
        dpi_3_x=dpi_convert(atoi(argv[9]));
        dpi_3_y=dpi_convert(atoi(argv[10]));

        if ((strcmp(argv[11],"dpi_btn_up") == 0) || (strcmp(argv[11],"dpi_btn_left") == 0)) {
          if (strcmp(argv[11],"dpi_btn_up") == 0) {
            dpi_btn_1=0x83;
            dpi_btn_2=0x04;
            dpi_btn_3=0x00;
            dpi_btn_4=0x83;
            dpi_btn_5=0x08;
            dpi_btn_6=0x00;
            dpi_btn_7=0x81;
            dpi_btn_8=0x81;
            dpi_btn_9=0x10;
          }
          else if (strcmp(argv[11],"dpi_btn_left") == 0) {
            dpi_btn_1=0x81;
            dpi_btn_2=0x00;
            dpi_btn_3=0x01;
            dpi_btn_4=0x81;
            dpi_btn_5=0x00;
            dpi_btn_6=0x02;
            dpi_btn_7=0x83;
            dpi_btn_8=0x83;
            dpi_btn_9=0x04;
          }
        }
        else {
          printf("%s is not a valid dpi button parameter",argv[11]);
          exit(1);
        }
        md_1=0x22;
        md_2=0x12;
        md_3=0x22;
        md_4=0x22;
        md_5=0x01;
        md_7=0x80;
      }
      else {
       printf("Too much or too little parameters for the %s mode",argv[4]);
       exit(1);
      }
    }

    else if (strcmp(argv[4],"--dpi_4") == 0) {
      if (argc==14) {
        dpi_1_x=dpi_convert(atoi(argv[5]));
        dpi_1_y=dpi_convert(atoi(argv[6]));
        dpi_2_x=dpi_convert(atoi(argv[7]));
        dpi_2_y=dpi_convert(atoi(argv[8]));
        dpi_3_x=dpi_convert(atoi(argv[9]));
        dpi_3_y=dpi_convert(atoi(argv[10]));
        dpi_4_x=dpi_convert(atoi(argv[11]));
        dpi_4_y=dpi_convert(atoi(argv[12]));
        if ((strcmp(argv[13],"dpi_btn_up") == 0) || (strcmp(argv[13],"dpi_btn_left") == 0)) {
          if (strcmp(argv[13],"dpi_btn_up") == 0) {
            dpi_btn_1=0x83;
            dpi_btn_2=0x04;
            dpi_btn_3=0x00;
            dpi_btn_4=0x83;
            dpi_btn_5=0x08;
            dpi_btn_6=0x00;
            dpi_btn_7=0x81;
            dpi_btn_8=0x81;
            dpi_btn_9=0x10;
          }
          else if (strcmp(argv[13],"dpi_btn_left") == 0) {
            dpi_btn_1=0x81;
            dpi_btn_2=0x00;
            dpi_btn_3=0x01;
            dpi_btn_4=0x81;
            dpi_btn_5=0x00;
            dpi_btn_6=0x02;
            dpi_btn_7=0x83;
            dpi_btn_8=0x83;
            dpi_btn_9=0x04;
          }
        }
        else {
          printf("%s is not a valid dpi button parameter",argv[13]);
          exit(1);
        }
        md_1=0x22;
        md_2=0x12;
        md_3=0x22;
        md_4=0x22;
        md_5=0x01;
        md_7=0x12;
        md_8=0x22;
      }
      else {
       printf("Too much or too little parameters for the %s mode",argv[4]);
       exit(1);
      }
    }

    else if (strcmp(argv[4],"--dpi_5") == 0) {
      if (argc==16) {
        dpi_1_x=dpi_convert(atoi(argv[5]));
        dpi_1_y=dpi_convert(atoi(argv[6]));
        dpi_2_x=dpi_convert(atoi(argv[7]));
        dpi_2_y=dpi_convert(atoi(argv[8]));
        dpi_3_x=dpi_convert(atoi(argv[9]));
        dpi_3_y=dpi_convert(atoi(argv[10]));
        dpi_4_x=dpi_convert(atoi(argv[11]));
        dpi_4_y=dpi_convert(atoi(argv[12]));
        dpi_5_x=dpi_convert(atoi(argv[13]));
        dpi_5_y=dpi_convert(atoi(argv[14]));
        if ((strcmp(argv[15],"dpi_btn_up") == 0) || (strcmp(argv[15],"dpi_btn_left") == 0)) {
          if (strcmp(argv[15],"dpi_btn_up") == 0) {
            dpi_btn_1=0x83;
            dpi_btn_2=0x04;
            dpi_btn_3=0x00;
            dpi_btn_4=0x83;
            dpi_btn_5=0x08;
            dpi_btn_6=0x00;
            dpi_btn_7=0x81;
            dpi_btn_8=0x81;
            dpi_btn_9=0x10;
          }
          else if (strcmp(argv[15],"dpi_btn_left") == 0) {
            dpi_btn_1=0x81;
            dpi_btn_2=0x00;
            dpi_btn_3=0x01;
            dpi_btn_4=0x81;
            dpi_btn_5=0x00;
            dpi_btn_6=0x02;
            dpi_btn_7=0x83;
            dpi_btn_8=0x83;
            dpi_btn_9=0x04;
          }
        }
        else {
          printf("%s is not a valid dpi button parameter",argv[15]);
          exit(1);
        }
        md_1=0x22;
        md_2=0x12;
        md_3=0x22;
        md_4=0x22;
        md_5=0x01;
        md_7=0x12;
        md_8=0x22;
        md_9=0x12;
        md_10=0x21;
      }
      else {
       printf("Too much or too little parameters for the %s mode",argv[4]);
       exit(1);
      }
    }
  }
  else {
    printf("%s is not a valid DPI_MODE value!\n",argv[4]);
    exit(1);
  }

  printf("Sending the initialization sequence ... ");
  /* This is the initialization sequence SetPoint sends to a G500 when you plug it into USB port.
     You can't flash a profile without sending this first.
     I'm not sure whether this sequence is universal for all G500's or differs from mouse to mouse. This
     is what I get with SetPoint 6.20 and my G500 (P/N 810-001240, M/N M-U0010) */
  send_msg(fd,0x00,0x80,0xa1,0x01,0x00,0x02);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x83,0xa2,0x01,0x80,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x83,0xa2,0x01,0x88,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x83,0xa2,0x01,0x80,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x83,0xa2,0x01,0x88,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x83,0xa2,0x01,0x80,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x81,0x0f,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg20(fd,0x00,0x82,0xa0,0x04,0x00,0x01,0x02,0x00,0x00,0x00,0xa1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x83,0xa2,0x00,0xa1,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x83,0xa2,0x00,0xa1,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x83,0x63,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x81,0x0f,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x80,0x0f,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg20(fd,0x00,0x90,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x4e,0x00,0x00,0xff,0x00,0x00,0x80,0x00,0x22,0x00);
  send_msg20(fd,0x00,0x91,0x01,0x22,0x22,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
  send_msg20(fd,0x00,0x91,0x02,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x10,0x10,0x02);
  send_msg20(fd,0x00,0x91,0x03,0x81,0x01,0x00,0x81,0x02,0x00,0x81,0x04,0x00,0x81,0x08,0x00,0x81,0x10,0x00,0x81);
  send_msg20(fd,0x00,0x91,0x04,0x20,0x00,0x81,0x40,0x00,0x81,0x80,0x00,0x81,0x00,0x01,0x81,0x00,0x02,0x8f,0x00);
  send_msg20(fd,0x00,0x93,0x05,0x00,0x8f,0x00,0x00,0x8f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x80,0x0f,0x01,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  printf("done!\n");

  /* This is the actual profile */
  printf("Flashing new profile ... ");

  /* 1st part of the profile */
  send_msg20(fd,0x00,0x82,0xa0,0x02,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg20(fd,0x00,0x90,0x06,0x01,0x02,0x00,0x00,0x00,0x00,0x4e,0x00,0x00,0xff,0x00,0x00,0x80,0x00,dpi_1_x,0x00);
  send_msg20(fd,0x00,0x91,0x07,dpi_1_y,0x22,0x11,0x00,dpi_2_x,0x00,dpi_2_y,md_1,md_2,0x00,dpi_3_x,0x00,dpi_3_y,md_3,md_4,0x00);
  send_msg20(fd,0x00,0x91,0x08,dpi_4_x,0x00,dpi_4_y,md_7,md_8,0x00,dpi_5_x,0x00,dpi_5_y,md_9,md_10,as,md_5,0x10,0x10,urr);
  send_msg20(fd,0x00,0x91,0x09,0x81,0x01,0x00,0x81,0x02,0x00,0x81,0x04,0x00,dpi_btn_7,0x08,0x00,dpi_btn_8,dpi_btn_9,0x00,0x81);
  send_msg20(fd,0x00,0x91,0x0a,0x20,0x00,0x83,0x01,0x00,0x83,0x02,0x00,dpi_btn_1,dpi_btn_2,dpi_btn_3,dpi_btn_4,dpi_btn_5,dpi_btn_6,0x8f,0x00);
  send_msg20(fd,0x00,0x93,0x0b,0x00,0x8f,0x00,0x00,0x8f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg20(fd,0x00,0x92,0x0c,0x01,0x02,0xff,0x00,0x00,0x00,0x02,0x00,0x00,0x79,0x13,0x00,0x00,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x83,0x63,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x80,0x0f,0x00,0x00,0x00);
  /* 2nd part of the profile */
  g500_reply(fd,DEBUG_INFOS);
  send_msg20(fd,0x00,0x90,0x0d,0x01,0x00,0x00,0x00,0x00,0x00,0x4e,0x00,0x00,0xff,0x00,0x00,0x80,0x00,dpi_1_x,0x00);
  send_msg20(fd,0x00,0x91,0x0e,dpi_1_y,0x22,0x11,0x00,dpi_2_x,0x00,dpi_2_y,md_1,md_2,0x00,dpi_3_x,0x00,dpi_3_y,md_3,md_4,0x00);
  send_msg20(fd,0x00,0x91,0x0f,dpi_4_x,0x00,dpi_4_y,md_7,md_8,0x00,dpi_5_x,0x00,dpi_5_y,md_9,md_10,as,md_5,0x10,0x10,urr);
  send_msg20(fd,0x00,0x91,0x10,0x81,0x01,0x00,0x81,0x02,0x00,0x81,0x04,0x00,dpi_btn_7,0x08,0x00,dpi_btn_8,dpi_btn_9,0x00,0x81);
  send_msg20(fd,0x00,0x91,0x11,0x20,0x00,0x81,0x40,0x00,0x81,0x80,0x00,dpi_btn_1,dpi_btn_2,dpi_btn_3,dpi_btn_4,dpi_btn_5,dpi_btn_6,0x8f,0x00);
  send_msg20(fd,0x00,0x93,0x12,0x00,0x8f,0x00,0x00,0x8f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
  g500_reply(fd,DEBUG_INFOS);
  send_msg(fd,0x00,0x80,0x0f,0x01,0x00,0x00);
  printf ("done!\n");
  }
  else
    printf("No G500 found! This program works only with Logitech G500!\n");
  close(fd);
  exit(0);
}

#endif /* G500_CONTROL */
