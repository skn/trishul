package trishul.annotate;

import japa.parser.JavaParser;
import japa.parser.ParseException;
import japa.parser.Token;
import japa.parser.ast.CompilationUnit;
import japa.parser.ast.visitor.GenerateSourcesVisitor;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;

/**
 * Compiler that reads an annotation specification and creates java classes from it
 */
public class Annotate
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
			System.err.println ("Usage: trishul.annotate.Annotate [-d <output dir>] <annotation specifications>");
			System.exit (1);
		}

		int 	i;
		File 	outputDir = new File (".");

		for (i = 0; i < args.length; i++)
		{
			String arg = args[i];
			if (!arg.startsWith ("-"))
				break;

			boolean ok = false;

			if (arg.equals ("-d"))
			{
				if (i + 1 < args.length)
				{
					i++;
					outputDir = new File (args[i]);
					if (!outputDir.exists () 		||
						!outputDir.isDirectory ()	||
						!outputDir.canWrite	()		)
					{
						System.err.println ("Invalid output directory specified");
						System.exit (1);
					}

					ok = true;
				}
			}

			if (!ok)
			{
				System.err.println ("Usage: trishul.annotate.Annotate [-d <output dir>] <annotation specifications>");
				System.exit (1);
			}
		}

		for (; i < args.length; i++)
		{
			try
			{
				// Parse it
				CompilationUnit cu = JavaParser.parse (args[i], new FileInputStream (args[i]));

				try
				{
					// Transform it
					AnnotateVisitor annotate = new AnnotateVisitor ();
					cu = (CompilationUnit) annotate.visit (cu, null);
					if (annotate.haveErrors ())
						continue;

					// Write the outputs
					GenerateSourcesVisitor dump = new GenerateSourcesVisitor (outputDir);
					dump.visit (cu, null);
				}
				catch (Exception dumpEx)
				{
					System.err.println ("Unable to create output: " + dumpEx.getMessage ());
				}
			}
			catch (ParseException pe)
			{
				parseException (args[i], pe);
			}
			catch (FileNotFoundException e)
			{
				System.err.println (e.getMessage ());
			}
		}
	}
}
