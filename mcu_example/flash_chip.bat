@echo off
SET FLASHFILE=%1
IF "%FLASHFILE%" == "" SET FLASHFILE="flash.bin"
SET FLASHDIR=%cd%
set FLASHDIR=%FLASHDIR:\=/%
set FLASHFILE=%FLASHFILE:\=/%

rem Using OpenOCD 0.7.0 JTAG:

rem cd C:\arm\openocd-0.7.0\openocd-0.7.0
rem bin\openocd-0.7.0.exe -f interface\olimex-arm-usb-tiny-h.cfg -f board\atmel_sam3s_ek.cfg -c "set FLASHDIR %FLASHDIR%" -c "set FLASHFILE %FLASHFILE%" -f %FLASHDIR%/flash_openocd.cfg

rem Using OpenOCD 0.9.0 JTAG:

cd C:\arm\openocd-0.9.0\
bin\openocd.exe -f interface\ftdi\olimex-arm-usb-tiny-h.cfg -f board\atmel_sam3s_ek.cfg -c "set FLASHDIR %FLASHDIR%" -c "set FLASHFILE %FLASHFILE%" -f %FLASHDIR%/flash_openocd.cfg

rem Using OpenOCD 0.9.0 SWD:

rem cd C:\arm\openocd-0.9.0\
rem bin\openocd.exe -f interface\ftdi\olimex-arm-usb-tiny-h.cfg -f interface\ftdi\olimex-arm-jtag-swd.cfg -f board\atmel_sam3s_ek.cfg -c "set FLASHDIR %FLASHDIR%" -c "set FLASHFILE %FLASHFILE%" -f %FLASHDIR%/flash_openocd.cfg