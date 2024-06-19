build/Reverse src/Reverse.rev build/out.asm
if [ $? = 0 ] 
then 
fasm build/out.asm > /dev/null 
fi
