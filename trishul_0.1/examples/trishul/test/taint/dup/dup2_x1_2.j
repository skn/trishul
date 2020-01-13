.class public trishul/test/taint/dup/dup2_x1_2
.super trishul/test/taint/TaintTest

.method public <init>()V
   aload_0
   invokenonvirtual trishul/test/taint/TaintTest/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
   .limit stack 16
   .limit locals 8

   bipush	2
   
   lconst_1
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/taint(JI)J
   
   ; Now: 2(0), l1(10)
   
   dup2_x1
   
   ; Now  l1(10), 2(0), l1(10)
   
   lstore 1
   istore 3
   lstore 4
   
   ldc		"dup2_x1_2 1"
   lload	1
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V
   
   ldc		"dup2_x1_2 2"
   iload	3
   bipush	0
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x1_2 3"
   lload	4
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V

   return
.end method
