passwords = open("common_passwords.txt", "r")
output = open("common_6_pwds.txt", "w")

# filter to 6 char passwords (7 - newline char) and output to new file
for password in passwords:
  if len(password) == 7:
    output.write("{}".format(password))

passwords.close()
output.close()
