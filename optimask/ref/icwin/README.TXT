ICED INSTALLATION INSTRUCTIONS

You should have:
* A CD labeled:  "IC LAYOUT AND VERIFICATION SOFTWARE" or the ICDIST.EXE file.
* A floppy disk labeled: "ICED LICENSE FILES"
* A HASP hardware key 

PART 1 - COPYING FILES

The CDROM (or ICDIST.EXE) contains copies of the ICED, DRC, and NLE/LVS software.  You can load any or all of them onto your hard disk.  However, you will only be able to run the programs you have licensed.  Licensing is controlled by the license files on the floppy disk.  The license files are named: ICED5AUX.EXE, DRC5AUX.EXE, and NLE5AUX.EXE.  You should have a license file for each license you purchased.

a) Load the CDROM in your drive and run Setup.exe.  If you downloaded ICDIST.EXE, execute that. 
   When setup finishes it will install an "ICED" icon on your desktop.

b) Load the floppy disk into drive A:.  Double click the "ICED" icon.  
   A DOS window will open.  At the DOS prompt type: 
	COPY A:\*.* <Enter>

PART 2 - LOADING THE HARDWARE KEY DRIVER

The first step is to see if a HASP driver has already been installed on your machine.  
At the DOS prompt type:  
	ICED5AUX <Enter>.   
A message box will appear.  If it says "HASP driver not installed." you must install the HASP driver.  Otherwise, skip to PART 3.
          
a) At the DOS prompt type:
	HDD32 <Enter>

b) If a choice is offered, choose a "Typical" installation.  

On Windows 98 machines, you may need to reboot at this point.  Try the test below, if it does not work the first time, reboot and try again.

PART 3 - FINAL TESTS

a) Plug the HASP hardware key into the appropriate port.  
   Parallel port keys can be put inline between your computer and your printer.
   USB keys can be plugged into any available USB port.

b) If you licensed the ICED layout editor type:
	ICED5AUX <Enter>
   Your computer should respond:
 	"Copy protection tests PASSED.   Serial number nnnn"

   If not, reboot and try again.  If it still does not work, you can call technical support or look at information on our website:
	http://www.iceditors.com/support/public/dongle-id.html

c) Similarly, if you licensed the DRC and/or the NLE/LVS type:
	DRC5AUX <Enter>
   and/or
	NLE5AUX <Enter>
   Again, your computer should respond:
	"Copy protection tests PASSED.   Serial number nnnn"
   If not, call technical support.

Installation is now complete.

LAUNCHING ICED PRODUCTS

We suggest double clicking the "ICED" icon whenever you have to open a DOS window to launch an ICED program.  The icon launches ICICON.EXE which sets certain environment variables, including the PATH.  Windows and ICED use these environment variables to help them find the correct files.

Read the information in the BASIC.PDF file found in the DOC subdirectory of the installation to learn how to open the editor and to use many of its features.  Read the other PDF files in this directory as needed.   If you are installing with ICDIST.EXE, not all of the manuals are included. You can download the others from our website http://www.iceditors.com.

CONTACTING TECHNICAL SUPPORT
In the USA: 1-800-933-4233
Outside the USA: +1 503 673 0013
Email: help@iceditors.com
