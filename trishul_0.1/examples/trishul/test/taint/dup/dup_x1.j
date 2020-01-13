.class public trishul/test/taint/dup/dup_x1
.super trishul/test/taint/TaintTest

.method public <init>()V
   aload_0
   invokenonvirtual trishul/test/taint/TaintTest/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
   .limit stack 4
   .limit locals 4

   bipush	13
   bipush	17
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/taint(II)I
   
   ; Yields 17(127), 13(0), 17(127)
   dup_x1
   
   istore_1
   istore_2
   istore_3
   
   ldc	"dup_x1 1"
   iload_1
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V

   ldc	"dup_x1 2"
   iload_2
   bipush	0
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V

   ldc	"dup_x1 3"
   iload_3
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   return
.end method
