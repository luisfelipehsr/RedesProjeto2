import re
import sys

file = open(sys.argv[1], "r")
lines = file.readlines()
file.close()
i = 0
total = 0
for line in lines:
    if re.search(r"Maximum delay", line) is not None:
        valueList = re.findall(r"[0-9]+", line)
        value = valueList[0]
        total += value
        i += 1

print(int(total / i))
