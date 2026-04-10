@echo off
pushd %~dp0
cd ../
python scripts/setup.py
popd
pause