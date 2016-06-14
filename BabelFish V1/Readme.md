# Babelfish V1

This version is Final Version and it contains following features:

1. The PDC_Port.vi will be run continuously as usual, except the PDC_Process_Dlg.exe will pop up automatically, that is user does have to run it independently from the PDC_Process_Dlg.exe folder.
2. Remember that the path of the PDC_Process_Dlg.exe must be correctly set! There is a box in the PDC_Port.vi that is the path of the PDC_Process_Dlg.exe and must be changed per its folder.(Attention!!)
3. No changes have been done to PDC_Process_Dlg.exe or the Module1.dll, and decodInf.dll.

## Software Required:

- Microsoft Visual Studio 2010 (Ultimate)
- Labview 2012 (or newer)

## Instructions to Install:

1. Download the source files on your local computer.
2. You will find the following folders:
  -  0-PDC-SW
  -  KTH software to check
3. Copy both folders under 'C:\'
4. Open three different instances of VC++ as administrator
5. Open in one of the VC++ instances:  'C:\0-PDC-SW\PDC Project 2010_DLLS\DecodInf\DecodInf.sln' and build it.
6. Open in a different VC++ instance: 'C:\0-PDC-SW\PDC Project 2010_DLLS\Module1\Module1.sln' and build it
7. Open in a third different  VC++ instance the file: 'C:\0-PDC-SW\PDC Project 2010_PROCESS\PDC_Process_Dlg\PDC_Process_Dlg.sln' and build it.
8. Go to the folder: 'C:\0-PDC-SW\PDC Project 2010_PROCESS\PDC_Process_Dlg\Debug' and right click on the file: 'PDC_Process_Dlg.exe'. 
Make sure that it is run under Windows 7 (option checked) and also that the option "to run it as administrator" is checked.
9. Open Labview as admin
10. Open the file: 'C:\KTH software to check\VI TUTORIAL PROJECTS\PDC_Port.vi'
11. Open the PDC simulator executable: PDCSimulator.exe
12. Uncheck: "Enable UDP Unicast"
13. Start the simulator
14. Run the Labview with the double arrow. You will see the VC++ dialog box, Click "Configuration Frame 2" and you can check in the PDCSimulator that it received a "send configuration frame 2"
15. Then you can follow the video for operating the Labview PDC_Port.vi


