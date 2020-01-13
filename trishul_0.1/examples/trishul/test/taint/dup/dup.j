.class public trishul/test/taint/dup/dup
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
   dup
   istore_1
   istore_2
   ldc	"dup 1"
   iload_1
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   ldc	"dup 2"
   iload_2
   bipush	127
   invokestatic	trishul/test/taint/TaintTest/checkTaint(Ljava/lang/String;II)V
   return
.end method
