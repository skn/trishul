package trishul.test.taint.other;

public class CreateObjectDemo
extends trishul.test.taint.TaintTest
{
    public static void main(String[] args) {

        Point originOne = new Point(23, 94);
        Rectangle rectOne = new Rectangle(originOne, taint(100,0x01), 200);
 		checkObjectTaint("rectOne object",rectOne,0x01);
		checkTaint("rectOne Area",rectOne.getArea(),0x01);
		checkTaint("rectOne",rectOne,0x00);

    	checkTaint ("fallback", getFallbackTaint (), 0);
    }
}
