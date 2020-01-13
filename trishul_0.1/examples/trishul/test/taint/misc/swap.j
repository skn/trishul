.class public trishul/test/taint/misc/swap
.super trishul/test/taint/TaintTest

.method public <init>()V
   aload_0
   invokenonvirtual trishul/test/taint/TaintTest/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
   .limit stack 4
   .limit locals 3

   bipush	17
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/taint(II)I
   
   ; Stack: 17(127)

   bipush	79
   bipush	13
   invokestatic	trishul/test/taint/TaintTest/taint(II)I

   ; Stack: 17(127), 79(13)
   swap

   ; Stack: 79(13), 17(127)
   
   istore_1
   istore_2

   ldc	"swap 1"
   iload_1
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc	"swap 2"
   iload_2
   bipush	13
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   return
.end method
