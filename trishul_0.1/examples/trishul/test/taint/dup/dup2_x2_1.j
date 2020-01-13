.class public trishul/test/taint/dup/dup2_x2_1
.super trishul/test/taint/TaintTest

.method public <init>()V
   aload_0
   invokenonvirtual trishul/test/taint/TaintTest/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
   .limit stack 8
   .limit locals 8

   bipush	4
   bipush	40
   invokestatic	trishul/test/taint/TaintTest/taint(II)I

   bipush	3
   bipush	30
   invokestatic	trishul/test/taint/TaintTest/taint(II)I

   bipush	2
   bipush	20
   invokestatic	trishul/test/taint/TaintTest/taint(II)I
   
   bipush	1
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/taint(II)I
   
   ; Yields 2, 1, 4, 3, 2, 1
   dup2_x2 
   
   
   istore 1 
   istore 2
   istore 3
   istore 4
   istore 5
   istore 6
   
   ldc		"dup2_x2_1 1"
   iload	1
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x2_1 2"
   iload	2
   bipush	20
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x2_1 3"
   iload	3
   bipush	30
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x2_1 4"
   iload	4
   bipush	40
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x2_1 5"
   iload	5
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   
   ldc		"dup2_x2_1 6"
   iload	6
   bipush	20
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V

   return
.end method
