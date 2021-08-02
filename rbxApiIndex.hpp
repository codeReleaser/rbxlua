#define R_LUA_TNIL 0
#define R_LUA_TBOOLEAN 1
#define R_LUA_TVECTOR 2
#define R_LUA_TLIGHTUSERDATA 3
#define R_LUA_TNUMBER 4
#define R_LUA_TSTRING 5
#define R_LUA_TTABLE 6
#define R_LUA_TFUNCTION 7
#define R_LUA_TUSERDATA 8
#define R_LUA_TTHREAD 9
#define R_LUA_TPROTO 10
typedef union r_GCObject r_GCObject;



struct r_GCheader
{
    r_GCObject* next;
    byte marked;
    byte tt;
};

union r_GCObject
{
    r_GCheader gch;
    DWORD ts; /* tstring */
    DWORD u; /* userdata */
    DWORD cl; /* closure */
    DWORD h; /* table */
    DWORD p; /* pointer */
    DWORD uv; /* upvalue */
    DWORD th;  /* thread */
};

typedef union
{
    r_GCObject* gc;
    void* p;
    double n;
    int b;
} r_Value;

#define r_TValuefields    r_Value value; union { r_lua_TValue *upv; int flg; }; int tt

static uintptr_t states = 0;

namespace rbx
{
    static std::uintptr_t top = 12;
    static std::uintptr_t base = 20;

    typedef struct r_lua_TValue
    {
        r_Value value;
        int obj;
        int tt;
    } r_TValue;

    class api
    {
    public:
        uintptr_t r_state;
         uintptr_t pcall(int a2, int a3);
         rbx::r_TValue* index2adr(int idx);
          void getfield(int index, const char* g);
         void setfield(int index, const char* g);
         void VGettable(rbx::r_TValue* index1, rbx::r_TValue* index2, rbx::r_TValue* index3);
         void VSettable(rbx::r_TValue* index1, rbx::r_TValue* index2, rbx::r_TValue* index3);
         TString* NewString(const char* str);
         TString* NewLString(const char* str, std::size_t size);
         VOID GetGlobal(const char* g);
         std::intptr_t DPrecall(rbx::r_TValue* funcStack, int nresults);
         Closure* NewCClosure(int index, int idx2, int idx3);
         std::uintptr_t PushCClosure(int index, lua_CFunction idx2, int index3);
         void pushinterger(double n);
         void pushnumber(double n);
         void pushstring(const char* s);
         void pushnil();
         void pushlstring(const char* const str, std::size_t length);
         double* xorvalue();
        operator uintptr_t() const
        {
            return r_state;
        }
        rbx::api(std::uintptr_t rstate);
    };
}




namespace rbx
{
	uintptr_t rebase(uintptr_t address)
	{
		return address - 0x400000 + reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
	}

	TString* rbx::api::NewLString(const char* index, std::size_t str)
	{
		return ((TString * (__fastcall*)(int rstate, const char*, std::size_t size))rebase(0x16f2fe0))(r_state, index, str);
	}

	TString* rbx::api::NewString(const char* str)
	{
		return NewLString(str, sizeof(str));
	}


	rbx::r_TValue* rbx::api::index2adr(int idx)
	{
		return ((r_TValue * (__fastcall*)(int, int))rebase(0x16d2c90))(r_state, idx);
	}

	void rbx::api::VGettable(rbx::r_TValue* a1, rbx::r_TValue* a2, rbx::r_TValue* a3)
	{
		((void(__cdecl*)(int state, rbx::r_TValue * a1, rbx::r_TValue * a2, rbx::r_TValue * a3))rebase(0x16D3CC0))(r_state, a1, a2, a3);
	}

	void rbx::api::getfield(int index, const char* g)
	{
		r_StkId t;
		r_TValue key;
		t = index2adr(index);
		r_setsvalue(&key, NewString(g));
		std::cout << "t " << std::endl;
		api::VGettable(t, &key, reinterpret_cast<r_TValue*>(r_state + rbx::top));
		std::cout << "getfield " << std::endl;
		r_incr_top(r_state);
	}

	VOID api::GetGlobal(const char* g)
	{
		api::getfield(LUA_GLOBALSINDEX, g);
		std::cout << g << std::endl;
	}

	void rbx::api::VSettable(rbx::r_TValue* a1, rbx::r_TValue* a2, rbx::r_TValue* a3)
	{
		((void(__cdecl*)(int state, rbx::r_TValue * a1, rbx::r_TValue * a2, rbx::r_TValue * a3))rebase(0x16f3590))(r_state, a1, a2, a3);
	}

	void rbx::api::setfield(int index, const char* g)
	{
		r_StkId t;
		r_TValue key;
		t = index2adr(index);
		r_setsvalue(&key, NewString(g));
		VSettable(t, &key, *reinterpret_cast<r_TValue**>(r_state + rbx::top) - 1);
		*reinterpret_cast<std::uintptr_t*>(r_state + rbx::top--);  /* pop value */
	}


	std::intptr_t rbx::api::DPrecall(rbx::r_TValue* funcStack, int nresults)
	{
		return ((std::intptr_t(__cdecl*)(int, rbx::r_TValue*, int))rebase(0x16d41c0))(r_state, funcStack, nresults);
	}

	Closure* rbx::api::NewCClosure(int idx, int a2, int a3)
	{
		return ((Closure * (__cdecl*)(int, int, int, int))rebase(0x16F5A80))(r_state, idx, a2, a3);
	}

#define r_setobj(obj1,obj2) \
  { const rbx::r_TValue *o2=(obj2); rbx::r_TValue *o1=(obj1); \
    o1->value = o2->value; o1->tt=o2->tt; \
}


	std::uintptr_t rbx::api::PushCClosure(int idx, lua_CFunction a2, int a3)
	{
		Closure* cl;
		cl = NewCClosure(0, (int)a2, *reinterpret_cast<int*>(r_state + 16));
		cl->c.f = (lua_CFunction)a3;
		*reinterpret_cast<r_TValue**>(r_state + rbx::top) -= reinterpret_cast<std::intptr_t>(a2);
		while (a3--)
			r_incr_top(states);
	}

	void rbx::api::pushnil()
	{
		r_setnilvalue(*reinterpret_cast<rbx::r_TValue**>(r_state + rbx::top));
		r_incr_top(r_state);
	}

	void rbx::api::pushnumber(double n)
	{
		r_setnvalue(*reinterpret_cast<r_TValue**>(r_state + rbx::top), n);
		r_incr_top(r_state);
	}

	void rbx::api::pushinterger(double n)
	{
		r_setnvalue(*reinterpret_cast<r_TValue**>(r_state + rbx::top), n);
		r_incr_top(r_state);
	}

	void rbx::api::pushlstring(const char* s, std::size_t length)
	{
		r_setsvalue(reinterpret_cast<r_TValue*>(r_state + rbx::top), reinterpret_cast<r_GCObject*>(NewLString(s, length)));
		r_incr_top(r_state);
	}

	void rbx::api::pushstring(const char* const str)
	{
		if (str == NULL)
			pushnil();
		else
			pushlstring(str, sizeof(str));
	}

	uintptr_t rbx::api::pcall(int a2, int a3)
	{
		return ((int(__cdecl*)(int, int, int))ASLR(0x16de560))(r_state, a2, a3);
	}
}

#define adjustresults(rL,nres) \
    { if (nres == LUA_MULTRET && *reinterpret_cast<std::uintptr_t*>(rL + L_TOP) >= *reinterpret_cast<uintptr_t**>(*reinterpret_cast<std::uintptr_t*>(rL + L_TOP) + 24 + 16) *reinterpret_cast<uintptr_t**>(*reinterpret_cast<std::uintptr_t*>(rL + L_TOP) + 24 + 16) = reinterpret_cast<std::uintptr_t*>(rL + L_TOP); }


#define checkresults(L,na,nr) \
     api_check(L, (nr) == LUA_MULTRET || (L->ci->top - L->top >= (nr) - (na)))

using _DWORD = std::uintptr_t;
using _BYTE = BYTE;

rbx::api::api(std::uintptr_t rstate)
{
	r_state = rstate;
}
