#!/bin/bash
root=$(git rev-parse --show-toplevel)
cd $root/libs
grep -Rn '#include.*nlohman' &> /dev/null
if [ $? -ne 0 ]; then
	echo "grep failed:"
	grep -Rn '#include.*nlohman'
fi
grep -Rn '#include.*nlohmann' | grep -v "^libmod/src/mod/lib/IO/JsonUtils.hpp" &> /dev/null
if [ $? -eq 0 ]; then
	echo "nlohmann json(_schema) may only be included via ibmod/src/mod/lib/IO/JsonUtils.hpp"
	echo "Found these other instances:"
	grep -Rn '#include.*nlohmann' | grep -v "^libmod/src/mod/lib/IO/JsonUtils.hpp"
	exit 1
fi
