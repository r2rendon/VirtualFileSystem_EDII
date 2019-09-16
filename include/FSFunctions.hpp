#include <fstream>
#include <math.h>
#include "ImportantFSData.hpp"


class FSFunctions{

public:
    void fileSystemInit(MetaData md, int qty);
    string getDirNameByIndex(int index);
    iNodeEntry get_iNodeByIndex(int index);
    int getFirstFreeIndex();
    int get_LastBrother(int father);
    void rewrite_iNode(int index, iNodeEntry rw);
    void isOccupied();
    bool createdDisc();
    void create_iNodeEntry(iNodeEntry newEntry);
    iNodeEntry get_iNodeByName(const char* name);
    int getIndexBy_iNode(iNodeEntry iN);

    //Commands
    int cd(char directoryToChange[25], int actual);
    int inverseCD(iNodeEntry actualDir);
    int mkdir(int cdFather, char name[25]);//Returns an int refering to its position in the global entries
    void ls(int actualDir);
    bool rm(int actualDir, int sonDirToDelete);
    void import(int actualDir, const char *fileName);
    void exportFile(const char *originFile, const char *exportFile);

};

bool FSFunctions::createdDisc()
{
    ifstream fileSystem("fileSystem.recio", ios::in | ios::binary);
    if (!fileSystem)
        return false;
    else
        return true;
}

iNodeEntry FSFunctions::get_iNodeByName(const char* name)
{
    ifstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);

    MetaData md;
    fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));
    iNodeEntry iRead;
    for (int i = 0; i < md.totalEntries; i++)
    {
        fileSystem.read(reinterpret_cast<char*>(&iRead), sizeof(iRead));
        if (strcmp(iRead.fileName, name) == 0)
            return iRead;
        
    }

    return iRead;

}

int FSFunctions::getIndexBy_iNode(iNodeEntry iN)
{
    ifstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);

    MetaData md;
    fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));
    iNodeEntry iRead;
    for (int i = 0; i < md.totalEntries; i++)
    {
        fileSystem.read(reinterpret_cast<char*>(&iRead), sizeof(iRead));
        if (strcmp(iRead.fileName, iN.fileName) == 0)
            return i;
        
    }

    return -1;;
}

int FSFunctions::getFirstFreeIndex()
{
    ifstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);

    MetaData md;
    fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));
    iNodeEntry iRead;
    for (int i = 0; i < md.totalEntries; i++)
    {
        fileSystem.read(reinterpret_cast<char*>(&iRead), sizeof(iRead));
        if (!iRead.occupied)
        {
            fileSystem.close();
            return i;
        }
        
    }

    fileSystem.close();
    return -1;
    
}

void FSFunctions::create_iNodeEntry(iNodeEntry newEntry)
{
    fstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);

    fileSystem.seekg(0, ios::beg);
    MetaData md;
    fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

    iNodeEntry iRead;

    for (int i = 0; i < md.totalEntries; i++)
    {
        fileSystem.read(reinterpret_cast<char *>(&iRead), sizeof(iNodeEntry));
        if (!iRead.occupied)
        {
            fileSystem.seekp(ios::beg+sizeof(MetaData)+sizeof(bm)+(sizeof(iNodeEntry)*i));
            fileSystem.write(reinterpret_cast<const char*>(&newEntry), sizeof(iNodeEntry));
            return;
        }
        
    }
    

}

void FSFunctions::fileSystemInit(MetaData md, int qty)
{
    ofstream fileSystem("fileSystem.recio", ios::out | ios::binary | ios::app);

    if (!fileSystem)
    {
        cout << "\nThe FileSystem could not be opened!";
        return;
    }

    md.systemSize = (sizeof(iNodeEntry)*qty)+sizeof(md);
    md.totalEntries = (sizeof(iNodeEntry));
    md.totalDirectBlocks = CANTIDAD_DE_BLOQUES_INODE*qty;
    strcpy(md.fsName, "File System: Recio");
    //Write metadata
    fileSystem.write(reinterpret_cast<const char *>(&md), sizeof(MetaData));
    //Bitmap
    BitMap bm(md.totalDirectBlocks);
    fileSystem.write(reinterpret_cast<const char*>(&bm), sizeof(bm));
    //Directory Entries
    iNodeEntry entries;

    iNodeEntry first;
    strcpy(first.fileName, "/"); first.fileType = 'D'; first.occupied = true;
    
    for (int i = 0; i < qty; i++)
    {
        if(i == 0)
            fileSystem.write(reinterpret_cast<const char *>(&first), sizeof(iNodeEntry));
        else
            fileSystem.write(reinterpret_cast<const char *>(&entries), sizeof(iNodeEntry));     
    }

    IDB * iDataBlock = new IDB;
    for (int i = 0; i < qty; i++)
        fileSystem.write(reinterpret_cast<const char*>(iDataBlock), sizeof(IDB));

    cout << "\n\nSe escribio en el archivo.";
    fileSystem.close();

}

string FSFunctions::getDirNameByIndex(int index)
{
    string rString = "";

    ifstream fileSystem("fileSystem.recio", ios::in | ios::binary);
    fileSystem.seekg(0, ios::beg);
    MetaData lecturaMD;
    fileSystem.read(reinterpret_cast<char *>(&lecturaMD), sizeof(MetaData));
    int totalEntries = lecturaMD.totalEntries;
    //Bitmap
    BitMap bm(lecturaMD.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char *>(&bm), sizeof(bm));

    iNodeEntry iRead;
    for (int i = 0; i < totalEntries; i++)
    {
        fileSystem.read(reinterpret_cast<char *>(&iRead), sizeof(iRead));
        if(i == index)
        {
            rString = iRead.fileName;
            break;
        }

    }

    fileSystem.close();
    return rString;

}

iNodeEntry FSFunctions::get_iNodeByIndex(int index)
{
    iNodeEntry rNode;

    ifstream fileSystem("fileSystem.recio", ios::in | ios::binary);
    fileSystem.seekg(0, ios::beg);
    MetaData lecturaMD;
    fileSystem.read(reinterpret_cast<char *>(&lecturaMD), sizeof(MetaData));
    int totalEntries = lecturaMD.totalEntries;
    //Bitmap
    BitMap bm(lecturaMD.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

    iNodeEntry iRead;
    for (int i = 0; i < totalEntries; i++)
    {
        fileSystem.read(reinterpret_cast<char *>(&iRead), sizeof(iNodeEntry));
        if (i == index)
        {
            rNode = iRead;
            break;
        }
        
    }

    fileSystem.close();
    return rNode;
    
}

void FSFunctions::isOccupied()
{
    ifstream fileSystem("fileSystem.recio", ios::in | ios::binary);

    fileSystem.seekg(0, ios::beg);
    MetaData lecturaMD;
    fileSystem.read(reinterpret_cast<char *>(&lecturaMD), sizeof(MetaData));

    iNodeEntry iRead;
    fileSystem.read(reinterpret_cast<char *>(&iRead), sizeof(iNodeEntry));

    cout << "\n\nOccupied?" << iRead.occupied;
}

void FSFunctions::rewrite_iNode(int index, iNodeEntry rw){

    fstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);

    if (!fileSystem)
    {
        cout << "Error opening the File System!";
        return;
    }

    fileSystem.seekg(0, ios::beg);
    MetaData md;
    fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

    fileSystem.seekp(ios::beg+sizeof(MetaData)+sizeof(bm)+(sizeof(iNodeEntry)*index));
    fileSystem.write(reinterpret_cast<const char*>(&rw), sizeof(iNodeEntry));
    fileSystem.close();
    
}

int FSFunctions::cd(char directoryToChange[25], int actual)
{
    ifstream fileSystem("fileSystem.recio", ios::in | ios::binary);

    if (!fileSystem)
    {
        cout << "\n\nError abriendo el File System."; 
        return -2;//-2 es cuando no se puede abrir el archivo 
    }

    fileSystem.seekg(0, ios::beg);
    MetaData md;
    fileSystem.read(reinterpret_cast<char *>(&md), sizeof(MetaData));

    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

    int totalEntriesFS = md.totalEntries;
    iNodeEntry entry;

    for (int i = 0; i < totalEntriesFS; i++)
    {
        fileSystem.read(reinterpret_cast<char *>(&entry), sizeof(iNodeEntry));
        if(strcmp(directoryToChange, entry.fileName) == 0)
        {
            if(actual == entry.father)
            {
                fileSystem.close();
                return i;
            }
        }
    }

    fileSystem.close();
    return -1;
    
}

int FSFunctions::inverseCD(iNodeEntry actualDir)
{
    ifstream fileSystem("fileSystem.recio", ios::in | ios::binary);

    if (!fileSystem)
    {
        cout << "\n\nError abriendo el File System."; 
        return -2;//-2 es cuando no se puede abrir el archivo 
    }

    fileSystem.seekg(0, ios::beg);
    MetaData md;
    fileSystem.read(reinterpret_cast<char *>(&md), sizeof(MetaData));

    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

    int totalEntriesFS = md.totalEntries;
    iNodeEntry entry;

    for (int i = 0; i < totalEntriesFS; i++)
    {
        fileSystem.read(reinterpret_cast<char *>(&entry), sizeof(iNodeEntry));
        if (actualDir.father == i)
        {
            fileSystem.close();
            return i;
        }
        
    }

    fileSystem.close();
    return -1;
    
}

int FSFunctions::get_LastBrother(int father)
{
    ifstream fileSystem("fileSystem.recio", ios::in | ios::binary);

    MetaData md;
    fileSystem.seekg(0, ios::beg);
    fileSystem.read(reinterpret_cast<char *>(&md), sizeof(MetaData));
    int totalEntries = md.totalEntries;

    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

    iNodeEntry iRead;
    for (int i = 0; i < totalEntries; i++)
    {
        fileSystem.read(reinterpret_cast<char *>(&iRead), sizeof(MetaData));
        if (iRead.father == father && iRead.rightBrother == -1)
            return i;
        
    }

    return -1;
    
}

int FSFunctions::mkdir(int cdFather, char name[25])
{
    fstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);
    if (!fileSystem)
    {
        cout << "Error opening the File System.";
        return -2;
    }

    MetaData md;
    fileSystem.seekg(0, ios::beg);
    fileSystem.read(reinterpret_cast<char *>(&md), sizeof(MetaData));
    
    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

    int totalEntries = md.totalEntries;
    iNodeEntry newDir;
    newDir.father = cdFather;
    newDir.occupied = true;
    strcpy(newDir.fileName, name);
    newDir.fileType = 'D';

    iNodeEntry iRead;
    iNodeEntry f = get_iNodeByIndex(cdFather);

    for(int i = 0; i < totalEntries; i++)
    {
        fileSystem.read(reinterpret_cast<char *>(&iRead), sizeof(iNodeEntry));
        if (iRead.occupied == false)
        {
            if (f.firstSon == -1)
            {
                f.firstSon = i;
                rewrite_iNode(cdFather, f);
            }
            else
            {
                //Find the last son and link it by right brother to this one
                iNodeEntry lb = get_iNodeByIndex(get_LastBrother(cdFather));
                lb.rightBrother = i;
            }
            
            
            fileSystem.seekp(ios::beg+sizeof(MetaData)+sizeof(bm)+(sizeof(iNodeEntry)*i));
            fileSystem.write(reinterpret_cast<const char*>(&newDir), sizeof(iNodeEntry));
            fileSystem.close();
            return i;
        }
        
    }
    
    fileSystem.close();
    return -1;
    
}

void FSFunctions::ls(int actualDir)
{
    ifstream fileSystem("fileSystem.recio", ios::binary | ios::in);
    if (!fileSystem)
    {
        cout << "Error opening the File System.";
        return;
    }

    fileSystem.seekg(0, ios::beg);
    MetaData md;
    fileSystem.read(reinterpret_cast<char *>(&md), sizeof(MetaData));
    int totalEntries = md.totalEntries;

    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

    iNodeEntry iRead;
    cout << endl;
    for (int i = 0; i < totalEntries; i++)
    {
        fileSystem.read(reinterpret_cast<char *>(&iRead), sizeof(iNodeEntry));
         if (iRead.father == actualDir)
             cout << "\t" << iRead.fileName << "\t";
        
    }
    
    fileSystem.close();
    
}

bool FSFunctions::rm(int actualDir, int sonDir)
{
    fstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);
    if (!fileSystem)
    {
        cout << "\nError opening the File System.";
        return false;
    }
    
    if (sonDir == 0)
        cout << "\n\t\tNo puede borrar la carpeta raiz!!";
    else
    {
        iNodeEntry emptyNode;

        MetaData md;
        fileSystem.seekg(0, ios::beg);
        fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));

        int totalEntries = md.totalEntries;
        iNodeEntry iRead;
        for (int i = 0; i < totalEntries; i++)
        {
            fileSystem.read(reinterpret_cast<char *>(&iRead), sizeof(iRead));
            
            if (iRead.father == sonDir || iRead.father == i)
            {
                fileSystem.seekp(sizeof(MetaData)+(sizeof(iNodeEntry)*i));
                fileSystem.write(reinterpret_cast<const char *>(&emptyNode), sizeof(iNodeEntry));
                return true;
            }
            
        }

    }
    
    return false;

}

void FSFunctions::import(int actualDir, const char *fileName)
{
    //iNodeEntry declaration
    iNodeEntry newFile;
    newFile.fileType = 'F';
    strcpy(newFile.fileName, fileName);
    newFile.father = actualDir;
    newFile.rightBrother = get_LastBrother(actualDir);
    newFile.occupied = true;

    ifstream* fileReader = new ifstream();
    fileReader->open(fileName, ios::in | ios::out);

    fileReader->seekg(0 ,ios::end);
    long fileSizeInBytes = fileReader->tellg();
    
    double size = fileSizeInBytes / (double)4096;
    long remainingSize = fileSizeInBytes;

    newFile.size = (unsigned int)fileSizeInBytes;

    //ADD THE SIZE OF THE FILE

    fileReader->seekg(0, ios::beg);

    if (size < 1)
        size = 1;
    
    cout << "Size in bytes: " << fileSizeInBytes;
    int total = ceil(size);

    //File System things
    fstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);

    MetaData md;
    fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
    BitMap bm(md.totalDirectBlocks);
    fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));
    IDB * iDataBlock = new IDB;
    
    for (int i = 0; i < CANTIDAD_DE_BLOQUES_INODE; i++)
    {
        if (!fileReader->eof())
        {
            if (remainingSize < 4096)
                fileReader->read(iDataBlock->iBlocks[i].data, remainingSize);
            else
                fileReader->read(iDataBlock->iBlocks[i].data, 4096);

            remainingSize -= 4096;
        }
        else
            break;
        
    }


    fileSystem.seekp(ios::beg+sizeof(MetaData)+sizeof(bm)+(sizeof(iNodeEntry)*md.totalEntries)+(sizeof(IDB)*getFirstFreeIndex()));
    fileSystem.write(reinterpret_cast<const char*>(iDataBlock), sizeof(IDB));
    create_iNodeEntry(newFile);

    fileSystem.close();
    fileReader->close();

}

void FSFunctions::exportFile(const char* originFile, const char* exportFileName)
{
	ofstream exporter;
	exporter.open(exportFileName, ios::binary | ios::out);

	iNodeEntry iOriginFile = get_iNodeByName(originFile);
	int size = iOriginFile.size;
	int remainingSize = size;

	fstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);

	MetaData md;
	fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
	BitMap bm(md.totalDirectBlocks);
	fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));
	IDB* iDataBlock = new IDB;
	for (int i = 0; i < md.totalEntries; i++)
	{
		fileSystem.read(reinterpret_cast<char*>(iDataBlock), sizeof(IDB));
		if (i = getIndexBy_iNode(iOriginFile))
			break;
	}

	for (int i = 0; i < CANTIDAD_DE_BLOQUES_INODE; i++)
	{
		if (remainingSize <= 0)
			break;

		if (iDataBlock->iBlocks[i].data[4096] == '$')
			break;

		if (remainingSize < 4096)
			exporter.write(reinterpret_cast<char*>(&iDataBlock->iBlocks[i].data), remainingSize);
		else
			exporter.write(reinterpret_cast<char*>(&iDataBlock->iBlocks[i].data), 4096);

		remainingSize -= 4096;

	}

	exporter.close();
	fileSystem.close();

}
