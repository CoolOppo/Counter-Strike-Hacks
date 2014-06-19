#pragma once

#include "Required.h"
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>

namespace NetVars
{

enum PropType_t
{
	PT_CHARS = 0,
	PT_CHAR,
	PT_UCHAR,
	PT_BOOL,
	PT_FLOAT,
	PT_INT,
	PT_VECTOR,
	PT_QANGLE,
	PT_EHANDLE,
	PT_FLAGS,
	PT_ARRAYELEMENT,
	PT_ARRAYQUOTE,
	PT_NOPREFIX,
	PT_NOTYPE,
	PT_UNKNOWN,
};

struct CNTVar;
struct CNTClass
{
	CNTClass() : m_nClassID(-1), m_pszName(NULL), m_pTable(NULL), m_nBase(0), m_pParent(NULL), m_nPos(-1)  { }
	CNTClass* Analyze(CNTClass* pParent, RecvTable* pTable, int offset = 0, bool addtolist = true);
	CNTClass* Analyze(ClientClass* pClass);
	CNTClass* Find(const char* tablename, bool checklist = true);
	void ExportCode(std::ostream& out, int depth = 0);
	void ExportRaw(std::ostream& out, int depth = 0);
	std::string GetStructName(bool forcegendt = false);
	// If this was analyzed from a clientclass
	int m_nClassID;
	const char* m_pszName;
	// Table info
	RecvTable* m_pTable;
	int m_nBase;
	CNTClass* m_pParent;
	// Kinda hacky, position in the global list
	int m_nPos;
	// Analyzed sub classes and networked values ( represented by inlined funcs )
	std::vector< CNTClass* > m_Structs;
	std::vector< CNTVar* > m_Vars;
};
struct CNTVar
{
	CNTVar() : m_pProp(NULL), m_nAbsOffset(0), m_nArraysize(0), m_pParent(NULL), m_bBasicType(false), m_nType(PT_INT), m_pClassType(NULL) { }
	bool Analyze(CNTClass* pClass, RecvProp* pProp);
	PropType_t GenType(RecvProp* pProp);
	PropType_t GuessType(const char* var);
	bool ExportCode(std::ostream& out, int depth = 0);
	void ExportRaw(std::ostream& out, int depth = 0);
	bool GetTypeName(std::string& str, bool parentgendt = false);
	void GetVarName(std::string& str);
	void RawTypeName(std::string& str);
	// Prop stuff, stores some extra derivated stuff
	RecvProp* m_pProp;
	int m_nAbsOffset;
	int m_nArraysize;
	// CNTClass this belongs to
	CNTClass* m_pParent;
	// True if this is a PropType_t, false if this prop provides its own type
	bool m_bBasicType;
	PropType_t m_nType;
	CNTClass* m_pClassType;
};

// State
extern std::vector< CNTClass* > g_Classes;
CNTClass* FindGlobalClass(const char* name);
void AddGlobalClass(CNTClass* pClass);   // Assumes the class hasn't been added already!
void SolveDep();

void Analyze(ClientClass* pClasses);
void GenCode(const char* file);
void GenRaw(const char* file);
// void PrintEnt( C_BaseEntity *pEnt );

// Utilty funcs
inline bool strstart(const char* str, const char* pat)
{
	while(*pat && *str == *pat)
	{
		str++;
		pat++;
	}

	return !*pat;
}
};