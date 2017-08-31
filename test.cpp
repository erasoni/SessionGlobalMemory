// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <process.h>
#include <cassert>

#include "SessionGlobalMemory.h"
using namespace Ambiesoft;



#if _MSC_VER < 1500
#define _countof(a) (sizeof(a)/sizeof(a[0]))
#endif 

#if _MSC < 1600
#define static_assert(s,t) assert(s)
#endif

struct Data {
	int d[1024];
	int checksum_;

	Data() {
		memset(d, 0, sizeof(d));
		checksum_ = 0;
		ctorcount++;
	}

	Data(const Data& data) {
		memcpy(d, &data.d, sizeof(d));
		checksum_ = data.checksum_;
		ctorcount++;
	}



	void setrand()
	{
		checksum_ = 0;
		for (int i = 0; i < _countof(d); ++i)
		{
			d[i] = rand();
			checksum_ += d[i];
		}
	}

	bool isChesksumOK()
	{
		int cs = 0;
		for (int i = 0; i < _countof(d); ++i)
		{
			cs += d[i];
		}
		assert(cs == checksum_);
		return checksum_ == cs;
	}
	static int ctorcount;
	static void print() {
		printf("ctorcount = %d\n", ctorcount);
	}
};
int Data::ctorcount;


template<class T>
CSessionGlobalMemory<T> getSGMEM(T initialvalue, LPCTSTR pName)
{
	CSessionGlobalMemory<T> ret(pName);
	ret = initialvalue;
	return ret;
}

void testreader(int span)
{
	CSessionGlobalMemory<Data> sgData("sgData");
	for (;;)
	{
		Data data;
		sgData.get(data);
		printf("Data Read:\t%d, %d, %d, ...\n", data.d[0], data.d[1], data.d[2]);
		printf("Checksum is %s.\n", data.isChesksumOK() ? "OK" : "NG");

		Sleep(span);
	}
}

void testwriter(int span)
{
	CSessionGlobalMemory<Data> sgData("sgData");
	for (;;)
	{
		Data data;
		data.setrand();
		sgData = data;
		printf("Data assigned:\t%d, %d, %d, ...\n", data.d[0], data.d[1], data.d[2]);
		printf("Checksum is %s.\n", data.isChesksumOK() ? "OK" : "NG");

		Sleep(span);
	}
}

void test2();
void test3();
int main(int argc, char* argv[])
{
	if (argc > 1 )
	{
		if (strcmp(argv[1], "test2") == 0)
		{
			test2();
			return 0;
		}
		else if (strcmp(argv[1], "test3") == 0)
		{
			test3();
			return 0;
		}
	}

	// test.exe -reader 100
	if (argc > 1)
	{
		
		int i= 1;
		// puts(argv[i]);
		if (_strcmpi(argv[i], "-reader") == 0)
		{
			SetConsoleTitleA("reader");			
			testreader(atoi(argv[i + 1]));
			return 0;
		}
		else if (_strcmpi(argv[i], "-writer") == 0)
		{
			SetConsoleTitleA("writer");
			testwriter(atoi(argv[i + 1]));
			return 0;
		}
		else
		{
			assert(false);
			return -1;
		}
	}

	srand((unsigned int)time(NULL));
	CSessionGlobalMemory<Data> sgData("sgData");
	CSessionGlobalMemoryNTS<Data> sgDataNTS("sgData");

	//CSessionGlobalMemory<Data> tmp(sgData);
	CSessionGlobalMemory<int> tmp2 (getSGMEM(3, "sgINT"));
	CSessionGlobalMemory<int> tmp3 (getSGMEM(3, "sgINTtmp"));
	tmp3=tmp2;
#if !defined(AMBIESOFT_NO_RVALUE_)
	tmp2=std::move(tmp3);
	tmp2=9999;
#endif
	std::string sa = "aaa";
	std::string sb;
#if !defined(AMBIESOFT_NO_RVALUE_)
	sb = std::move(sa);
#endif
	sb = "bbb";
	bool quit=false;
	for(; !quit;)
	{
		printf("\nPush 'a' to assign data, 'r' to read data, '0' to launch reader, '1' to launch writer,  and 'q' to quit.\n");
		switch(int ch=_getch())
		{
			case '0': case '1':
			{
				// http://www.cplusplus.com/forum/beginner/103383/
				char comspec[MAX_PATH]; comspec[0] = 0;
				size_t t;
				getenv_s(&t, comspec, "COMSPEC");
				_spawnl(_P_NOWAIT // flags
					, comspec   // cmd line
					, comspec   // arg[0] to main (of target)
					, "/c"      // arg[1]
					, "start"   // etc (see note above)
					, comspec
					, "/c"
					, argv[0]
					, ch == '0' ? "-reader" : "-writer"
					, "100"
					, NULL);
			}
			break;
			
			case 'a':
			{
				Data data;
				data.setrand();
				sgData = data;
				printf("Data assigned:\t%d, %d, %d, ...\n", data.d[0], data.d[1], data.d[2]);
				printf("Checksum is %s.\n", data.isChesksumOK() ? "OK":"NG");
			}
			break;
			case 'A':
			{
				Data data;
				data.d[0]=rand();
				data.d[1]=rand();
				data.d[2]=rand();
				memcpy(&sgDataNTS,&data,sizeof(data));
				printf("Data assigned:\t%d, %d, %d, ...\n", data.d[0], data.d[1], data.d[2]);
				printf("Checksum is %s.\n", data.isChesksumOK() ? "OK" : "NG");
			}
			break;

			case 'r':
			{
				Data data;
				sgData.get(data);
				
				printf("Data Read:\t%d, %d, %d, ...\n", data.d[0], data.d[1], data.d[2]);
				printf("Checksum is %s.\n", data.isChesksumOK() ? "OK" : "NG");
			}
			break;
			case 'R':
			{
				Data data;
				memcpy(&data, &sgDataNTS, sizeof(data));
				printf("Data Read:\t%d, %d, %d, ...\n", data.d[0], data.d[1], data.d[2]);
				printf("Checksum is %s.\n", data.isChesksumOK() ? "OK" : "NG");
			}
			break;


			case 'q':
			case 'Q':
			{
				quit = true;
				break;
			}
			break;

			default:
				break;
		}
	}


	return 0;
}



template<int LENGTH>
struct FixedLengthStruct{
	unsigned char data[LENGTH];
	int getLength() const {
		return LENGTH;
	}
};

struct VariableLengthStructBase {
protected:
	int length_;
	VariableLengthStructBase(){}
public:
	int getLength() const {
		return length_;
	}
};

template<int LENGTH>
struct VariableLengthStruct :VariableLengthStructBase {
public:
	unsigned char data[LENGTH];
	VariableLengthStruct() {
		length_ = LENGTH;
	}

	unsigned char* getData() {
		return data;
	}

};

void test2()
{
	static_assert(1000 == sizeof(FixedLengthStruct<1000>), "not 1000");
	assert(1000 == sizeof(FixedLengthStruct<1000>));

	CSessionGlobalMemory<FixedLengthStruct<1000> > sgData("thousandbyte");

	FixedLengthStruct<1000> data;
	sgData.get(data);
	sgData.getName();
}

void test3()
{
	// setter
	CDynamicSessionGlobalMemory sgDyn("dyn1000", 1000);
	unsigned char* p = (unsigned char*)malloc(1000);
	p[0] = 11;
	p[1] = 12;
	p[2] = 13;

	p[997] = 97;
	p[998] = 98;
	p[999] = 99;
	sgDyn.set(p);


	{
		// getter
		CDynamicSessionGlobalMemory sgDynUser("dyn1000");
		unsigned char* p = (unsigned char*)malloc(sgDynUser.size());
		sgDynUser.get(p);
		assert(p[0] == 11);
		assert(p[999] == 99);
	}
}