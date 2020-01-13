package japa.parser.ast.expr;

import java.util.HashSet;
import trishul_p.Taint;

public class NamedTaint
extends Taint
{
	private HashSet<String>	taintNames;

    public NamedTaint (int taintType, HashSet<String> taintNames)
    {
        super (taintType, 0);
        this.taintNames = taintNames;
    }

    public HashSet<String> taintNames ()
    {
    	return taintNames;
    }

    public String toString ()
    {
 		String s = null;

 		for (String name : taintNames)
 		{
 			if (s == null) s = name;
 			else s = s + ", " + name;
 		}

		return TYPE_STRINGS[type] + ":{" + s + "}";
   }

}
