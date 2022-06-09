#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <wchar.h>
#include <algorithm>
#include <iostream>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <filesystem>
#include <signal.h>

using namespace std;
namespace fs = filesystem;

string PATH = "/Users/jaikocher/Documents/Sem4/OS/Package/CompaniOS/Paths/";

// void clear_screen()
// {
//     pid_t pid = fork();
//     if (pid == 0)
//     {
//         execlp("clear", "clear", "", NULL);
//     }
//     return;
// }

int getModeCount()
{
    int modeCount = 0;
    for (const auto &f : fs::directory_iterator(PATH))
    {
        modeCount++;
    }

    return modeCount;
}

int terminateProcess(pid_t pid, int sig)
{
    int status = kill(pid, sig);
    return status;
}

pid_t runProcess(const char *app)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        printf("\033[1m\nSuccessfully Opened %s!\n", app);
        execlp("open", "open", app, NULL);
    }
    return pid;
}

void setUpMode(string modeName)
{
    char c;
    ifstream file;
    string line, appPath, path = PATH;
    path = path + modeName + ".txt";
    vector<string> appPaths;
    vector<pid_t> pids;
    file.open(path, ios::in);
    while (getline(file, line, '\n'))
    {
        appPaths.push_back(line);
    }
    file.close();

    for (int i = 0; i < appPaths.size(); i++)
    {
        pids.push_back(runProcess(appPaths[i].c_str()));
    }

    sleep(1);
    cout << "\033[1mEnter 0 To Exit: ";
    cin >> c;

    if (c == '0')
    {
        for (int i = 0; i < appPaths.size(); i++)
        {
            terminateProcess(pids[i], 0);
        }
    }
    return;
}

int addMode()
{
    string modeName, newModePath;
    bool exists;
    do
    {
        exists = false;
        cout << "\033[1mEnter The Name of The New Mode: ";
        cin >> modeName;
        if (modeName == "exit" || modeName == "EXIT")
        {
            return -1;
        }
        newModePath = PATH + modeName;
        newModePath += ".txt";

        for (const auto &f : fs::directory_iterator(PATH))
        {
            if (newModePath == f.path())
            {
                cout << "\033[1m\nFile Already Exists!\n\n";
                exists = true;
            }
        }
    } while (exists);

    string more;
    ofstream file(newModePath);

    do
    {
        string app;

        cout << "\033[1mEnter The Path of The Application(.app): ";
        cin >> app;

        file << app;

        cout << "\033[1m\nDo You Wish To Add More Applications? (Y/N) ";
        cin >> more;
        if (more == "Y" || more == "y")
        {
            file << "\n";
        }
    } while (more == "Y" || more == "y");

    file.close();
    cout << "\033[1m\n" << modeName << " Mode Has Been Successfully Added!\n";
    return 0;
}

int editMode()
{
    string modePath, modeName;
    bool exists = false;

    cout << "\033[1mEnter The Name of The Mode To Be Edited: ";
    cin >> modePath;
    modeName = modePath;
    if (modePath == "exit" || modePath == "EXIT")
    {
        return -1;
    }
    modePath = PATH + modePath + ".txt";

    // Check if mode already exists
    for (const auto &f : fs::directory_iterator(PATH))
    {
        if (modePath == f.path())
        {
            exists = true;
            break;
        }
    }
    if (!exists)
    {
        cout << "\033[1m\n" << modeName << " Was Not Found!\n";
        return -1;
    }

    string more;
    do
    {
        int c;
        cout << "\033[1m\n1. Add Application\n2. Remove Application\n3. Edit Application\n\n";
        cin >> c;
        switch (c)
        {
        case 1:
        {
            ofstream file;
            file.open(modePath, ios_base::app);
            string app;

            cout << "\033[1mEnter The Path of The New Application(.app): ";
            cin >> app;
            file << app;

            file.close();

            break;
        }
        case 2:
        {
            string app;
            ifstream fileRead(modePath);
            string line, substr;
            int j = 0;
            vector<string> values;

            cout << "\033[1mEnter The Path of The Application To Be Removed: ";
            cin >> app;

            while (getline(fileRead, line))
            {
                values.push_back(line);
            }

            fileRead.close();
            remove(modePath.c_str());

            ofstream fileWrite(modePath);
            bool firstLine = true;
            bool found = false;

            for (int i = 0; i < values.size(); i++)
            {
                if ((values[i] == app))
                {
                    found = true;
                    continue;
                }
                else
                {
                    values[i] = "\n" + values[i] + ",";
                    fileWrite << values[i];
                    fileWrite << values[i + 1];
                    firstLine = false;
                }
            }

            if (!found)
            {
                cout << "\033[1m" << app << " Was Not Found!\n";
            }

            fileWrite.close();

            break;
        }

        // Edit Application
        /*case 3:
        {
            string app, arg, newApp, newArg;
            ifstream file(modePath);
            string line, substr;
            int j = 0;
            vector<string> values;

            cout << "Enter the path of the application to be edited: ";
            cin >> app;
            cout << "Enter its current argument: ";
            cin >> arg;
            cout << app << " will be changed to: ";
            cin >> newApp;
            cout << arg << " will be changed to: ";
            cin >> newArg;
            newApp = "\"" + newApp + "\"";
            app = "\"" + app + "\"";
            if (newArg != "NULL")
            {
                newArg = "\"" + newArg + "\"";
            }
            if (arg != "NULL")
            {
                arg = "\"" + arg + "\"";
            }

            while (getline(fileRead, line))
            {
                stringstream ss(line);
                while (ss.good())
                {
                    getline(ss, substr, ',');
                    values.push_back(substr);
                }
            }

            fileRead.close();
            remove(modePath.c_str());

            ofstream fileWrite(modePath);
            bool found = false;

            for (int i = 0; i < values.size(); i = i + 2)
            {
                if ((values[i] == app) && (values[i + 1] == arg))
                {
                    values[i] = newApp;
                    values[i + 1] = newArg;
                    found = true;
                }

                if (i == 0)
                {
                    values[i] += ",";
                }
                else
                {
                    values[i] = "\n" + values[i] + ",";
                }

                fileWrite << values[i];
                fileWrite << values[i + 1];
            }

            fileWrite.close();

            if (!found)
            {
                cout << "The application " << app << " with argument " << arg << " was not found!\n";
            }

            break;
        }*/
        default:
        {
            cout << "\033[1mInvalid Option!\n";
            break;
        }
        }
        cout << "\033[1mDo You Wish To Make Any Other Changes? (Y/N) ";
        cin >> more;
    } while (more == "Y" || more == "y");

    return 0;
}

int deleteMode()
{
    string del_modePath, del_modeName;
    bool deleted = false;

    cout << "\033[1mEnter The Name of The Mode To Be Removed: ";
    cin >> del_modePath;
    del_modeName = del_modePath;
    if (del_modePath == "exit" || del_modePath == "EXIT")
    {
        return -1;
    }
    del_modePath = PATH + del_modePath + ".txt";

    // Check if mode already exists
    for (const auto &f : fs::directory_iterator(PATH))
    {
        if (del_modePath == f.path())
        {
            remove(del_modePath.c_str());
            deleted = true;
            break;
        }
    }
    if (!deleted)
    {
        cout << "\033[1m\n" << del_modeName << " Was Not Found!\n";
        return -1;
    }
    else
    {
        cout << "\033[1m" << del_modeName << " Mode Has Been Successfully Deleted!\n";
    }

    return 0;
}

void edit()
{
    int c = 5;
    while (c < 1 || c > 4)
    {
        cout << "\033[1mEdit Modes\n1. Add Mode\n2. Edit Mode\n3. Delete Mode\n4. Exit Editor\n";
        cin >> c;
        switch (c)
        {
        case 1:
            addMode();
            break;
        /*case 2:
            editMode();
            break;*/
        case 3:
            deleteMode();
            break;
        case 4:
            return;
        default:
            cout << "\033[1mInvalid option!\n";
        }
    }
    return;
}

int main()
{
    string choice;
    char *username;
    int noOfModes = getModeCount();
    vector<string> modes;

    // Getting the macOS username
    username = getpwuid(getuid())->pw_name;

    /*bool internet = InternetCheckConnection("https://google.com", FLAG_ICC_FORCE_CONNECTION, 0);
    if (internet)
    {
        cout << "\n\x1b[42;97mYou are connected to the internet!\x1b[0m\n";
    }
    else
    {
        cout << "\n\x1b[7;31mYou are NOT connected to the internet!\x1b[0m\n";
    }*/

    printf("\033[1m\nHey %s!\n\n", username);
    for (const auto &f : fs::directory_iterator(PATH))
    {
        char line[100];
        string modeName = f.path().u8string();
        size_t start = modeName.find("Paths/");
        size_t len = modeName.find(".txt");
        start += 6;
        len -= start;
        modeName = modeName.substr(start, len);
        modes.push_back(modeName);
    }
    
    do
    {
        printf("\033[1m\nMenu:\n\tModes:\n");
        for (int i = 0; i < modes.size(); i++)
        {
            printf("\033[1m\t\t- %s\n\n", modes[i].c_str());
        }
        printf("\033[1m\t- Edit Modes\n\t- Exit\n\n");
        printf("\033[1mEnter Your Choice: ");
        cin >> choice;
        if (find(modes.begin(), modes.end(), choice) != modes.end())
        {
            setUpMode(choice);
        }
        else if (choice == "Edit Modes" || choice == "edit modes" || choice == "Edit" || choice == "edit")
        {
            edit();
        }
        else if (choice != "Exit" && choice != "exit")
        {
            cout << "\033[1mInvalid Option!\n";
        }
        else
        {
            break;
        }
    } while (choice != "Exit" && choice != "exit");

    printf("\033[1m\n\n-------------------THANK YOU-------------------\n\n");

    return 0;
}