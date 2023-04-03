cmd_/home/hussam/Desktop/Device-Drivers/hello.mod := printf '%s\n'   hello.o | awk '!x[$$0]++ { print("/home/hussam/Desktop/Device-Drivers/"$$0) }' > /home/hussam/Desktop/Device-Drivers/hello.mod
