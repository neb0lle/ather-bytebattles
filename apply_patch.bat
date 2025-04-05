@echo off

rem Apply the patch
git apply --reject --whitespace=fix patch

rem Remove the specified directories
@REM rd /s /q asdk-gen2\platform\cyt2b75\sdk\common\src\drivers\cxpi
@REM rd /s /q asdk-gen2\platform\cyt2b75\sdk\common\src\drivers\smif
@REM rd /s /q asdk-gen2\platform\cyt2b75\sdk\tviibe1m\hdr\rev_b
@REM rd /s /q asdk-gen2\platform\cyt2b75\sdk\tviibe1m\hdr\rev_c

echo Patch applied.