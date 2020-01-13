import java.io.File;
import java.util.Date;

public class DirectoryIndexOutput
extends StringBufferOutput
{
	private Server 			server;
	private File 			dir;
	private Request 		request;
	private Authenticator 	authenticator;

	DirectoryIndexOutput (Server server, Authenticator authenticator, File dir, Request request)
	{
		this.server	 = server;
		this.dir 	 = dir;
		this.request = request;
		this.authenticator	= authenticator;
	}

	Authenticator getAuthenticator ()
	{
		return authenticator;
	}

	void finish ()
	throws Exception
	{
		String[] list = dir.list ();

		sb.append ("<PRE><b><h2>Index of directory " + request.getPath () + " </h2></b>\n");
        sb.append ("<hr>\n");
        sb.append ("   <b><u>File</u>                    <u>Last-Modified</u>                  <u>Size</u></b>\n\n");
        //sb.append ("<img src = /images/back.gif>"+"  <A HREF ="+s+ " >" + "Parent Directory " + "</A>\n");

        for (int i = 0; i < list.length; i++)
        {
	        File thisfile = new File (dir, list[i]);
	        if (thisfile.isDirectory ())
		        sb.append ("<img src=" + server.getMIMEImage (thisfile) + ">"+"  <A HREF = " +  list[i]+ "/" +">"+list[i]+"</A>\n");
        }

        for (int i = 0; i < list.length; i++)
        {
            int j = list[i].length ();
            File thisfile = new File (dir, list[i]);
            if (thisfile.isFile ())
            {
           	    sb.append ("<img src=" + server.getMIMEImage (thisfile) + ">"+"  <A HREF = " + list[i]+">"+list[i]+"</A>");

	            while ( j++ < 20 )
	                sb.append(" ");

				String date = DateToString.getString (thisfile.lastModified());
    	        sb.append(date);
        	    j = date.length () ;

            	while (j++ < 35)
                	sb.append(" ");
            	sb.append (((thisfile.length()/1000) + 1) + " K\n");
        	}
        }
        sb.append("</PRE>\n");

        super.finish ();
	}

	String getMIMEType ()
	{
		return "text/html";
	}

	Date getLastModified ()
	{
		return new Date (dir.lastModified ());
	}
}
