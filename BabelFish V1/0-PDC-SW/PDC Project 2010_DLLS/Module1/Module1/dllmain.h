// dllmain.h : Declaration of module class.

class CModule1Module : public ATL::CAtlDllModuleT< CModule1Module >
{
public :
	DECLARE_LIBID(LIBID_Module1Lib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MODULE1, "{4245E949-38FB-4110-B7AA-06A0AEC888A1}")
};

extern class CModule1Module _AtlModule;
