@echo off

REM echo Arg1 is %1
REM echo Arg2 is %2
REM echo Arg3 is %3
REM echo Arg4 is %4
REM echo Arg5 is %5

if not "%~5"=="" (
    REM echo Setting ENV var VCINSTALLDIR to %5 before calling...
	SET "VCINSTALLDIR=%~5"
	REM echo Setting ENV var VCINSTALLDIR to "%VCINSTALLDIR%" before calling...
)

REM "${OPENMITTSU_WINDEPLOYQT_EXE}" --dir \\\"${PROJECT_BINARY_DIR}/windeployqt\\\" $<TARGET_FILE:openMittsu>\""

set after1=%~1 %~2 "%~3" "%~4"
REM echo Calling %after1%

%after1%