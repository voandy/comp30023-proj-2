import operator

passwords = open("common_6_pwds.txt", "r")

# counts freq of each char in common passwords
charDict = {}
for password in passwords:
    for char in password:
        if char in charDict:
            charDict[char] += 1
        else:
            charDict[char] = 1

# sort by most frequent
sortedChars = {v: k for v, k in sorted(charDict.items(), key=lambda x: x[1], reverse = True)}.keys()

# output to files
alpha = open("freq_alpha.txt", "w")
num = open("freq_num.txt", "w")
spec = open("freq_spec.txt", "w")

for char in sortedChars:
    if char in " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~":
        spec.write(char)
    elif char in "0123456789":
        num.write(char)
    elif char in "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ":
        alpha.write(char)


passwords.close()
alpha.close()
num.close()
spec.close()
