package trishul_p.compiler;

import japa.parser.JavaParser;
import japa.parser.ParseException;
import japa.parser.Token;
import japa.parser.ast.CompilationUnit;
import japa.parser.ast.visitor.DumpVisitor;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * This class compiles Polymer policies into standard Java sourcecode.
 */
public class Compile
{
	private static void parseException (String file, ParseException e)
	{
		Token token = e.currentToken;
		System.err.println (file + " (" + token.beginLine + ", " + token.beginColumn + "): " + e.getMessage ());
	}

	public static void main (String[] args)
	{
		if (args.length == 0)
		{
			System.err.println ("Usage: trishul_p.compiler.Compile <policy files>");
			System.exit (1);
		}

		for (int i = 0; i < args.length; i++)
		{
			try
			{
				String path = args[i];
				int pos = path.lastIndexOf ('.');
				String baseName = (pos < 0) ? path : path.substring (0, pos);
				String outPath  = baseName + ".java";
	    		File newFile = new File (outPath);
	    		newFile.delete ();

				// Parse it
				CompilationUnit cu = JavaParser.parse (path, new FileInputStream (path));

				// Transform it
				try
				{
					TrishulPVisitor visitor = new TrishulPVisitor ();
					cu = (CompilationUnit) visitor.visit (cu, null);
					if (visitor.haveErrors ())
						continue;
				}
				catch (ParseException pe) 	{throw pe;}
				catch (Exception e) 		{e.printStackTrace ();}

				// Write the output

	    		newFile.createNewFile ();
	    		FileOutputStream out = new FileOutputStream (newFile);
	    		out.write (cu.toString ().getBytes ());
			}
			catch (ParseException pe)
			{
				parseException (args[i], pe);
			}
			catch (IOException e)
			{
				System.err.println (e.getMessage ());
			}
		}
	}
}