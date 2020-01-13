package trishul_p;

import java.net.URL;
import java.security.*;
import java.util.*;

public final class EnginePolicyURL
extends java.security.Policy
{
  	class Grant
  	{
		class Principal
		{
			String	type;
			String	param;

			Principal (String type, String param)
			{
				this.type 	= type;
				this.param 	= param;
			}
		}

  		private PermissionCollection		permissions = new Permissions ();
  		private URL							codeBase;
  		private HashSet<String>				signedBy;
  		private List<Principal>				principals;

  		Grant ()
  		{
  		}

  		void addPermission (Permission permission)
  		{
  			permissions.add (permission);
  		}

  		PermissionCollection getPermissions ()
  		{
  			return permissions;
  		}

		void addSigner (String signer)
		{
			if (signedBy == null)
				signedBy = new HashSet<String> ();
			signedBy.add (signer);
		}

		void setCodeBase (URL codeBase)
		{
			this.codeBase = codeBase;
		}

		URL getCodeBase ()
		{
			return codeBase;
		}

		void addPrincipal (String type, String param)
		{
			principals.add (new Principal (type, param));
		}
  	};

  	private final URL url;
  	private Map<CodeSource, PermissionCollection>	permissions = new HashMap<CodeSource, PermissionCollection> ();


  	public EnginePolicyURL (URL url)
  	{
    	this.url = url;
    	refresh ();
  	}

	/**
	 *
	 * @throws SecurityException if the policy cannot be loaded.
	 */
  	public void refresh()
  	{
  		try
  		{
  			new EnginePolicyParser (url, url.openStream (), this);
  		}
  		catch (Exception e)
  		{
  			throw new SecurityException ("When loading " + url + "\n" + e.getMessage (), e);
  		}
  	}

	public synchronized PermissionCollection getPermissions(CodeSource codesource)
	{
		Permissions permissions = new Permissions ();

		for (Map.Entry<CodeSource, PermissionCollection> entry: this.permissions.entrySet ())
		{
			if (entry.getKey ().implies (codesource))
			{
				merge (permissions, entry.getValue ());
			}
		}

		return permissions;
	}

	synchronized void addGrant (Grant grant)
	{
		// TODO: include signer certificates
		CodeSource codeSource = new CodeSource (grant.getCodeBase (), (java.security.cert.Certificate[]) null);

		if (!permissions.containsKey (codeSource))
			permissions.put (codeSource, grant.getPermissions ());
		else merge (permissions.get (codeSource), grant.getPermissions ());
	}

	private void merge (PermissionCollection target, PermissionCollection source)
	{
		for (Enumeration e =source.elements(); e.hasMoreElements();)
		{
			target.add ((Permission) e.nextElement());
     	}
	}
}
