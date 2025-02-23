#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdlib>

using namespace std;


fstream f_json;

enum TExpressionPart { epNone, epBrackets, epNumber, epValue, epText };
const string jsEmpty = " \t";
const string jsBrakets = "[]{}:,";
const string jsNumbers = "0123456789";
const string jsText = "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

const int MaxDeep = 50;
enum Tjson_item_type { jitArray, jitDict };
typedef struct {
	Tjson_item_type item_type;
	string item_name;
	int item;
} Tjson_item;

void menu() {
	cout << " -----------------------------------------------------------------------------" << endl;
	cout << "|  " << "MENU" << setw(71) << "  |" << endl;
	cout << " -----------------------------------------------------------------------------" << endl;
	cout << "|  " << setw(25) << "h - wyswetlenie helpa" << " | " << setw(37) << "o nazwa_pliku - otwarcie pliku o nazwa_pliku" << "  |" << endl;
	cout << " -----------------------------------------------------------------------------" << endl;
	cout << "|" << setw(74) << "c - sprawdzanie czy plik jest poprawnym formatenm JSON" << "  |" << endl;
	cout << " -----------------------------------------------------------------------------" << endl;
	cout << "|  " << setw(72) << "p sciezka_dostepu - pobieranie danych" << "  |" << endl;
	cout << " -----------------------------------------------------------------------------" << endl;

}

void help() {
	cout << "Na potrzeby projektu wykorzystano takie operacii: " << endl;
	cout << "-otwieranie pliku - biblioteka fstream, file.open(path, ios::app | ios::out | ios::in)" << endl;
	cout << "-sprawdzanie czy plik zostal otwarty - .is_open()" << endl;
	cout << "-pobieranie danych z pliku tekstowego - getline()" << endl;
	cout << "-sprawdzanie popranosci pliku:" << endl;
	cout << "  -podzial otrzymanej linijki na fragmenty, ich analiza (funkcji: .find(symbol); .size(); copy) " << endl;
	cout << "-stprzwdzanie poprawnosi JSONpath odbywa sie tak samo jak sprawdzanie poprawnosci pliku" << endl;
	cout << "Inne wykorzystane rzeczy: typedef; enum" << endl;
}

void error_out(int LinePos, int CursorPos, string ExprPartValue) {
	string ErrMessage;
	ErrMessage = "Unexpected Bracket '" + ExprPartValue;
	ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
	ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
	cout << endl << ErrMessage << endl;
}

bool CheckSymbol(string Symbols, char Symbol) {
	int Pos;
	Pos = Symbols.find(Symbol);
	if (Pos >= 0)
		return true;
	else
		return false;
}

bool get_next_part(TExpressionPart& ExprPartType, string& ExprPartValue, char*& charArray, int& SizeArray, bool ReadFile, int& LinePos, int& CursorPos, string& ErrMessage) {
	string line;
	bool flagPB = false;

	ExprPartType = epNone;
	ExprPartValue = "";
	if (charArray == NULL) {
		if (ReadFile) {
			if (getline(f_json, line)) {
				SizeArray = line.size() + 1;
				charArray = new char[SizeArray];
				copy(line.begin(), line.end(), charArray);
				charArray[line.size()] = '\0';
				CursorPos = 0;
				LinePos++;
			}
			else
				return true;
		}
		else
			return true;
	}

	while (true) {

		if (CursorPos + 1 >= SizeArray) {
			bool flagNL = false;
			if (ReadFile) {
				if (getline(f_json, line)) {
					SizeArray = line.size() + 1;
					charArray = new char[SizeArray];
					copy(line.begin(), line.end(), charArray);
					charArray[line.size()] = '\0';
					CursorPos = 0;
					LinePos++;
					flagNL = true;
				}
			}
			switch (ExprPartType)
			{
			case epNone:
				if (flagNL)
					break;
				else
					return true;
			case epNumber:
				return true;
			case epValue:
				return true;
			case epText:
				if (flagNL)
					break;
				else {
					ErrMessage = "Unexpected finish text :(";
					return false;
				}
			default:
				break;
			}
		}

		char rCh;
		rCh = charArray[CursorPos];
		switch (ExprPartType)
		{
		case epNone:
			if (CheckSymbol(jsEmpty, rCh)) {
				CursorPos++;
				continue;
			}
			if (CheckSymbol(jsBrakets, rCh)) {
				CursorPos++;
				ExprPartType = epBrackets;
				ExprPartValue = rCh;
				return true;
			}
			if (rCh == '.') {
				CursorPos++;
				ExprPartType = epNumber;
				flagPB = true;
				ExprPartValue = rCh;
				continue;
			}
			if (rCh == '-') {
				CursorPos++;
				ExprPartType = epNumber;
				ExprPartValue = rCh;
				continue;
			}
			if (rCh == '+') {
				CursorPos++;
				ExprPartType = epNumber;
				ExprPartValue = "";// rCh;
				continue;
			}
			if (CheckSymbol(jsNumbers, rCh)) {
				CursorPos++;
				ExprPartType = epNumber;
				ExprPartValue = rCh;
				continue;
			}
			if (CheckSymbol(jsText, rCh)) {
				CursorPos++;
				ExprPartType = epValue;
				ExprPartValue = rCh;
				continue;
			}
			if (rCh == '"') {
				CursorPos++;
				ExprPartType = epText;
				ExprPartValue = "";
				continue;
			}
			ErrMessage = "Unexpected symbol '" + rCh;
			ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
			ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
			return false;
		case epNumber:
			if (CheckSymbol(jsEmpty, rCh)) {
				return true;
			}
			if (CheckSymbol(jsBrakets, rCh)) {
				return true;
			}
			if (rCh == '.') {
				if (flagPB) {
					ErrMessage = "Unexpected symbol '" + rCh;
					ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
					ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
					return false;
				}
				ExprPartValue = ExprPartValue + rCh;
				CursorPos++;
				flagPB = true;
				continue;
			}
			if (CheckSymbol(jsNumbers, rCh)) {
				ExprPartValue = ExprPartValue + rCh;
				CursorPos++;
				continue;
			}
			ErrMessage = "Unexpected symbol '" + rCh;
			ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
			ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
			return false;
		case epValue:
			if (CheckSymbol(jsEmpty, rCh)) {
				return true;
			}
			if (CheckSymbol(jsBrakets, rCh)) {
				return true;
			}
			if (CheckSymbol(jsText, rCh)) {
				ExprPartValue = ExprPartValue + rCh;
				CursorPos++;
				continue;
			}
			if (CheckSymbol(jsNumbers, rCh)) {
				ExprPartValue = ExprPartValue + rCh;
				CursorPos++;
				continue;
			}
			ErrMessage = "Unexpected symbol '" + rCh;
			ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
			ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
			return false;
		case epText:
			if (rCh == '"') {
				CursorPos++;
				return true;
			}
			if (rCh == '\\') {
				CursorPos++;
				rCh = charArray[CursorPos];
				if (rCh == '"') {
					ExprPartValue = ExprPartValue + '"';
					CursorPos++;
					continue;
				}
				if (rCh == 't') {
					ExprPartValue = ExprPartValue + '\t';
					CursorPos++;
					continue;
				}
				if (rCh == 'T') {
					ExprPartValue = ExprPartValue + '\t';
					CursorPos++;
					continue;
				}
				if (rCh == '\\') {
					ExprPartValue = ExprPartValue + '\\';
					CursorPos++;
					continue;
				}
				ErrMessage = "Unexpected symbol '" + rCh;
				ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
				ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
				return false;
			}
			ExprPartValue = ExprPartValue + rCh;
			CursorPos++;
			continue;
		default:
			break;
		}
	}

	return false;
}

bool CheckPaths(Tjson_item json_path[MaxDeep], int size_json_path, Tjson_item command_path[MaxDeep], int size_command_path) {
	if (size_json_path == size_command_path) {
		for (int i = 0; i < size_json_path; i++) {
			if (json_path[i].item_type == command_path[i].item_type) {
				if (json_path[i].item_type == jitArray) {
					if (json_path[i].item != command_path[i].item)
						return false;
				}
				else if (json_path[i].item_type == jitDict) {
					if (json_path[i].item_name != command_path[i].item_name)
						return false;
				}
				else
					return false;
			}
			else
				return false;
		}
		return true;
	}
	else
		return false;
}

void Parce_JSON(string Command, Tjson_item command_path[MaxDeep], int size_command_path) {
	Tjson_item json_path[MaxDeep];
	int item_json_path = -1;
	TExpressionPart RunExprPartType = epNone;
	string RunExprPartValue = "";
	//string RunExprPartKey = "";

	if (f_json.is_open()) {
		f_json.clear();
		f_json.seekg(0, ios_base::beg);

		char* charArray = NULL;
		int LinePos = 0;
		int CursorPos = 0;
		int SizeArray = -1;
		while (true) {
			TExpressionPart ExprPartType;
			string ExprPartValue = "";
			string ErrMessage = "";
			if (get_next_part(ExprPartType, ExprPartValue, charArray, SizeArray, true, LinePos, CursorPos, ErrMessage)) {
				switch (ExprPartType)
				{
				case epNone:
					if (item_json_path < 0) {
						cout << endl << "JSON file is correct";
						cout << endl;
					}
					else {
						cout << endl << "Unexpected end of the file" << endl;
						return;
					}
					return;
				case epBrackets:
					if (ExprPartValue == "[") {
						if (RunExprPartType == epNone) {
							if (item_json_path + 1 < MaxDeep) {
								item_json_path++;
								json_path[item_json_path].item_type = jitArray;
								json_path[item_json_path].item_name = "";
								json_path[item_json_path].item = -1;
								RunExprPartType = epNone;
								RunExprPartValue = "";
							}
							else {
								cout << endl << "Overflow :(" << endl;
								return;
							}
						}
						else {
							error_out(LinePos, CursorPos, ExprPartValue);
							return;
						}
					}
					else if (ExprPartValue == "]") {
						if (item_json_path >= 0) {
							//if (RunExprPartType == epText or RunExprPartType == epNumber or RunExprPartType == epValue) {
							if (true) {
								if (json_path[item_json_path].item_type == jitArray) {
									json_path[item_json_path].item++;
									item_json_path--;
									RunExprPartType = epNone;
									RunExprPartValue = "";
								}
								else {
									error_out(LinePos, CursorPos, ExprPartValue);
									return;
								}
							}
							else {
								error_out(LinePos, CursorPos, ExprPartValue);
								return;
							}
						}
						else {
							error_out(LinePos, CursorPos, ExprPartValue);
							return;
						}
					}
					else if (ExprPartValue == "{") {
						if (RunExprPartType == epNone) {
							if (item_json_path + 1 < MaxDeep) {
								item_json_path++;
								json_path[item_json_path].item_type = jitDict;
								json_path[item_json_path].item_name = "";
								json_path[item_json_path].item = -1;
								RunExprPartType = epNone;
								RunExprPartValue = "";
							}
							else {
								cout << endl << "Overflow :(" << endl;
								return;
							}
						}
						else {
							error_out(LinePos, CursorPos, ExprPartValue);
							return;
						}
					}
					else if (ExprPartValue == "}") {
						if (item_json_path >= 0) {
							//if (RunExprPartType == epText or RunExprPartType == epNumber or RunExprPartType == epValue) {
							if (true) {
								if (json_path[item_json_path].item_type == jitDict) {
									item_json_path--;
									RunExprPartType = epNone;
									RunExprPartValue = "";
								}
								else {
									error_out(LinePos, CursorPos, ExprPartValue);
									return;
								}
							}
							else {
								error_out(LinePos, CursorPos, ExprPartValue);
								return;
							}
						}
						else {
							error_out(LinePos, CursorPos, ExprPartValue);
							return;
						}
					}
					else if (ExprPartValue == ":") {
						if (item_json_path >= 0) {
							if (RunExprPartType == epText) {
								if (json_path[item_json_path].item_type == jitDict and json_path[item_json_path].item_name == "") {
									json_path[item_json_path].item_name = RunExprPartValue;
									RunExprPartType = epNone;
									RunExprPartValue = "";
								}
								else {
									error_out(LinePos, CursorPos, ExprPartValue);
									return;
								}
							}
							else {
								error_out(LinePos, CursorPos, ExprPartValue);
								return;
							}
						}
						else {
							error_out(LinePos, CursorPos, ExprPartValue);
							return;
						}
					}
					else if (ExprPartValue == ",") {
						if (item_json_path >= 0) {
							//if (RunExprPartType == epText or RunExprPartType == epNumber or RunExprPartType == epValue) {
							if (true) {
								if (json_path[item_json_path].item_type == jitArray) {
									json_path[item_json_path].item++;

									if (Command == "show") {
										if (CheckPaths(json_path, item_json_path + 1, command_path, size_command_path)) {
											cout << endl << "Show" << endl;
											return;
										}
									}

									RunExprPartType = epNone;
									RunExprPartValue = "";
								}
								else if (json_path[item_json_path].item_type == jitDict) {
									json_path[item_json_path].item++;
									json_path[item_json_path].item_name = "";
									RunExprPartType = epNone;
									RunExprPartValue = "";
								}
								else {
									error_out(LinePos, CursorPos, ExprPartValue);
									return;
								}
							}
							else {
								error_out(LinePos, CursorPos, ExprPartValue);
								return;
							}
						}
						else {
							error_out(LinePos, CursorPos, ExprPartValue);
							return;
						}
					}
					else {
						error_out(LinePos, CursorPos, ExprPartValue);
						return;
					}
					break;
				case epNumber:
					if (RunExprPartType == epNone) {
						if (item_json_path >= 0) {
							RunExprPartType = epNumber;
							RunExprPartValue = ExprPartValue;
						}
						else {
							error_out(LinePos, CursorPos, ExprPartValue);
							return;
						}
					}
					else {
						error_out(LinePos, CursorPos, ExprPartValue);
						return;
					}
					break;
				case epValue:
					if (RunExprPartType == epNone) {
						if (item_json_path >= 0 and ((ExprPartValue == "true") or (ExprPartValue == "false" or ExprPartValue == "null"))) {
							RunExprPartType = epValue;
							RunExprPartValue = ExprPartValue;
						}
						else {
							error_out(LinePos, CursorPos, ExprPartValue);
							return;
						}
					}
					else {
						error_out(LinePos, CursorPos, ExprPartValue);
						return;
					}
					break;
				case epText:
					if (RunExprPartType == epNone) {
						if (item_json_path >= 0) {
							RunExprPartType = epText;
							RunExprPartValue = ExprPartValue;
						}
						else {
							ErrMessage = "Unexpected Text '" + ExprPartValue;
							ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
							ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
							cout << endl << ErrMessage << endl;
							return;
						}
					}
					else {
						ErrMessage = "Unexpected Text '" + ExprPartValue;
						ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
						ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
						cout << endl << ErrMessage << endl;
						return;
					}
					break;
				default:
					break;
				}
			}
			else
			{
				cout << endl << "Error ";
				cout << ErrMessage;
				cout << endl;
				return;
			}
		}
		cout << endl << "???";
		cout << endl;
	}
	else {
		cout << endl << "Plik nie jest otwarty" << endl;
	}

}

void Parce_CommandPath(string Path) {
	Tjson_item command_path[MaxDeep];
	int size_command_path = -1;

	TExpressionPart RunExprPartType = epNone;
	string RunExprPartValue = "";
	//string RunExprPartKey = "";

	int SizeArray;
	SizeArray = Path.size() + 1;
	char* charArray = new char[SizeArray];
	copy(Path.begin(), Path.end(), charArray);
	charArray[SizeArray - 1] = '\0';

	if ((SizeArray > 1) and (charArray[0] == '$')) {
		int CursorPos = 1;
		int LinePos = 1;

		while (true) {
			TExpressionPart ExprPartType;
			string ExprPartValue = "";
			string ErrMessage = "";
			if (get_next_part(ExprPartType, ExprPartValue, charArray, SizeArray, false, LinePos, CursorPos, ErrMessage)) {
				switch (ExprPartType)
				{
				case epNone:
					if ((RunExprPartType == epNone) and (size_command_path >= 0)) {
						cout << endl << "PATH file is correct";
						cout << endl;

						Parce_JSON("show", command_path, size_command_path + 1);
					}
					else {
						cout << endl << "Unexpected end of the command path" << endl;
						return;
					}
					return;
				case epBrackets:
					if (ExprPartValue == "[") {
						if (RunExprPartType == epNone) {
							if (size_command_path + 1 < MaxDeep) {
								size_command_path++;
								//json_path[item_json_path].item_type = jitArray;
								command_path[size_command_path].item_name = "";
								command_path[size_command_path].item = -1;
								RunExprPartType = ExprPartType;
								RunExprPartValue = ExprPartValue;
							}
							else {
								cout << endl << "Overflow :(" << endl;
								return;
							}
						}
						else {
							ErrMessage = "Unexpected Bracket '" + ExprPartValue;
							ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
							ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
							cout << endl << ErrMessage << endl;
							return;
						}
					}
					else if (ExprPartValue == "]") {
						if (size_command_path >= 0) {
							if (RunExprPartType == epText or RunExprPartType == epNumber) {
								RunExprPartType = epNone;
								RunExprPartValue = "";
							}
							else {
								ErrMessage = "Unexpected Bracket '" + ExprPartValue;
								ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
								ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
								cout << endl << ErrMessage << endl;
								return;
							}
						}
						else {
							ErrMessage = "Unexpected Bracket '" + ExprPartValue;
							ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
							ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
							cout << endl << ErrMessage << endl;
							return;
						}
					}
					else {
						ErrMessage = "Unexpected bracket '" + ExprPartValue;
						ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
						ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
						cout << endl << ErrMessage << endl;
						return;
					}
					break;
				case epNumber:
					if (RunExprPartType == epBrackets and RunExprPartValue == "[") {
						if (size_command_path >= 0) {
							if (CheckSymbol(ExprPartValue, '.')) {
								ErrMessage = "Unexpected float number '" + ExprPartValue;
								ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
								ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
								cout << endl << ErrMessage << endl;
								return;
							}
							RunExprPartType = epNumber;
							RunExprPartValue = ExprPartValue;
							command_path[size_command_path].item_type = jitArray;
							command_path[size_command_path].item_name = "";
							command_path[size_command_path].item = stoi(ExprPartValue);
						}
						else {
							ErrMessage = "Unexpected Number '" + ExprPartValue;
							ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
							ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
							cout << endl << ErrMessage << endl;
							return;
						}
					}
					else {
						ErrMessage = "Unexpected Number '" + ExprPartValue;
						ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
						ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
						cout << endl << ErrMessage << endl;
						return;
					}
					break;
				case epValue:
					ErrMessage = "Unexpected Value '" + ExprPartValue;
					ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
					ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
					cout << endl << ErrMessage << endl;
					return;
				case epText:
					if (RunExprPartType == epBrackets and RunExprPartValue == "[") {
						if (size_command_path >= 0) {
							RunExprPartType = epText;
							RunExprPartValue = ExprPartValue;
							command_path[size_command_path].item_type = jitDict;
							command_path[size_command_path].item_name = ExprPartValue;
							command_path[size_command_path].item = -1;
						}
						else {
							ErrMessage = "Unexpected Text '" + ExprPartValue;
							ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
							ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
							cout << endl << ErrMessage << endl;
							return;
						}
					}
					else {
						ErrMessage = "Unexpected Text '" + ExprPartValue;
						ErrMessage = ErrMessage + "' on line " + to_string(LinePos);
						ErrMessage = ErrMessage + "' at pos " + to_string(CursorPos);
						cout << endl << ErrMessage << endl;
						return;
					}
					break;
				default:
					break;
				}
			}
			else
			{
				cout << endl << "Error ";
				cout << ErrMessage;
				cout << endl;
				return;
			}
		}
		cout << endl << "???";
		cout << endl;
	}
	else {
		cout << endl << "Path is not correct" << endl;
	}

}

bool openFile(string path) {
	f_json.open(path, ios::app | ios::out | ios::in);
	if (!f_json.is_open()) {
		cout << "Error" << endl;
		return false;
	}
	else {
		cout << "Plik zostal otwarty" << endl;
		return true;
	}
}

void test() {
	if (f_json.is_open()) {
		f_json.clear();
		f_json.seekg(0, ios_base::beg);

		char* charArray = NULL;
		int LinePos = 0;
		int CursorPos = 0;
		int SizeArray = -1;
		while (true) {
			TExpressionPart ExprPartType;
			string ExprPartValue = "";
			string ErrMessage = "";
			if (get_next_part(ExprPartType, ExprPartValue, charArray, SizeArray, true, LinePos, CursorPos, ErrMessage)) {
				switch (ExprPartType)
				{
				case epNone:
					cout << endl << "Ok END";
					cout << endl;
					return;
				case epBrackets:
					cout << endl << "Ok Brakets ";
					cout << ExprPartValue;
					break;
				case epNumber:
					cout << endl << "Ok Number ";
					cout << ExprPartValue;
					break;
				case epValue:
					cout << endl << "Ok Value ";
					cout << ExprPartValue;
					break;
				case epText:
					cout << endl << "Ok Text ";
					cout << ExprPartValue;
					break;
				default:
					break;
				}
			}
			else
			{
				cout << endl << "Error ";
				cout << ErrMessage;
				cout << endl;
				return;
			}
		}
		cout << endl << "???";
		cout << endl;
	}
	else {
		cout << endl << "Plik nie jest otwarty" << endl;
	}

}

void Input(bool& Quit) {
	string path;
	char input = ' ';
	cin >> input;
	switch (input) {
	case 'H':
	case 'h':
		help();
		break;
	case 'O':
	case 'o':
		cin >> path;
		openFile(path);
		break;
	case 'C':
	case 'c':
		Parce_JSON("", NULL, -1);
		break;
	case 'T':
	case 't':
		test();
		break;
	case 'Q':
	case 'q':
		Quit = true;
		break;
	case 'P':
	case 'p':
		cin >> path;
		Parce_CommandPath(path);
		break;
	}
}
int main() {
	bool Quit = false;
	menu();
	while (!Quit)
		Input(Quit);
	return 0;
}
