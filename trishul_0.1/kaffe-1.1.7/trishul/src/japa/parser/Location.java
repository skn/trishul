package japa.parser;

public class Location
{
	private	String 	file;
	private int		line;
	private int		column;

	public static final Location BUILTIN = new Location ("<builtin>", -1, -1);

	public Location (String file, int line, int column)
	{
		this.file	= file;
		this.line	= line;
		this.column	= column;
	}

	public String getFile ()
	{
		return file;
	}

	public int getLine ()
	{
		return line;
	}

	public int getColumn ()
	{
		return column;
	}
}
