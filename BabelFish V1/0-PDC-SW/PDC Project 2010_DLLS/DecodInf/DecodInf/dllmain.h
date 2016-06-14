// dllmain.h : Declaration of module class.

class CDecodInfModule : public ATL::CAtlDllModuleT< CDecodInfModule >
{
public :
	DECLARE_LIBID(LIBID_DecodInfLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_DECODINF, "{B0C6FA20-433D-4C46-855D-D998222A25B1}")
};

extern class CDecodInfModule _AtlModule;
