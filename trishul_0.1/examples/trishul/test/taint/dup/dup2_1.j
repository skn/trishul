.class public trishul/test/taint/dup/dup2_1
.super trishul/test/taint/TaintTest

.method public <init>()V
   aload_0
   invokenonvirtual trishul/test/taint/TaintTest/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
   .limit stack 8
   .limit locals 4

   lconst_1
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/taint(JI)J
   
   ; Yields l1(127), l1(127)
   dup2
   
   lstore 1
   lstore 3
   
   ldc	"dup2_1 1"
   lload 	1
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V
   
   ldc	"dup2_1 2"
   lload	3
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V
   return
.end method
