#include "windows.h"
#include "iostream"
#include "vector"
#include "ctime"
#include "string"

using namespace std;

HANDLE handle;

typedef __int32 dataTypeOfSearchingValue;
typedef __int32* dataTypeOfAdress;

dataTypeOfSearchingValue buffer, whatWeLookFor; //buffer is value we find everytime we check adress, whatWeLookFor is what we want to change, change is to what we change
dataTypeOfAdress adress;
dataTypeOfAdress lastAdress;
struct ValAdress
{
	dataTypeOfSearchingValue value;
	dataTypeOfAdress adress;
};
struct SavedAdress
{
	ValAdress valAdress;
	string name;
};
vector<ValAdress> allMatches;
vector<SavedAdress> savedAdresses;

dataTypeOfAdress counter;

void secondSearch();

void menu()
{
	bool beInLoop = true;
	while (beInLoop)
	{
		//All matches
		cout << "MENU\n";
		cout << "All matches: " << allMatches.size() << '\n';

		//for (auto i : allMatches)
		//{
		//	cout << (PBYTE*)i.adress << '\n';
		//}

		int size = allMatches.size();
		cout << "What do u want now? ";
		if (size == 0)
			cout << "You have 0 matches: ";
		else if (size == 1)
			cout << "NICE, you have one match: ";
		else
			cout << "You have more than 1 matches: ";

		cout << "\nhint: input smth:\nc - change searching value and search again for better result,\ng - go to change value of found adresses\nd - display adress\ns - save adresses\no - if you wanna unsave all adresses\nl - load adresses\nn - new search\nx - exit\n: ";
		char c;

		int g;//tmp for input
		cin >> c;
		getchar();
		switch (c)
		{
		case 'c':
			cout << "Imput new value to search: ";
			cin >> g;
			getchar();
			whatWeLookFor = (dataTypeOfSearchingValue)g;
			adress = 0;
			secondSearch();
			break;
		case 'g':
			cout << "okay then put value you wanna change to: ";
			dataTypeOfSearchingValue change;
			cin >> g;
			getchar();
			change = (dataTypeOfSearchingValue)g;
			for (auto i : allMatches)
				WriteProcessMemory(handle, (LPVOID)i.adress, &change, sizeof(i.value), NULL);
			break;
		case 'd':
			cout << "All adresses:\n";
			for (auto i : allMatches)
				cout << (int*)i.adress << " value: " << (int)i.value << '\n';
			break;
		case 'o':
			savedAdresses.clear();
			break;
		case 's':
			for (auto i : allMatches)
				savedAdresses.push_back({ i,"" });
			if (savedAdresses.size() > 1)
			{
				cout << "All adresses you just saved:\n";
				int counterTmp = 0;
				for (auto i : savedAdresses)
				{
					cout << counterTmp << ": " << i.name << " " << (int*)i.valAdress.adress << '\n';
					++counterTmp;
				}
				while (true)
				{
					cout << "choose one you wanna name and -1 if u dont want: ";
					int choice;
					cin >> choice;
					getchar();
					if (choice == -1)
						break;
					cout << "Its name: ";
					char str[256];
					cin.getline(str, 256);
					savedAdresses.at(choice).name = str;
				}
			}
			if (savedAdresses.size() == 1)
			{
				cout << "All adresses you just saved:\n";
				for (auto i : savedAdresses)
					cout << i.name << " " << (int*)i.valAdress.adress << '\n';
				cout << "Do you wanna name it (y or n): ";
				char choice;
				cin >> choice;
				getchar();
				if (choice == 'n')
					break;
				cout << "Its name: ";
				char str[256];
				cin.getline(str, 256);
				savedAdresses.at(choice).name = str;
			}
			break;
		case 'l':
			if (!savedAdresses.empty())
			{
				cout << "All saved adresses:\n";
				int counterTmp = 0;
				for (auto i : savedAdresses)
				{
					cout << counterTmp << ": " << " " << (int*)i.valAdress.adress << " " << i.name << '\n';
					++counterTmp;
				}
				cout << "Choose one: ";
				unsigned int choice;
				cin >> choice;
				getchar();
				allMatches.clear();
				allMatches.push_back(savedAdresses.at(choice).valAdress);
			}
			else { cout << "There no saved adresses\n"; }
			break;
		case 'n':
			beInLoop = false;
			break;
		case 'x':
			cout << "Exiting\n";
			Sleep(2000);
			exit(0);
			break;
		}
	}
}

SYSTEM_INFO si;
bool stopping = false;
bool doDisplayFounds = false;
void search()
{
	cout << "SEARCH...\n";
	SIZE_T sizeOfBuffer = sizeof(buffer);

	while (true)
	{
		if (ReadProcessMemory(handle, adress, &buffer, sizeOfBuffer, NULL))
		{
			if (buffer == whatWeLookFor)
			{
				if (doDisplayFounds)
					cout << (int*)adress << " " << '\n';
				allMatches.push_back({ buffer, adress });
			}
			adress += 1;
			counter += 1;
		}
		else
		{
			adress += si.dwPageSize;
			counter += si.dwPageSize;
		}

		if (stopping)
			if (counter >= ((dataTypeOfAdress)0x10000000))
			{
				counter = 0;

				cout << "Matches found: " << allMatches.size() << "\nadress has limited the " << (PBYTE*)adress << " value: Menu? y == yes, n == no: \n";

				char c;
				cin >> c;
				getchar();
				if (c == 'y')
				{
					lastAdress = adress;
					break;
				}
			}

		if (adress > lastAdress)
		{
			counter = 0;

			cout << "All accessed adresses have passed\nGoing to menu\n";
			break;
		}
	}
}

void secondSearch()
{
	cout << "SECOND SEARCH\n";
	vector<ValAdress> secondSearchMatches;

	for (auto i : allMatches)
	{
		if (ReadProcessMemory(handle, i.adress, &buffer, sizeof(buffer), NULL))
		{
			if (buffer == whatWeLookFor)
			{
				secondSearchMatches.push_back(i);
			}
		}
	}

	allMatches = secondSearchMatches;

	//allMatches.clear();
	//for(auto i: secondSearchMatches)
	//	allMatches.
}

int main()
{
	//naming the app to be hacked
	//LPCSTR appName = "THE SETTLERS II - ﬁ¡»À≈…ÕŒ≈ »«ƒ¿Õ»≈ (11757)";

	std::string tmp;
	cout << "Your app name: ";
	getline(cin, tmp);

	LPCSTR appName = tmp.c_str();

	//info of system
	GetSystemInfo(&si);

	//getting the process of the window of the app to be hacked
	HWND hwnd;
	while (true)
	{
		hwnd = FindWindow(NULL, appName);
		if (hwnd == NULL)
		{
			std::cout << "err: cant find the window\n";
			cout << "To update press any button\n";
			getchar();
		}
		else
			break;
	}

	DWORD procId;
	GetWindowThreadProcessId(hwnd, &procId);
	handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
	if (handle == NULL)
	{
		std::cout << "cant open the process\n";
		Sleep(1500);
		return -1;
	}

	// just for fun we look here the level of the process protection idk if its worth sice i ve known not much about processes
	std::cout << "Process protection: ";
	PROCESS_PROTECTION_LEVEL_INFORMATION ppli;
	if (GetProcessInformation(handle, ProcessProtectionLevelInfo, &ppli, sizeof(ppli)))
	{
		std::cout << (int)ppli.ProtectionLevel << '\n';
	}
	else
		std::cout << "failed\n";

	// some process info
	/*APP_MEMORY_INFORMATION ami;
	GetProcessInformation(handle, ProcessAppMemoryInfo, &ami, sizeof(APP_MEMORY_INFORMATION));
	std::cout << "here: " << ami.AvailableCommit << '\n';*/

	//here we start searching for adress
	while (true)
	{
		adress = 0x00000000;
		lastAdress = (dataTypeOfAdress)0x7FFFFFFF;
		counter = 0;
		allMatches.clear();

		cout << "Input what we look for: ";
		int g;
		cin >> g; getchar();
		whatWeLookFor = (dataTypeOfSearchingValue)g;

		cout << "Should we stop every 0x10000000 adress pass? (cuz u can stop anytime or just wait until all is done)(y or n): ";
		char c;
		cin >> c;
		if (c == 'y')
			stopping = true;
		else
			stopping = false;

		if (stopping)
		{
			cout << "Should we display what we found during the process? (y or n): ";
			cin >> c;
			if (c == 'y')
				doDisplayFounds = true;
			else
				doDisplayFounds = false;
		}
		else doDisplayFounds = false;

		search();
		menu();
	}

	std::cout << "The End";
	getchar();

	return 0;
}