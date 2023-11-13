.extern HandleTrap
.extern HandleTimer
.extern HandleConsole
.global supervisorTrap
.align 4
supervisorTrap:
j syscall
j timer
j empty
j empty
j empty
j empty
j empty
j empty
j empty
j console
sret


.global syscall

syscall:
addi sp,sp,-256
//ostavljamo prostor za 32 registra opste namene jer 32 * 8 = 256,svaki reg 8 bajtova

.irp index,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
sd x\index,\index * 8(sp)
.endr


call _ZN5Riscv10HandleTrapEv
.irp index,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
ld x\index,\index * 8(sp)
.endr

addi sp,sp,256

sret


.global timer

timer:
addi sp,sp,-256
//ostavljamo prostor za 32 registra opste namene jer 32 * 8 = 256,svaki reg 8 bajtova

.irp index,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
sd x\index,\index * 8(sp)
.endr


call _ZN5Riscv11HandleTimerEv
.irp index,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
ld x\index,\index * 8(sp)
.endr

addi sp,sp,256
sret

.global empty

empty:
sret


.global console

console:

addi sp,sp,-256
//ostavljamo prostor za 32 registra opste namene jer 32 * 8 = 256,svaki reg 8 bajtova

.irp index,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
sd x\index,\index * 8(sp)
.endr


call _ZN5Riscv13HandleConsoleEv
.irp index,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
ld x\index,\index * 8(sp)
.endr

addi sp,sp,256

sret