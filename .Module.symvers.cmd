cmd_/home/hussam/Desktop/Device-Drivers/Module.symvers := sed 's/ko$$/o/' /home/hussam/Desktop/Device-Drivers/modules.order | scripts/mod/modpost -m -a  -o /home/hussam/Desktop/Device-Drivers/Module.symvers -e -i Module.symvers   -T -