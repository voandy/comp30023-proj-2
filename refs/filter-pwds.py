passwords = open("10-million-passwords.txt", "r")
output = open("common_6_pwds.txt", "w+")

for password in passwords:
  if len(password) == 7:
    output.write("{}".format(password))

passwords.close()
