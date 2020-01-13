package trishul_p;

import java.security.*;
import java.security.Policy;
import java.util.Enumeration;

class ProtectionDomain
extends java.security.ProtectionDomain
{
	private Policy policy;

	private ProtectionDomain (java.security.ProtectionDomain assignedDomain, 
							  CodeSource codeSource, Policy policy)
	{
		super (codeSource, getPermissions (assignedDomain, policy.getPermissions (codeSource)), null, null);
	}

	private static PermissionCollection getPermissions (java.security.ProtectionDomain assignedDomain,
														PermissionCollection wanted)
	{
		if (assignedDomain != null)
		{
			// Check if all requested permissions are already granted
			PermissionCollection assigned = assignedDomain.getPermissions ();
			Enumeration enumPerm = wanted.elements ();
			while (enumPerm.hasMoreElements ())
			{
				Permission perm = (Permission) enumPerm.nextElement ();
				if (!assigned.implies (perm))
					throw new SecurityException ("Permission not granted to initiating policy: " + perm);
			}

		}
		return wanted;
	}
}
