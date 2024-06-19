build/Reverse src/Reverse.rev build/out.asm
if [ $? != 0 ] 
then 
exit 1
fi
fasm build/out.asm > /dev/null 
