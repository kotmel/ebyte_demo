param([String]$debugfile = "");

# This powershell file has been generated by the IAR Embedded Workbench
# C - SPY Debugger, as an aid to preparing a command line for running
# the cspybat command line utility using the appropriate settings.
#
# Note that this file is generated every time a new debug session
# is initialized, so you may want to move or rename the file before
# making changes.
#
# You can launch cspybat by typing Powershell.exe -File followed by the name of this batch file, followed
# by the name of the debug file (usually an ELF / DWARF or UBROF file).
#
# Read about available command line parameters in the C - SPY Debugging
# Guide. Hints about additional command line parameters that may be
# useful in specific cases :
#   --download_only   Downloads a code image without starting a debug
#                     session afterwards.
#   --silent          Omits the sign - on message.
#   --timeout         Limits the maximum allowed execution time.
#


if ($debugfile -eq "")
{
& "F:\01_JH_Software\IAR\Install_ForStm8\common\bin\cspybat" -f "E:\0_Ebyte_Product\0_Ebyte\E15\E15-EVB02\Software\2021-05-06-Project\0_Project\IAR_for_Stm8\ExMx_Demo\settings\project.E15-EVB02_E10-400M20S.general.xcl" --backend -f "E:\0_Ebyte_Product\0_Ebyte\E15\E15-EVB02\Software\2021-05-06-Project\0_Project\IAR_for_Stm8\ExMx_Demo\settings\project.E15-EVB02_E10-400M20S.driver.xcl" 
}
else
{
& "F:\01_JH_Software\IAR\Install_ForStm8\common\bin\cspybat" -f "E:\0_Ebyte_Product\0_Ebyte\E15\E15-EVB02\Software\2021-05-06-Project\0_Project\IAR_for_Stm8\ExMx_Demo\settings\project.E15-EVB02_E10-400M20S.general.xcl" --debug_file=$debugfile --backend -f "E:\0_Ebyte_Product\0_Ebyte\E15\E15-EVB02\Software\2021-05-06-Project\0_Project\IAR_for_Stm8\ExMx_Demo\settings\project.E15-EVB02_E10-400M20S.driver.xcl" 
}
