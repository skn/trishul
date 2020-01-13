package trishul_p;

public class TaintPattern
{
	private String	taintName;
	private Taint	taint;
	private boolean	matchAll;
	private String	saveName;

	public TaintPattern (String taintName, boolean matchAll, String saveName)
	{
		this.taintName 	= taintName;
		this.matchAll	= matchAll;
		this.saveName	= saveName;
	}

	public TaintPattern (Taint taint, boolean matchAll, String saveName)
	{
		this.taint 		= taint;
		this.matchAll	= matchAll;
		this.saveName	= saveName;
	}

	public String getTaintName ()
	{
		return taintName;
	}

	public boolean hasTaint ()
	{
		return (taint != null);
	}

	public Taint getTaint ()
	{
		return taint;
	}

	public boolean matchAll ()
	{
		return matchAll;
	}

	public boolean hasSaveName ()
	{
		return (saveName != null);
	}

	public String getSaveName ()
	{
		return saveName;
	}

	public String toString ()
	{
		return "#<" + (hasTaint () ? getTaint ().toString () : getTaintName ())
					+ (matchAll () ? " & " : " | ")
					+ (hasSaveName () ? getSaveName () : "")
					+ ">";
	}
}
