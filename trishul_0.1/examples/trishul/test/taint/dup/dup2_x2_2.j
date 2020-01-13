.class public trishul/test/taint/dup/dup2_x2_2
.super trishul/test/taint/TaintTest

.method public <init>()V
   aload_0
   invokenonvirtual trishul/test/taint/TaintTest/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
   .limit stack 8
   .limit locals 8

   bipush	3
   bipush	30
   invokestatic	trishul/test/taint/TaintTest/taint(II)I
   
   bipush	2
   bipush	20
   invokestatic	trishul/test/taint/TaintTest/taint(II)I
   
   lconst_1
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/taint(JI)J
   
   ; Now: 3(30), 2(20), l1(10)
   
   dup2_x2
   
   ; Now  l1(10), 3(30), 2(20), l1(10)
   
   lstore 1
   istore 3
   istore 4
   lstore 5
   
   ldc		"dup2_x2_2 1"
   lload	1
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V
   
   ldc		"dup2_x2_2 2"
   iload	3
   bipush	20
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x2_2 3"
   iload	4
   bipush	30
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x2_2 4"
   lload	5
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V

   return
.end method
