package trishul_p;

import java.security.*;
import java.security.Policy;
import java.util.Enumeration;

public class EngineSecurityManager
extends SecurityManager
{
	private PermissionCollection 	granted;
	private SecurityManager 		previous;

	private EngineSecurityManager (SecurityManager previous, CodeSource codeSource, Policy policy)
	{
		if (!(previous instanceof EngineSecurityManager))
		{
			this.previous = previous;
		}

		granted = mergePermissions (previous, policy.getPermissions (codeSource));
	}

	private static PermissionCollection mergePermissions (SecurityManager granted,
														  PermissionCollection wanted)
	{
		if (granted != null)
		{
			// Check if all requested permissions are already granted
			Enumeration enumPerm = wanted.elements ();
			while (enumPerm.hasMoreElements ())
			{
				Permission perm = (Permission) enumPerm.nextElement ();
				try
				{
					granted.checkPermission (perm);
				}
				catch (SecurityException e)
				{
					throw new SecurityException ("Permission not granted to initiating policy: " + perm);
				}
			}

		}
		return wanted;
	}

	public void checkPermission(Permission perm)
	{
		checkPermission (perm, null);
	}


	public void checkPermission (Permission perm, Object context)
	{
		if (!granted.implies (perm))
		{
			throw new SecurityException ("Not allowed: " + perm + "\nAssigned permissions:\n" + granted);
		}

		if (previous != null)
		{
			previous.checkPermission (perm, context);
		}
	}
}