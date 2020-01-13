.class public trishul/test/taint/dup/dup2_x2_3
.super trishul/test/taint/TaintTest

.method public <init>()V
   aload_0
   invokenonvirtual trishul/test/taint/TaintTest/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
   .limit stack 8
   .limit locals 8
   
   lconst_0
   bipush	30
   invokestatic	trishul/test/taint/TaintTest/taint(JI)J

   bipush	2
   bipush	20
   invokestatic	trishul/test/taint/TaintTest/taint(II)I
   
   bipush	1
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/taint(II)I
   
   ; Now: l0(30), 2(20), 1(10)
   
   dup2_x2
   
   ; Now  2(20), 1(10), l0(30), 2(20), 1(10)
   
   istore 1
   istore 2
   lstore 3
   istore 5
   istore 6
   
   ldc		"dup2_x2_3 1"
   iload	1
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x2_3 2"
   iload	2
   bipush	20
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x2_3 3"
   lload	3
   bipush	30
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V
   
   ldc		"dup2_x2_3 4"
   iload	5
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x2_3 4"
   iload	6
   bipush	20
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V

   return
.end method
