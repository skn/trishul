.class public trishul/test/taint/dup/dup2_2
.super trishul/test/taint/TaintTest

.method public <init>()V
   aload_0
   invokenonvirtual trishul/test/taint/TaintTest/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
   .limit stack 8
   .limit locals 4

   bipush	13
   bipush	14
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/taint(II)I
   
   ; Yields 13(0), 14(127), 13(0), 14(127)
   dup2
   
   istore 1
   istore 2
   istore 3
   istore 4
   
   ldc	"dup2_2 1"
   iload 	1
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc	"dup2_2 2"
   iload	2
   bipush	0
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V

   ldc	"dup2_2 3"
   iload 	3
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc	"dup2_2 4"
   iload	4
   bipush	0
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V

   return
.end method
