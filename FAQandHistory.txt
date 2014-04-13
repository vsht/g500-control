g500_control - configure Logitech G500's internal profile under Linux
Copyright (C) 2011 Vladyslav Shtabovenko <DFEW.Entwickler@googlemail.com>

This project does not have any affiliation with Logitech or lomoco

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


Why and how I created g500_control
========================================================================
I'm a loyal Logitech customer since 2005 and I really like Logitech's 
devices. Although not a gamer myself I enjoy using their gaming mice 
like MX510, G5 or G500. SetPoint is a great software which allows you to
configure a lot of things, but unfortunately it's not available for 
Linux. Whether you can live with it or not depends on the particular 
mouse model you have. While using MX510 and G5 I didn't miss SetPoint 
much because for changing MX510's dpi there is lomoco and G5 even has 
dedicated dpi buttons. But G500 is different. This mouse has so many cool
features like angle snapping, customizable USB report rate, dpi 
resolution adjustable in 100 dpi steps or persistent memory that I was 
seriously troubled about the fact that until now there was no Linux 
software able to modify those settings. Of course, you can boot into 
Windows, flash the necessary profile using SetPoint and then keep using
it under Linux, but due to the fact that I rarely use Windows, this 
workaround proved to be too cumbersome for me. While googling about this 
issue I stumbled upon Andreas Schneider's blog 
(http://blog.cryptomilk.org/2011/02/22/logitech-linux-mouse-support/)
where he presented a proof of concept code g_hack.c for changing 
resoultions of different Logitech's G series models. I tried it out on 
my G500 but it didn't work. Reading user comments on that post I found 
a comment by Marian Kyral where he described his efforts to get the 
battery status of his MX Anywhere by tracing USB communication via 
SnoopyPro (http://sourceforge.net/projects/usbsnoop/files/SnoopyPro/). 
I downloaded this tool and managed to get it working with my G500 under 
Windows 7 while using SetPoint 6.20. This allowed me to understand how 
SetPoint communicates with a G500 and what must be done to change G500's
internal memory. Here I'd like to make clear that I DID NOT hack, crack
or disassemble SetPoint or G500's firmware. In fact, I don't even know 
to use a disassembler or how to reverse engineer a firmware.  The USB 
communication between SetPoint and a G500 is not encrypted and is quite 
easy to understand if you change only one setting per time and then 
compare the logs to find out which bytes have been changed. All you need
is time and some basic HEX math knowledge. Subsequently, I modified 
Andreas Schneider's code to make it compatible with my G500 
(e.g. adding 20 byte buffers). Originally, I only wanted to have a quick
and dirty dpi switcher for my personal needs but as I discovered how to
enable angle snapping or change USB report rate, I realized that it 
would be good to share the modified code with the Linux community. 
Which I'm hereby doing.

g500_control's current status
========================================================================
Please don't forget that g500_control is highly experimental in a sense 
that so far it has been tested only on a single G500: my own. As far as 
my G500 is concerned, g500_control works really great and allows me to 
change much of the settings otherwise available only via SetPoint. If 
you decide to test it on your G500, you must understand that you're 
doing it on your own risk. I bear absolutely no responsibility for 
anything that may happen to your G500 while using g500_control. On the 
other hand, I really tried hard to make the code safe concerning user 
input and to test everything I could test very thoroughly. What you 
could do to be on the safe side is to use SnoopyPro to examine the 
communication between the SetPoint on your machine and your G500. Check 
the source code for the initialization sequence and compare it with the 
initialization sequence in your logs. If they match, then g500_control 
probably should work with your G500 too. But once again, I can't give 
you any guaranty and everything you're doing is on your own risk.

Any particular tips on installing g500_control?
========================================================================
Not really. Just compile it with "gcc -o g500_control g500_control.c"
And use it with sudo. If you want the program to work without password 
prompt, you might copy it to /usr/bin "sudo cp g500_control /usr/bin" 
and apply  "sudo chmod ug+s /usr/bin/g500_control". Be aware that making 
a program SUID is generally a dangerous and not really 
appreciated thing in the Linux world.

How to use
========================================================================
To display help, simply run g500_control without any arguments. The help
is actually pretty self explanatory. I also included some examples to 
help you customizing your settings. At the end of this file you'll find 
the help normally displayed by g500_control

Are the changes made by g500_control persistent?
========================================================================
g500_control writes desired settings directly into G500's internal 
memory just as SetPoint does. Those settings are persistent and are not
lost if you reboot your PC or unplug the mouse. Nevertheless you should
remember that when using G500 on a Windows systems with SetPoint 
installed, SetPoint will always flash its own settings without asking
your permission. Thus, if you dual boot Linux and Windows and use 
different profiles for each OS, you may consider running g500_control 
automatically when starting Linux, for it to load your desired settings.

How can I check my current USB report rate?
========================================================================
Google for evhz.c (currently available here http://hlds.pl/Evhz.c)
compile it and run evhz as root. Now move your G500. The "average hz"
value should correspond to your current USB report rate 

How I can check whether angle snapping is working?
========================================================================
Open your favorite graphics editor, choose a pencil/brush tool and try
to draw a straight vertical or horizontal line. Compared to angle 
snapping disabled, your line should be almost ideally straight.

What about buttons mapping and macros like in SetPoint?
========================================================================
Currently the only button mapping implemented is the ability to map dpi
buttons to +/- or forwards/backwards when using more than one dpi 
resolution. When using only one dpi resolution, +/- buttons become 
visible to X11 and thus can be mapped via xbindkeys or similar tools
to something useful. Making an arbitrary button to act as dpi increase/
decrease button is not difficult, but making an arbitrary button perform
an arbitrary action is. The problem is that SetPoint uses a mixed 
strategy to map certain actions to certain buttons. For some actions,
the buttons really do send appropriate keycodes but for others SetPoint
simply detects the pressed button and performs the defined action 
itself. Moreover, under Linux you don't really need to map buttons to 
actions on the hardware level. There are tools like xbindkeys around 
which allow you to map any button to anything you want. Thus, I might 
look into allowing more buttons act like dpi buttons but I'm definitely
not going to waste my time for figuring out which actions can be mapped 
to certain buttons on the hardware level and which not.

What about acceleration/sensitivity settings?
=======================================================================
I tried really hard to find out where these settings are saved but in 
the meantime I tend to believe that these are provided only by the 
SetPoint software, i.e. not by G500's hardware. There is a simple 
experiment to check this out. In SetPoint, turn off OS sensitivity 
control and set both sensitivity and acceleration to 0. This will make
your G500 extremely slow. If you, however, reboot to Linux, you'll notice
that the G500 works as usual and isn't slow at all. The same holds
also for vertical and horizontal scrolling speed. Of course, I can't know
for sure, but at the moment I consider all those to be software emulated
and not something you can change on the hardware level.

Do you plan a GUI version of g500_control?
========================================================================
Unfortunately, I know very little about GUI programming and it probably
would take me too much time to create a more or less working GUI. May be
someone kind enough will provide us a GUI/frontend someday ;)

Why it's always necessary input all profile settings?
========================================================================
The thing is that you can't simply change one particular parameter in 
G500's internal profile without reflashing the whole profile. SetPoint 
basically does the same, but since it keeps your complete profile(s) in
registry, you don't notice that each time you change a single setting and
click apply, the whole profile gets reflashed.

I tried playing with the source code and now my G500 doesn't work at all
========================================================================
This actually happened to me several times while coding g500_control. 
In my case, however, the mouse started working again after I rebooted
into Windows with SetPoint. My theory is that every time SetPoint 
detects a G500, it flashes the mouse with the predefined profile which
literally brings it back to life. I, however, in no way claim that it's
impossible to brick a G500 completely by flashing it with junk data.
So, be careful!

Do you plan to actively develop g500_control?
========================================================================
Frankly speaking, I regard g500_control to be rather a proof of concept
than a standalone utility worth developing. On one hand, I'm a 
very poor programmer and chronically lack free time. On the other hand,
I use g500_control myself, so as long as I'll be using my G500 under 
Linux you can expect to more or less maintain the code.

g500_control internal help
========================================================================
WARNING! HIGHLY EXPERIMENTAL! THIS MAY POTENTIALLY BRICK YOUR LOGITECH G500 MOUSE,
SO USE IT ON YOUR OWN RISK! NO LIABILITY FOR ANY DIRECT OR INDIRECT DAMAGE CAUSED
BY USING THIS PROGRAM IS ACCEPTED!

g500_control 0.0.1 is based on Andreas Schneider's g_hack.c and
allows you to change Logitech G500's internal profile under Linux

Usage:  g500_control HIDDEVICE URR AS DPI_MODE OPTIONS

Please note that all arguments are necessary!
The number of parameters in OPTIONS depends on the DPI_MODE you choose.
The only exception is that when using --dpi_default for DPI_MODE, you don't need OPTIONS at all.
If you get a 'hiddev open: Permission denied' error, run g500_control with sudo.

HIDDEVICE - probably /dev/usb/hiddev0 or /dev/hidraw0
URR - set G500's USB report rate
 	--urr_default		Set USB report rate equal 500 which is G500's default behavior
	--125			Set USB report rate equal 125
	--200			Set USB report rate equal 200
	--250			Set USB report rate equal 250
	--333			Set USB report rate equal 333
	--500			Set USB report rate equal 500
	--1000			Set USB report rate equal 1000
AS - enable or disable angle snapping for drawing perfectly straight lines
	--as_default		Disable angle snapping which is G500's default behavior
	--as_on			Enable angle snapping
	--as_off		Disable angle snapping
DPI_MODE - choose the DPI mode
	--dpi_default		Use three different dpi resolutions: 400, 800 and 2000 dpi.
				Use dpi buttons to switch between different dpi values
				which is G500's default behavior
	--dpi_1			Use one dpi resolution and allow dpi buttons to be used as
				normal buttons. They become visible in xev and can be mapped
				to something useful via xbindkeys or other X-Window tools
	--dpi_2			Use two different dpi resolutions. Use dpi buttons to
				switch between different dpi values
	--dpi_3			Use three different dpi resolutions. Use dpi buttons to
				switch between different dpi values
	--dpi_4			Use four different dpi resolutions. Use dpi buttons to
				switch between different dpi values
	--dpi_5			Use five different dpi resolutions. Use dpi buttons to
				switch between different dpi values
OPTIONS - set options according to the chosen DPI mode
  For dpi_default no additional options are needed
  For dpi_1 following options must be set
	dpi_x			specify dpi resolution for the x axis. Valid values range from 
				200 to 5700 in 100 dpi steps, e.g. 200, 300, 400, 500, 600 etc.
	dpi_y			specify dpi resolution for the y axis. Valid values range from
				200 to 5700 in 100 dpi steps, e.g. 200, 300, 400, 500, 600 etc.
  For dpi_2 following options must be set
	dpi_1_x			specify 1st dpi resolution for the x axis.
	dpi_1_y			specify 1st dpi resolution for the y axis.
	dpi_2_x			specify 2nd dpi resolution for the x axis.
	dpi_2_y			specify 2nd dpi resolution for the y axis.
	dpi_btn			specify whether +/- (on the upper side of the mouse) or 
				forwards/backwards (on the left side of the mouse) buttons act as dpi
				buttons. Valid values are dpi_btn_up and dpi_btn_side.
  For dpi_3 following options must be set
	dpi_1_x			specify 1st dpi resolution for the x axis.
	dpi_1_y			specify 1st dpi resolution for the y axis.
	dpi_2_x			specify 2nd dpi resolution for the x axis.
	dpi_2_y			specify 2nd dpi resolution for the y axis.
	dpi_3_x			specify 3rd dpi resolution for the x axis.
	dpi_3_y			specify 3rd dpi resolution for the y axis.
	dpi_btn			specify whether +/- (on the upper side of the mouse) or 
				forwards/backwars (on the left side of the mouse) buttons act as dpi
				buttons. Valid values are dpi_btn_up and dpi_btn_side.
  For dpi_4 following options must be set
	dpi_1_x			specify 1st dpi resolution for the x axis.
	dpi_1_y			specify 1st dpi resolution for the y axis.
	dpi_2_x			specify 2nd dpi resolution for the x axis.
	dpi_2_y			specify 2nd dpi resolution for the y axis.
	dpi_3_x			specify 3rd dpi resolution for the x axis.
	dpi_3_y			specify 3rd dpi resolution for the y axis.
	dpi_4_x			specify 4th dpi resolution for the x axis.
	dpi_4_y			specify 4th dpi resolution for the y axis.
	dpi_btn			specify whether +/- (on the upper side of the mouse) or 
				forwards/backwars (on the left side of the mouse) buttons act as dpi
				buttons. Valid values are dpi_btn_up and dpi_btn_side.
  For dpi_5 following options must be set
	dpi_1_x			specify 1st dpi resolution for the x axis.
	dpi_1_y			specify 1st dpi resolution for the y axis.
	dpi_2_x			specify 2nd dpi resolution for the x axis.
	dpi_2_y			specify 2nd dpi resolution for the y axis.
	dpi_3_x			specify 3rd dpi resolution for the x axis.
	dpi_3_y			specify 3rd dpi resolution for the y axis.
	dpi_4_x			specify 4th dpi resolution for the x axis.
	dpi_4_y			specify 4th dpi resolution for the y axis.
	dpi_5_x			specify 5th dpi resolution for the x axis.
	dpi_5_y			specify 5th dpi resolution for the y axis.

	dpi_btn			specify whether +/- (on the upper side of the mouse) or 
				forwards/backwars (on the left side of the mouse) buttons act as dpi
				buttons. Valid values are dpi_btn_up and dpi_btn_side.
Examples:
	./g500_control /dev/usb/hiddev0 --urr_default --as_default --dpi_default
		this corresponds to G500's factory settings with 500 USB report rate,
		angle snapping disabled and 3 dpi resolutions (400, 800 and 2000).
		dpi buttons are used to switch between the 3 dpi values.
	./g500_control /dev/usb/hiddev0 --urr_default --as_default --dpi_1 1600 1600
		here, only one dpi resolution (1600 dpi for both axes) is set. dpi buttons
		can be mapped to anything you want via xbindkeys or similar tools.
	./g500_control /dev/usb/hiddev0 --1000 --as_on --dpi_1 1600 1600
		same as above, but now we increase USB report rate from 500 to 1000 and
		enable angle snapping.
	./g500_control /dev/usb/hiddev0 --1000 --as_on --dpi_1 3200 4000
		same as above, but now we use different dpi resolutions for each axis.
		for the x axis it's 3200 and for the y axis it's 4000.
	./g500_control /dev/usb/hiddev0 --500 --as_off --dpi_3 800 800 1600 1600 5700 5700 dpi_btn_up
		here, we set 3 different dpi resolutions: 800, 1600 and 5700 (equal for both axes).
		The dpi buttons (+/-) are used to switch between these. Moreover, we explicitly set USB
		repeat rate to 500 and disable angle snapping
	./g500_control /dev/usb/hiddev0 --500 --as_off --dpi_3 800 800 1600 1600 5700 5700 dpi_btn_side
		same as above but here we use (<-/->) as dpi buttons.
	./g500_control /dev/usb/hiddev0 --333 --as_on --dpi_5 800 800 1600 1600 3200 3200 4800 4800 5700 5700 dpi_btn_up
		here, we set 5 different dpi resolutions: 800, 1600, 3200, 4800 and 5700
		(equal for both axes). The dpi buttons (+/-) are used to switch between these. 
		Moreover, we explicitly set USB repeat rate to 333 and enable angle snapping
	./g500_control /dev/usb/hiddev0 --333 --as_on --dpi_5 800 800 1600 1000 3200 3200 4800 4800 5700 5700 dpi_btn_up
		similar to the above, but the second resolution has now different values for the axes:
		1600 dpi for the x axis and 1000 dpi for the y axis.

















