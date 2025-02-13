#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <windows.h>
#include <cstdio>
#include <iomanip>
#include <string.h>
#include <vector>

using namespace std;

const int NumberPrograms = 30; // Константа кол-во программ(Шорткатов)

string NameProgramm[NumberPrograms] = { // Массив строк, содержашие название программ для отображения пользователю*(шорткаты)
    "Google Chrome", "Notepad++", "Steam", "OBS Studio", "Epic Games Launcher", "Radmin VPN", "ShareX", "FACEIT Anti-Cheat", "qBittorrent", "Rufus", "AIDA64 Extreme",
    "Autoruns", "Discord", "Krita", "VS Code", "7-zip", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30"
};

string NameProgrammId[NumberPrograms] = { // Массив строк, содержашие ID Программ(шорткаты)
    "Google.Chrome", "Notepad++.Notepad++", "Valve.Steam", "OBSProject.OBSStudio", "EpicGames.EpicGamesLauncher", "Famatech.RadminVPN", "ShareX.ShareX", "FACEITLTD.FACEITAC",
    "qBittorrent.qBittorrent", "Rufus.Rufus", "FinalWire.AIDA64.Extreme", "Microsoft.Sysinternals.Autoruns", "Discord.Discord", "KDE.Krita", "Microsoft.VisualStudioCode",
    "7zip.7zip", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30"
};

string UserInput, InputTemp; // Строки для ввода пользователя; для временной строки в getline. Используются в UserInputForInstall , IfHelper.

bool IsRunningAsAdmin() { // Функция для проверки, запущена ли программа от имени администратора
    BOOL isAdmin = FALSE;
    PSID adminGroup;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    
    if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
                                 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    
    return isAdmin;
}

bool IsWingetAvailable() { // Функция для проверки наличия winget
    return (system("where winget >nul 2>nul") == 0);
}

void InstallWinget() { //Функция Установка Winget
    cout << "Winget не найден. Устанавливаем...\n";
    system("powershell.exe -Command \"Add-AppxPackage -RegisterByFamilyName -MainPackage Microsoft.DesktopAppInstaller_8wekyb3d8bbwe\"");
}

void UpdateWinget(){ // Функция обновления через MS Store 
    system("powershell.exe -Command \"Start-Process 'ms-windows-store://pdp/?productid=9NBLGGH4NNS1' -ArgumentList '-Wait'\"");
    cout << "Проверьте, обновлено ли приложение в Microsoft Store.\n";   
}

void CloseMSStore(){ // Функция убивания процесса MS Store
    system("taskkill /IM WinStore.App.exe /F >nul 2>nul");
}

// Скелеты функция для корректной работы IfHelper
void UpdateAllProgram();

void HelpForUsers();

void UninstallProgram();

void ProgramListShow();

void UserInputForInstall();

void ManualInstallation();

void IfHelper(){ // Функция переключения между вкладками/Функциями. Вынесена отдельна для работы со всех функций.
    if(UserInput == "Help" || UserInput == "help"){
        HelpForUsers();
        UserInputForInstall();
    }
    if(UserInput == "Update" || UserInput == "update" || UserInput == "Upgrade" || UserInput == "upgrade"){
        UpdateAllProgram();
        UserInputForInstall();
    }
    if(UserInput == "Delete" || UserInput == "delete" || UserInput == "Uninstall" || UserInput == "uninstall")
        UninstallProgram();
    if(UserInput == "Install" || UserInput == "install" || UserInput == "Add" || UserInput == "add")
        ManualInstallation();
    if(UserInput == "Exit" || UserInput == "exit"){
        UserInputForInstall();
    }
}

void ProgramListShow(){ // Функция для показа пользователю списка шорткатов программ
    cout << "Программы:\n";
        for (int i = 0; i < (NumberPrograms / 2); i++){ // Цикл для отображения строк
            int a = i + NumberPrograms / 2; // Переменная для 2 столбца
            if (i>=9){ // Для ровного отображения 2-х значных значений
                cout << "["<< i+1 << "] " << left << setw(49) << NameProgramm[i] // Вывод 1 стобика
             << "[" << a+1 << "] " << right << NameProgramm[a] << endl; // Вывод 2 столбика
            } else{
                cout << "["<< i+1 << "] " << left << setw(50) << NameProgramm[i] 
                << "[" << a+1 << "] " << right << NameProgramm[a] << endl;
            }
        }
}

void InstallProgramm(const string& NameProgrammId){ //Установка программ, которые выбрал user. (const string& NameProgrammId) - параметр функции
    string InsProg = "winget install --id " + NameProgrammId + " --silent --accept-package-agreements --accept-source-agreements"; // Запись в строку команды терминала
    system(InsProg.c_str()); // Вывод строки с переменной* в терминал, .c_str() - C строка
}

void UserInputForInstall(){ // Функция распознавания ввода команд или значений шорткатов пользователя.
    vector<int> InputNumber; //Динам.массив Int для хранения номеров программ, введенных пользователем. 
    while (true){ // Бесконечный цикл.
        system("cls");
        ProgramListShow();
        cout << "Введите команду(help - все команды) или номера программ через запятую(Например: 3,5,7):\n";
        getline(cin, UserInput); // Ввод пользователя.
        if(UserInput == "Exit") // Выход из программы
            break;
        IfHelper(); 
        stringstream ss(UserInput); // Создание потока из введенных чисел пользователя. Нужно для преобразования типа.
        InputNumber.clear(); // Отчистка вектора на случай повторного ввода.
        bool Error = false; // Нужно для перезапуска цикла в случае ошибки пользователя.
        try { // Обработка исключений. В случае ошибки отправляет в блок catch.
            while (getline(ss, InputTemp, ',')){ // Цикл для разбива ввода пользователя на числа.
                int TestLife = stoi(InputTemp); // Преобразование числа из строки в числовое значение.
                if  (TestLife > 0 && TestLife < NumberPrograms){ //Проверка валидности числа. && = и*
                    InputNumber.push_back(TestLife-1); // Запись числа в вектор, -1 для записи индекса.
                } else { 
                cout << "Ошибка. Значение " << TestLife << " вне диапозона программ для быстрой установки. Попробуйте снова.\n";
                Error = true;
            }
        }
        } catch (...){ // Обработка исключений.
            if (UserInput == "Update" || UserInput == "update" || UserInput == "Upgrade" || UserInput == "upgrade"|| "Help" || UserInput == "help" ||
            "Delete" || UserInput == "delete" || UserInput == "Uninstall" || UserInput == "uninstall" || "Install" || UserInput == "install" || UserInput == "Add" || UserInput == "add"){
                Error = true;
            } else {
            cout << "Некорректный ввод: " << InputTemp << endl;
            Error = true;
            }
        }
        for(int IndexInputNumber : InputNumber){ // перебирает элементы вектора. IndexInputNumber принимает значение каждого элемента из вектора InputNumber на каждой итерации цикла.
        InstallProgramm(NameProgrammId[IndexInputNumber]); // Установка программ, номера которых выбрал пользователь.
        }
    }
}

void UpdateAllProgram(){ // Функция для обновление всех программ
    system("cls");
    cout << "Обновляем установленные программы.\n" <<"Обновляются только программы, которые есть в репозитории Microsoft Winget\n\n";
    system("winget upgrade --all --silent --accept-package-agreements --accept-source-agreements");
    system("pause");
}

void HelpForUsers(){ // Список комманд
    system("cls");
    cout <<"Список комманд (Все другие имена в скобках являются аналогичными командами):\n" <<
    "Help - команда для запуска данного окна.\n" <<
    "Update(Upgrade) - команда для автообновления установленных программ из репозитория Winget.\n" <<
    "Delete(Uninstall) - команда для запуска удаление программ.\n" <<
    "Install(Add) - команда для ручной установки программ.\n"<<
    "Exit - команда выхода на главную страницу/Выхода из программы(Если вы на главной)\n\n" <<
    "Подробное описание работы Delete/Install будет указано при их выборе.\n";
    system("pause");
}

void UninstallProgram(){ // Функция удаление программ
    system("cls");
    cout << "Если возникает строка:\n" <<
        "*Обнаружено несколько установленных пакетов, соответствующих введенным условиям. Уточните условия.*\n" <<
        "Впишите полный ИД программы, которую надо удалить.(Будет указан)\n\n";
    while (true) { // Беск. цикл
        cout << "Введите команду или название программы в строку, которую хотите удалить(Пример: Chrome)\n";
        getline(cin, UserInput); //Ввод названия программы для удаления/команды
        IfHelper();
        string DelProgram = "winget uninstall "+  UserInput + " --silent --accept-source-agreements"; //Строка для терминала
        system(DelProgram.c_str()); //Вывод строки в терминал
    }
}

void ManualInstallation(){ // Функция ручной установки программ
    system("cls");
    while (true){ // Беск. цикл
        cout << "Впишите команду или название программы, которую хотите установить.\n";
        getline(cin, UserInput);
        IfHelper();
        cout << "Из данной таблицы(Если результаты найдены) найдите столбы 'Имя', 'ИД' и 'источник'\n" <<
        "Убедитесь, что источником является 'winget'.\n" <<
        "Выберит нужную программу в столбе 'Имя', впишите его ИД в строку.\n\n";
        string SearchProgram = "Winget search " +UserInput; // Строка для вывода таблицы найденных программ
        system(SearchProgram.c_str()); // Вывод в терминал строки
        cout << "\nВведите ИД программы:\n";
        string UserInstallInput;
        getline(cin, UserInstallInput);
        string InstallManualProgramm = "winget install --id " + UserInstallInput + " --silent --accept-package-agreements --accept-source-agreements";
        system(InstallManualProgramm.c_str());
    }
}

int main(){ //Основа
    setlocale(LC_ALL, "ru_RU.UTF-8"); //Кодировка,  setlocale(LC_ALL, "RU") - не работает.
    string answer, answer2; // Для ответа
    if (!IsRunningAsAdmin()) {  // Проверка прав администратора
        cout << "Этот скрипт требует запуска от имени администратора!\n";
        system("pause");
        return 1;
    }

    cout << "Проверка наличия winget...\n"; // Проверка наличия winget
    if (IsWingetAvailable() == false) {
        InstallWinget();
        if(IsWingetAvailable() == false){
            cout << "Ошибка установки Winget.\n";
            system("pause"); // Останавливает программу если winget не установился(почему-то)
            return 1;
        }
    } else { //Если установлена, запрашиваем обновление
        cout << "Winget установлен, но возможно, требуется обновить 'Установщий приложения' в Microsoft Store.\n"
        << "Открыть Microsoft Store?(yes/да)(Без обновления программа может не работать)\n";
        cin >> answer;
        if(answer == "y" || answer == "yes" || answer == "Да" || answer == "да" || answer == "Y" || answer == "Yes" || answer == "д" || answer == "Д"){ //Все Варианты ответа, || = или*
            UpdateWinget(); // Обновление
            cout << "Завершить процесс Microsoft Store?(yes/да)\n";
            cin >> answer2;
            if(answer2 == "y" || answer2 == "yes" || answer2 == "Да" || answer2 == "да" || answer2 == "Y" || answer2 == "Yes" || answer2 == "д" || answer2 == "Д"){ //Все Варианты ответа
                CloseMSStore(); // Смерть MS Store  
            }
        }
    }    

    cin.ignore(); // Для корректного отображения. Убирает символ новой строки ('\n') после последного ввода.
    UserInputForInstall(); // Запуск самой программы
    return 0;
} 
