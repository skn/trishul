package trishul_p;

import java.security.*;

class EnginePolicyEmpty
extends java.security.Policy
{
	public void refresh ()
	{
	}

  	public PermissionCollection getPermissions(CodeSource codesource)
  	{
  		return new Permissions ();
  	}

}
