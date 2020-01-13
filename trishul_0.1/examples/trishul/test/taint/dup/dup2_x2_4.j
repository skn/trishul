.class public trishul/test/taint/dup/dup2_x2_4
.super trishul/test/taint/TaintTest

.method public <init>()V
   aload_0
   invokenonvirtual trishul/test/taint/TaintTest/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
   .limit stack 8
   .limit locals 8

   lconst_1
   bipush	20
   invokestatic	trishul/test/taint/TaintTest/taint(JI)J

   lconst_0
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/taint(JI)J

   ; Yields 0, 1, 0
   dup2_x2 
   
   
   lstore 1 
   lstore 3
   lstore 5
   
   ldc		"dup2_x2_4 1"
   lload	1
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V
   
   ldc		"dup2_x2_4 2"
   lload	3
   bipush	20
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V

   ldc		"dup2_x2_4 3"
   lload	5
   bipush	10
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V
   return
.end method
