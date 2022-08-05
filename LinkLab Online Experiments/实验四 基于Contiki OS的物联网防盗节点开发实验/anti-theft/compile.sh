#!/bin/bash

# 执行编译指令
make TARGET=${BOARDTYPE}
exitcode=$?
exit $exitcode