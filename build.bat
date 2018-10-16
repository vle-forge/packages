vle.exe -P vle.reader clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.tester clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.discrete-time clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.extension.celldevs clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.extension.decision clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.extension.dsdevs clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.extension.fsa clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.extension.petrinet clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.ode clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.ode_test clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.recursive clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.discrete-time.decision clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.reader_test clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.extension.cellqss clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.discrete-time_test clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.recursive_test clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.examples clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.tester_test clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.discrete-time.decision_test clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P ext.lua clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P ext.muparser clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P ext.qwt clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P ext.shapelib clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P vle.adaptative-qss.examples clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P gvle.decision clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P gvle.discrete-time clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P gvle.simulating.plan clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR
vle.exe -P gvle.forrester clean rclean configure build
if %ERRORLEVEL% NEQ 0 goto :ERROR

:EOF
echo Process completed
pause
exit 

:ERROR
echo Failure
pause
exit /b %ERRORLEVEL% 
