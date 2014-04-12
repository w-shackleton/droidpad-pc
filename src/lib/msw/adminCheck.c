#include "adminCheck.h"

#include <stdio.h>
#include <lmcons.h>

// Taken from http://support.microsoft.com/kb/q118626/

/*-------------------------------------------------------------------------

  -
  isCurrentUserLocalAdministrator ()

  This function checks the token of the calling thread to see if the caller
  belongs to the Administrators group.

  Return Value:
  TRUE if the caller is an administrator on the local machine.
  Otherwise, FALSE.
  --------------------------------------------------------------------------*/
BOOL isCurrentUserLocalAdministrator(void)
{
	BOOL   fReturn         = FALSE;
	DWORD  dwStatus;
	DWORD  dwAccessMask;
	DWORD  dwAccessDesired;
	DWORD  dwACLSize;
	DWORD  dwStructureSize = sizeof(PRIVILEGE_SET);
	PACL   pACL            = NULL;
	PSID   psidAdmin       = NULL;

	HANDLE hToken              = NULL;
	HANDLE hImpersonationToken = NULL;

	PRIVILEGE_SET   ps;
	GENERIC_MAPPING GenericMapping;

	PSECURITY_DESCRIPTOR     psdAdmin           = NULL;
	SID_IDENTIFIER_AUTHORITY SystemSidAuthority = SECURITY_NT_AUTHORITY;


	/*
	   Determine if the current thread is running as a user that is a member of
	   the local admins group.  To do this, create a security descriptor that
	   has a DACL which has an ACE that allows only local aministrators access.
	   Then, call AccessCheck with the current thread's token and the security
	   descriptor.  It will say whether the user could access an object if it
	   had that security descriptor.  Note: you do not need to actually create
	   the object.  Just checking access against the security descriptor alone
	   will be sufficient.
	   */
	const DWORD ACCESS_READ  = 1;
	const DWORD ACCESS_WRITE = 2;


	{

		/*
		   AccessCheck() requires an impersonation token.  We first get a primary
		   token and then create a duplicate impersonation token.  The
		   impersonation token is not actually assigned to the thread, but is
		   used in the call to AccessCheck.  Thus, this function itself never
		   impersonates, but does use the identity of the thread.  If the thread
		   was impersonating already, this function uses that impersonation context.
		   */
		if (!OpenThreadToken(GetCurrentThread(), TOKEN_DUPLICATE|TOKEN_QUERY,
					TRUE, &hToken))
		{
			if (GetLastError() != ERROR_NO_TOKEN)
				goto cleanup;

			if (!OpenProcessToken(GetCurrentProcess(), 
						TOKEN_DUPLICATE|TOKEN_QUERY, &hToken))
				goto cleanup;
		}

		if (!DuplicateToken (hToken, SecurityImpersonation, 

					&hImpersonationToken))
			goto cleanup;


		/*
		   Create the binary representation of the well-known SID that
		   represents the local administrators group.  Then create the security
		   descriptor and DACL with an ACE that allows only local admins access.
		   After that, perform the access check.  This will determine whether
		   the current user is a local admin.
		   */
		if (!AllocateAndInitializeSid(&SystemSidAuthority, 2,
					SECURITY_BUILTIN_DOMAIN_RID,
					DOMAIN_ALIAS_RID_ADMINS,
					0, 0, 0, 0, 0, 0, &psidAdmin))
			goto cleanup;

		psdAdmin = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (psdAdmin == NULL)
			goto cleanup;

		if (!InitializeSecurityDescriptor(psdAdmin, 

					SECURITY_DESCRIPTOR_REVISION))
			goto cleanup;

		// Compute size needed for the ACL.
		dwACLSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) +
			GetLengthSid(psidAdmin) - sizeof(DWORD);

		pACL = (PACL)LocalAlloc(LPTR, dwACLSize);
		if (pACL == NULL)
			goto cleanup;

		if (!InitializeAcl(pACL, dwACLSize, ACL_REVISION2))
			goto cleanup;

		dwAccessMask= ACCESS_READ | ACCESS_WRITE;

		if (!AddAccessAllowedAce(pACL, ACL_REVISION2, dwAccessMask, 

					psidAdmin))
			goto cleanup;

		if (!SetSecurityDescriptorDacl(psdAdmin, TRUE, pACL, FALSE))
			goto cleanup;

		/*
		   AccessCheck validates a security descriptor somewhat; set the group
		   and owner so that enough of the security descriptor is filled out to
		   make AccessCheck happy.
		   */
		SetSecurityDescriptorGroup(psdAdmin, psidAdmin, FALSE);
		SetSecurityDescriptorOwner(psdAdmin, psidAdmin, FALSE);

		if (!IsValidSecurityDescriptor(psdAdmin))
			goto cleanup;

		dwAccessDesired = ACCESS_READ;

		/*
		   Initialize GenericMapping structure even though you
		   do not use generic rights.
		   */
		GenericMapping.GenericRead    = ACCESS_READ;
		GenericMapping.GenericWrite   = ACCESS_WRITE;
		GenericMapping.GenericExecute = 0;
		GenericMapping.GenericAll     = ACCESS_READ | ACCESS_WRITE;

		if (!AccessCheck(psdAdmin, hImpersonationToken, dwAccessDesired,
					&GenericMapping, &ps, &dwStructureSize, &dwStatus,
					&fReturn))
		{
			fReturn = FALSE;
			goto cleanup;
		}
	}
cleanup:
	{
		// Clean up.
		if (pACL) LocalFree(pACL);
		if (psdAdmin) LocalFree(psdAdmin);
		if (psidAdmin) FreeSid(psidAdmin);
		if (hImpersonationToken) CloseHandle (hImpersonationToken);
		if (hToken) CloseHandle (hToken);
	}

	return fReturn;
}

