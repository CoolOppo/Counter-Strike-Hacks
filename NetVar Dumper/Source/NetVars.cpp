#include "NetVars.h"

namespace NetVars
{
std::vector< CNTClass* > g_Classes;

CNTClass* CNTClass::Analyze(ClientClass* pClass)
{
	CNTClass* ret =  Analyze(NULL, pClass->m_pRecvTable);
	return ret;
}

CNTClass* CNTClass::Analyze(CNTClass* pParent, RecvTable* pTable, int offset, bool addtolist)
{
	m_pTable = pTable;
	m_nBase = offset;
	m_pParent = pParent;
	// Check if this table has already been analyzed
	CNTClass* current = Find(m_pTable->GetName());

	if(current)
	{
		return current;
	}

	// Eventually add to global list
	if(addtolist)
	{
		m_nPos = g_Classes.size();
		AddGlobalClass(this);
	}

	// Analyze the props
	for(int i = 0; i < m_pTable->GetNumProps(); i++)
	{
		CNTVar* var = new CNTVar();

		if(var->Analyze(this, m_pTable->GetPropA(i)))
		{
			m_Vars.push_back(var);
		}
		else
		{
			delete var;
		}
	}

	return NULL;
}

CNTClass* CNTClass::Find(const char* tablename, bool checklist)
{
	// Check global list
	if(checklist)
	{
		CNTClass* top = FindGlobalClass(tablename);

		if(top)
		{
			return top;
		}
	}

	// Check local classes
	for(unsigned int i = 0; i < m_Structs.size(); i++)
	{
		if(!strcmp(tablename, m_Structs[i]->m_pTable->GetName()))
		{
			return m_Structs[i];
		}
	}

	// Check parent
	if(m_pParent)
	{
		return m_pParent->Find(tablename, false);
	}

	// Cannot find class
	return NULL;
}

void CNTClass::ExportCode(std::ostream& out, int depth)
{
	// Some basic formatting
	std::string pre("");

	for(int i = 0; i < depth; i++)
	{
		pre.append("\t");
	}

	// Variables we didn't understand (excludes baseclass and some stuff that is auto discarded)
	int failcount = 0;
	out << pre << "struct " << GetStructName(!depth);

	if(m_pParent && !depth)
	{
		out << " : public " << m_pParent->GetStructName(!depth);
	}

	out << "\n" << pre << "{\n";

	// Write the subclasses
	for(unsigned int i = 0; i < m_Structs.size(); i++)
	{
		m_Structs[i]->ExportCode(out, depth + 1);
	}

	// Write the variables
	for(unsigned int i = 0; i < m_Vars.size(); i++)
	{
		failcount += m_Vars[i]->ExportCode(out, depth + 1) ? 0 : 1;
	}

	// Write number of failed variables
	out << pre << "};";

	if(failcount)
	{
		out << " // Fail count = " << failcount;
	}

	out << "\n";
}

void CNTClass::ExportRaw(std::ostream& out, int depth)
{
	// Some basic formatting
	std::string pre("");

	for(int i = 0; i < depth; i++)
	{
		pre.append("\t");
	}

	// Name and info
	out << pre << "== TABLE : " << m_pTable->GetName() << " ==\n";

	for(unsigned int i = 0; i < m_Structs.size(); i++)
	{
		m_Structs[i]->ExportRaw(out, depth + 1);
	}

	out << pre << "-- props : " << m_pTable->GetNumProps() << " --\n";

	for(unsigned int i = 0; i < m_Vars.size(); i++)
	{
		m_Vars[i]->ExportRaw(out, depth + 1);
	}

	//out << pre << "== END : " << m_pTable->GetName() << " ==\n";
}

std::string CNTClass::GetStructName(bool forcegendt)
{
	std::string str;

	if(m_nClassID >= 0 || forcegendt)
	{
		str.assign("Gen");
		str.append(m_pTable->GetName());
	}
	else
	{
		const char* ptr = m_pTable->GetName();

		if(strstart(ptr, "m_"))
		{
			ptr += 2;
		}
		else if(strstart(ptr, "C"))
		{
			ptr += 1;
		}
		else if(strstart(ptr, "DT_"))
		{
			ptr += 3;
		}

		str.assign("C");
		str.append(ptr);
	}

	return str;
}

bool CNTVar::Analyze(CNTClass* pClass, RecvProp* pProp)
{
	// Store some stuff
	m_pProp = pProp;
	m_nAbsOffset = pClass->m_nBase + pProp->GetOffset();
	m_pParent = pClass;
	RecvTable* pSubTable = m_pProp->GetDataTable();

	// Check if we are a baseclass
	if(!strcmp(m_pProp->GetName(), "baseclass"))
	{
		// Check if we already analyzed it
		CNTClass* old = pClass->Find(pSubTable->GetName());

		if(!old)
		{
			// Analyze it
			CNTClass* ntclass = new CNTClass();
			ntclass->Analyze((pClass->m_pParent) ? pClass->m_pParent->m_pParent : NULL, pSubTable, m_nAbsOffset, true);
			// We found our parent
			old = ntclass;
		}

		pClass->m_pParent = old;
		// Do not add this to the list of funcs, this is implied through inheritance
		return false;
	}
	// Check if we have a sub table
	else if(pSubTable)
	{
		int numprops = pSubTable->GetNumProps();

		if(numprops >= 1 && !strcmp(pSubTable->GetPropA(0)->GetName(), "000") ||
				numprops >= 2 && !strcmp(pSubTable->GetPropA(1)->GetName(), "000"))
		{
			// This is actually a simple array
			m_bBasicType = true;
			m_nType = GenType(m_pProp);
			m_nArraysize = pSubTable->GetNumProps();
			return true;
		}
		else
		{
			CNTClass* sub = new CNTClass;
			CNTClass* prev = sub->Analyze(pClass, pSubTable, m_nAbsOffset, false);

			if(!prev)    // If it wasn't already analyzed
			{
				// Add a new struct to our parent
				pClass->m_Structs.push_back(sub);
				prev = sub;
			}
			else
			{
				delete sub;
			}

			m_bBasicType = false;
			m_pClassType = prev;
			return true;
		}
	}
	else
	{
		// Guess the type of this variable
		m_bBasicType = true;
		m_nType = GenType(m_pProp);
		int arraysize = m_pProp->GetNumElements();

		if(arraysize > 1)
		{
			m_nArraysize = arraysize;
		}

		return true;
	}
}

PropType_t CNTVar::GenType(RecvProp* pProp)
{
	PropType_t type = GuessType(pProp->GetName());

	if(type == PT_NOTYPE || type == PT_NOPREFIX || type == PT_UNKNOWN)
	{
		switch(pProp->GetType())
		{
		case DPT_Int:
			return PT_INT;

		case DPT_Float:
			return PT_FLOAT;

		case DPT_Vector:
			return PT_VECTOR;

		default:
			return PT_UNKNOWN;
		}
	}

	return type;
}

PropType_t CNTVar::GuessType(const char* var)
{
	if(strstr(var, "_array_element"))
	{
		return PT_ARRAYELEMENT;
	}
	else if(*var == '\"')
	{
		return PT_ARRAYQUOTE;
	}
	else if(!strstart(var, "m_"))
	{
		return PT_NOTYPE;
	}
	else if(strstart(var, "m_i") || strstart(var, "m_n"))
	{
		return PT_INT;
	}
	else if(strstart(var, "m_fl"))
	{
		return PT_FLOAT;
	}
	else if(strstart(var, "m_b"))
	{
		return PT_BOOL;
	}
	else if(strstart(var, "m_f"))
	{
		return PT_FLAGS;
	}
	else if(strstart(var, "m_ch"))
	{
		return PT_CHAR;
	}
	else if(strstart(var, "m_uch"))
	{
		return PT_UCHAR;
	}
	else if(strstart(var, "m_h"))
	{
		return PT_EHANDLE;
	}
	else if(strstart(var, "m_vec"))
	{
		return PT_VECTOR;
	}
	else if(strstart(var, "m_ang"))
	{
		return PT_QANGLE;
	}
	else if(strstart(var, "m_psz") || strstart(var, "m_sz") || strstart(var, "m_isz"))
	{
		return PT_CHARS;
	}
	else if(var[2] >= 'A' && var[2] <= 'Z')
	{
		return PT_NOPREFIX;
	}
	else
	{
		return PT_UNKNOWN;
	}
}

bool CNTVar::ExportCode(std::ostream& out, int depth)
{
	const char* pszName = m_pProp->GetName();
	// Dirty hack because the source engine likes us
	static const char* pszPrevName = NULL;
	static bool bCheckit = false;

	// Check if we can ignore this one
	if(bCheckit)
	{
		bCheckit = false;

		if(!strstart(pszPrevName, pszName) && strlen(pszPrevName) == strlen(pszName) + 3)
		{
			return true;  // Don't add to failcount
		}
	}

	// If this contains [0] the next var might be redundant garbage
	if(strstr(pszName, "[0]"))
	{
		bCheckit = true;
		pszPrevName = pszName;
	}

	// Don't make baseclass add to failcount
	if(!strcmp(pszName, "baseclass"))
	{
		return true;
	}

	std::string var, type;

	if(!GetTypeName(type, !depth))
	{
		return false;  // We couldn't figure out what type this is
	}

	GetVarName(var);

	// Some basic formatting
	for(int i = 0; i < depth; i++)
	{
		out << "\t";
	}

	out << "inline " << type << "* " << var << "() { return MakePointerGEN< " << type << " >( this, " << std::hex << "0x" << m_pProp->GetOffset() << " ); }" << std::dec;

	if(m_nArraysize > 0)
	{
		out << " // Array size = " << m_nArraysize;
	}

	out << "\n";
	return true;
}

void CNTVar::ExportRaw(std::ostream& out, int depth)
{
	// Some basic formatting
	for(int i = 0; i < depth; i++)
	{
		out << "\t";
	}

	// Name and info
	std::string type;
	RawTypeName(type);
	out << "Name = " << m_pProp->GetName() << " \tOffset = " << m_pProp->GetOffset() << " \tType = " << type << "\n";
}

bool CNTVar::GetTypeName(std::string& str, bool parentgendt)
{
	if(m_bBasicType)
	{
		switch(m_nType)
		{
		case PT_CHARS:
			str.assign("char*");
			return true;

		case PT_CHAR:
			str.assign("char");
			return true;

		case PT_UCHAR:
			str.assign("uchar");
			return true;

		case PT_BOOL:
			str.assign("bool");
			return true;

		case PT_FLOAT:
			str.assign("float");
			return true;

		case PT_INT:
			str.assign("int");
			return true;

		case PT_VECTOR:
			str.assign("Vector");
			return true;

		case PT_QANGLE:
			str.assign("QAngle");
			return true;

		case PT_EHANDLE:
			str.assign("EHANDLE");
			return true;

		case PT_FLAGS:
			str.assign("int");
			return true;

		default:
			return false;
		}
	}
	else
	{
		str.assign(m_pClassType->GetStructName(parentgendt));
		return true;
	}
}

void CNTVar::GetVarName(std::string& str)
{
	// Shut up
	char szName[256];
	const char* pszProp = m_pProp->GetName();

	if(strstart(pszProp, "m_"))
	{
		strcpy_s(szName, pszProp + 2);
	}
	else
	{
		strcpy_s(szName, pszProp);
	}

	char* ptr = szName;

	for(ptr = szName; *ptr; ptr++)
		if(*ptr == '.')
		{
			*ptr = '_';
		}

	ptr = strstr(szName, "[");

	if(ptr)
	{
		*ptr = *(ptr + 1);
		*(ptr + 1) = 0;
	}

	str.assign(szName);
}

void CNTVar::RawTypeName(std::string& str)
{
	switch(m_nType)
	{
	case PT_CHARS:
		str.assign("PT_CHARS");
		break;

	case PT_CHAR:
		str.assign("PT_CHAR");
		break;

	case PT_UCHAR:
		str.assign("PT_UCHAR");
		break;

	case PT_BOOL:
		str.assign("PT_BOOL");
		break;

	case PT_FLOAT:
		str.assign("PT_FLOAT");
		break;

	case PT_INT:
		str.assign("PT_INT");
		break;

	case PT_VECTOR:
		str.assign("PT_VECTOR");
		break;

	case PT_QANGLE:
		str.assign("PT_QANGLE");
		break;

	case PT_EHANDLE:
		str.assign("PT_EHANDLE");
		break;

	case PT_FLAGS:
		str.assign("PT_FLAGS");
		break;

	case PT_ARRAYELEMENT:
		str.assign("PT_ARRAYELEMENT");
		break;

	case PT_ARRAYQUOTE:
		str.assign("PT_ARRAYQUOTE");
		break;

	case PT_NOPREFIX:
		str.assign("PT_NOPREFIX");
		break;

	case PT_NOTYPE:
		str.assign("PT_NOTYPE");
		break;

	case PT_UNKNOWN:
		str.assign("PT_UNKNOWN");
		break;

	default:
		str.assign("you broke it");
		break;
	}
}

// State

CNTClass* FindGlobalClass(const char* name)
{
	for(unsigned int i = 0; i < g_Classes.size(); i++)
	{
		if(!strcmp(name, g_Classes[i]->m_pTable->GetName()))
		{
			return g_Classes[i];
		}
	}

	return NULL;
}

void AddGlobalClass(CNTClass* pClass)
{
	// I had more plans with this but that kinda failed
	g_Classes.push_back(pClass);
}

void SolveDep()
{
	bool solved = false;

	while(!solved)
	{
		solved = true;

		for(int i = 0; i < (int)g_Classes.size(); i++)
		{
			CNTClass* pParent = g_Classes[i]->m_pParent;

			if(pParent && pParent->m_nPos > i)
			{
				solved = false;

				// Move parent up til right before us
				for(int p = pParent->m_nPos; p > i; p--)
				{
					CNTClass* pTemp = g_Classes[p];
					g_Classes[p] = g_Classes[p - 1];
					g_Classes[p - 1] = pTemp;
					g_Classes[p]->m_nPos++;
					g_Classes[p - 1]->m_nPos--;
				}
			}
		}
	}
}

void Analyze(ClientClass* pClasses)
{
	//g_Classes.clear(); // erm this probably doesn't delete all the dynamically allocated CNTClasses...
	for(; pClasses; pClasses = pClasses->m_pNext)
	{
		CNTClass* pClass = new CNTClass();
		pClass->Analyze(pClasses);
	}
}

void GenCode(const char* file)
{
	std::ofstream out(file);
	out << "#pragma once\n";
	out << "#include <windows.h>\n";
	out << "template<typename T> inline T* MakePointerGEN( PVOID thisptr, int offset ) { return reinterpret_cast<T*>( reinterpret_cast<UINT_PTR>( thisptr ) + offset ); }\n";

	if(out.bad() || out.fail())
	{
		return;
	}

	// Solve dependency problems
	SolveDep();

	// The data tables
	for(unsigned int i = 0; i < g_Classes.size(); i++)
	{
		g_Classes[i]->ExportCode(out);
	}

	// Done :)
	out.close();
}

void GenRaw(const char* file)
{
	std::ofstream out(file);

	if(out.bad() || out.fail())
	{
		return;
	}

	// Start writing
	for(unsigned int i = 0; i < g_Classes.size(); i++)
	{
		g_Classes[i]->ExportRaw(out);
	}
}
};
