// Copyright (c) 2002-2005 Princeton University

package trishul_p;

import java.util.StringTokenizer;
import java.util.regex.*;

/** This class is a glorified regexp for representing types, which may
    be simple types or classes and may contain wildcards.  */

public class TypePattern
{
	private Taint taint;

	private boolean simple=false;
	public boolean isSimple() { return simple; }
	private String name="";
	private Pattern p;
	public boolean isPattern() { return p!=null; }
	public Pattern pattern() {return p;}
	private boolean v=false;
	public boolean isVoid() { return v; }
	private boolean allargs=false;
	public boolean isAllArgs() { return allargs; }

  	public TypePattern (String s, int taintType)
  	{
	    s = s.trim();

	    // Check if it contains a taint
	    int pos = s.indexOf ("@");
	    if (pos < 0)
	    {
    		name = s;
    	}
    	else
    	{
    		name  = s.substring (0, pos);
    		taint = new Taint (taintType, Integer.parseInt (s.substring (pos + 1)));
    	}

    	if (name.equals("boolean") |
			name.equals("byte") |
			name.equals("short") |
			name.equals("int") |
			name.equals("long") |
			name.equals("float") |
			name.equals("double") |
			name.equals("char"))
		{
			simple = true;
		}
		else if (name.equals(".."))
		{
			allargs = true;
		}
		else if (name.equals("void"))
		{
			v = true;
		}
		else if (name.indexOf("*") > -1)
		{
			p = Pattern.compile(name.replaceAll("\\.","/").replaceAll("\\*","\\.\\*"));
		}
	}

	public boolean hasTaint ()
	{
		return (taint != null);
	}

	public Taint getTaint ()
	{
		return taint;
	}

  	public boolean matchesString (String s)
  	{
	    if (isAllArgs()) return true;
	    if (isPattern()) return p.matcher(s.replaceAll("\\.","/")).matches();
	    if (name.equals(s)) return true;
	    return false;
  	}

  	public boolean matchesValue (Object value)
  	{
  		boolean match = matchesString (value.getClass ().getName ());
  		return match;
  	}

  	public String getTypeString ()
  	{
  		return name;
  	}

  	public String toString ()
  	{
  		if (taint != null) return name + "@" + taint.getTaint ();
  		return name;
  	}
}

