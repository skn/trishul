.class public trishul/test/taint/dup/dup_x2_2
.super trishul/test/taint/TaintTest

.method public <init>()V
   aload_0
   invokenonvirtual trishul/test/taint/TaintTest/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
   .limit stack 8
   .limit locals 5

   lconst_1
   bipush	17
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/taint(II)I
   
   ; Yields 17(127), long 1(0), 17(127)
   dup_x2
   
   istore_1
   lstore_2
   istore 4
   
   ldc	"dup_x2 1"
   iload_1
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V

   ldc	"dup_x2 2"
   lload_2
   bipush	0
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;JI)V

   ldc	"dup_x2 3"
   iload 4
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   return
.end method
