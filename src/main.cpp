#include "FSFunctions.hpp"

using namespace std;

string getCommand(string iString)
{
    string rCommand = "";

    for(int i  = 0; i < iString.length(); i++)
    {
        if(iString[i] != ' ')
            rCommand += iString[i];
        else if(iString[i] == ' ')
            return rCommand;
        
    }

    return "";

}

string getName(string iString)
{
    string rName = "";
    bool space = false;

    for (int i = 0; i < iString.length(); i++)
    {
        if (iString[i] == ' ')
            space = true;
        else if (space == true)
        {
            rName += iString[i]; 
        }
        
    }

    return rName;

}

string getFileName(string iString)
{
    string rName = "";
    int spaces = 0;

    for (int i = 0; i < iString.length(); i++)
    {
        if(iString[i] == ' ')
            spaces++;
        else if (spaces == 1)
            rName += iString[i];
        
    }

    return rName;
    
}

string getExportName(string iString)
{
    string rName = "";
    int spaces = 0;

    for (int i = 0; i < iString.length(); i++)
    {
        if(iString[i] == ' ')
            spaces++;
        else if (spaces == 2)
            rName += iString[i];
        
    }

    return rName;
    
}

int main() {

    bool salir = false;
    string input;

    FSFunctions functions;
    int actualDir = 0;

    while (salir == false)
    {
        string name = "", command = "", sActualDir = "";
        //cout << endl << actualDir;
        sActualDir = functions.getDirNameByIndex(actualDir);
        cout << "\n[" << sActualDir << "]" << ">";
        getline(cin, input);

        command = getCommand(input);
        name = getName(input);

        if(command == "vfs")
        {
            if (!functions.createdDisc())
            {
                MetaData md;
                int size = stoi(name);
                functions.fileSystemInit(md, size);
            }
            else
            {
                cout << "\nEl Disco ya esta creado!\n";
            }
            

        }
        else if(command == "cd" && name != "..")
        {
            char cName[25];
            int prev = actualDir;
            strcpy(cName, name.c_str());
            actualDir = functions.cd(cName, actualDir);
            if (actualDir == -1)
            {
                cout << "\n\t\tNombre de directorio no encontrado.";
                actualDir = prev;
            }
            
        }
        else if (command == "cd" && name == "..")
        {
            int prev = actualDir;
            iNodeEntry iActual = functions.get_iNodeByIndex(actualDir);
            actualDir = functions.inverseCD(iActual);
            if(actualDir == -1)
            {
                cout << "\n\t\tYa se encuentra en el directorio principal.";
                actualDir = prev;
            }
        }        
        else if(command == "mkdir")
        {
            char dirName[25];
            strcpy(dirName, name.c_str());
            functions.mkdir(actualDir, dirName);
        }
        else if(command == "ls")
            functions.ls(actualDir);
        else if(command == "import")
            functions.import(actualDir, name.c_str());
        else if(command == "export")
        {
            string fName = getFileName(input);
            string eName = getExportName(input);
            functions.exportFile(fName.c_str(), eName.c_str());
        }
        else if(input == "s" || input == "S")
            salir = true;
        else
            cout << "\nCommand not found\n";
        
    }  

}