package japa.parser.ast.visitor;

import japa.parser.Location;
import japa.parser.ast.*;
import japa.parser.ast.body.*;
import japa.parser.ast.expr.*;
import japa.parser.ast.stmt.*;
import japa.parser.ast.type.*;
import java.io.File;
import java.io.FileOutputStream;
import java.util.List;

/**
 * Generates a new sourcefile for each class in the compilation unit.
 */
public class GenerateSourcesVisitor
extends DumpVisitor
{
	private File 	rootDir;
	private String 	packageName;
	private List<ImportDeclaration>	imports;

	/// Directory including package name.
	private File	realOutputDir;

	/**
	 *
	 * @param rootDir The root directory for the generated output. The package name will be appended to this class.
	 */
	public GenerateSourcesVisitor (File rootDir)
	{
		this.rootDir = rootDir;
	}

    public void visit (CompilationUnit n, Object arg)
    throws Exception
    {
    	if (n.pakage == null)
    	{
    		realOutputDir = rootDir;
    	}
    	else
    	{
    		packageName = n.pakage.name;
    		realOutputDir = new File (rootDir, packageName.replace ('.', '/'));
    		realOutputDir.mkdirs (); // An error will be caught when the class output is generated
    	}

    	imports = n.imports;

    	super.visit (n, false);
    }

    public void visit (ClassOrInterfaceDeclaration n, Object arg)
    throws Exception
    {
    	// Nested class?
    	if (((Boolean) arg).booleanValue ())
    	{
    		super.visit (n, arg);
    	}
    	else
    	{
			printer.clear ();
			if (packageName != null)
				printer.println ("package " + packageName + ";\n");
			if (imports != null)
			{
				for (ImportDeclaration imp : imports)
					imp.accept (this, arg);
				printer.println ();
			}

    		super.visit (n, true);

    		File newFile = new File (realOutputDir, n.name + ".java");
    		newFile.delete ();
    		newFile.createNewFile ();
    		FileOutputStream out = new FileOutputStream (newFile);
    		String java = printer.getSource ();
    		out.write (java.getBytes ());
    	}

    }
}
