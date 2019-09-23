#include <fstream>
#include <math.h>
#include "ImportantFSData.hpp"



class FSFunctions {

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
	void eraseIDB(int index);
	void act_bitMap(BitMap newBitmap);
	void print_BitMap();

	//Commands
	int cd(char directoryToChange[25], int actual);
	int inverseCD(iNodeEntry actualDir);
	int mkdir(int cdFather, char name[25]);//Returns an int refering to its position in the global entries
	void ls(int actualDir);
	void rm(iNodeEntry iRead, int pos);
	void import(int actualDir, const char* fileName);
	void exportFile(const char* originFile, const char* exportFile);

};

bool FSFunctions::createdDisc()
{
	ifstream fileSystem("fileSystem.recio", ios::in | ios::binary);
	if (!fileSystem)
		return false;
	else
		return true;
}

void FSFunctions::print_BitMap()
{
	ifstream fileSystem("fileSystem.recio", ios::in | ios::binary);
	MetaData md;
	fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
	BitMap bm(md.totalDirectBlocks);
	fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));
	bm.printBitMap(md.totalDirectBlocks);
	fileSystem.close();
}

void FSFunctions::act_bitMap(BitMap newBitmap)
{
	ofstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);

	fileSystem.seekp(ios::beg + sizeof(MetaData));
	fileSystem.write(reinterpret_cast<char*>(&newBitmap), sizeof(newBitmap));
	fileSystem.close();
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
		fileSystem.read(reinterpret_cast<char*>(&iRead), sizeof(iNodeEntry));
		if (!iRead.occupied)
		{
			iNodeEntry father = get_iNodeByIndex(newEntry.father);
			

			if (father.firstSon == -1)
			{
				father.firstSon = i;
				rewrite_iNode(getIndexBy_iNode(father), father);
			}
			else
			{
				iNodeEntry lb = get_iNodeByIndex(get_LastBrother(getIndexBy_iNode(father)));
				lb.rightBrother = i;
				rewrite_iNode(getIndexBy_iNode(lb), lb);
			}

			fileSystem.seekp(ios::beg + sizeof(MetaData) + sizeof(bm) + (sizeof(iNodeEntry) * i));
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

	md.systemSize = (sizeof(iNodeEntry) * qty) + sizeof(md);
	md.totalEntries = qty;
	md.totalDirectBlocks = CANTIDAD_DE_BLOQUES_INODE * qty;
	strcpy(md.fsName, "File System: Recio");
	//Write metadata
	fileSystem.write(reinterpret_cast<const char*>(&md), sizeof(MetaData));
	//Bitmap
	BitMap bm(md.totalDirectBlocks);
	fileSystem.write(reinterpret_cast<const char*>(&bm), sizeof(bm));
	//Directory Entries
	iNodeEntry entries;

	iNodeEntry first;
	strcpy(first.fileName, "/"); first.fileType = 'D'; first.occupied = true;

	for (int i = 0; i < qty; i++)
	{
		if (i == 0)
			fileSystem.write(reinterpret_cast<const char*>(&first), sizeof(iNodeEntry));
		else
			fileSystem.write(reinterpret_cast<const char*>(&entries), sizeof(iNodeEntry));
	}

	IDB* iDataBlock = new IDB;
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
	fileSystem.read(reinterpret_cast<char*>(&lecturaMD), sizeof(MetaData));
	int totalEntries = lecturaMD.totalEntries;
	//Bitmap
	BitMap bm(lecturaMD.totalDirectBlocks);
	fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

	iNodeEntry iRead;
	for (int i = 0; i < totalEntries; i++)
	{
		fileSystem.read(reinterpret_cast<char*>(&iRead), sizeof(iRead));
		if (i == index)
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
	if (index == -1)
		return rNode;

	ifstream fileSystem("fileSystem.recio", ios::in | ios::binary);
	fileSystem.seekg(0, ios::beg);
	MetaData lecturaMD;
	fileSystem.read(reinterpret_cast<char*>(&lecturaMD), sizeof(MetaData));
	int totalEntries = lecturaMD.totalEntries;
	//Bitmap
	BitMap bm(lecturaMD.totalDirectBlocks);
	fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

	iNodeEntry iRead;
	for (int i = 0; i < totalEntries; i++)
	{
		fileSystem.read(reinterpret_cast<char*>(&iRead), sizeof(iNodeEntry));
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
	fileSystem.read(reinterpret_cast<char*>(&lecturaMD), sizeof(MetaData));

	iNodeEntry iRead;
	fileSystem.read(reinterpret_cast<char*>(&iRead), sizeof(iNodeEntry));

	cout << "\n\nOccupied?" << iRead.occupied;
}

void FSFunctions::rewrite_iNode(int index, iNodeEntry rw) {

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

	fileSystem.seekp(ios::beg + sizeof(MetaData) + sizeof(bm) + (sizeof(iNodeEntry) * index));
	fileSystem.write(reinterpret_cast<const char*>(&rw), sizeof(iNodeEntry));
	fileSystem.close();

}

void FSFunctions::eraseIDB(int index)
{
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
	iNodeEntry rw;
	for (int i = 0; i < md.totalEntries; i++)
		fileSystem.read(reinterpret_cast<char*>(&rw), sizeof(iNodeEntry));
	IDB* iDataBlocks = new IDB;
	iNodeEntry file = get_iNodeByIndex(index);
	for (int i = 0; i < (file.size/4096); i++)
	{
		if (bm.bitmap[i] == '1')
		{
			bm.setOff(i);
		}
		else
			break;
	}
	// for (int i = 0; i < md.totalEntries; i++)
	// {
	//     fileSystem.read(reinterpret_cast<char*>(iDataBlocks), sizeof(IDB));
	//     if (i == index)
	//         break;

	// }
	act_bitMap(bm);
	fileSystem.seekp(ios::beg + sizeof(MetaData) + sizeof(bm) + (sizeof(iNodeEntry) * md.totalEntries + (sizeof(IDB) * index)));
	fileSystem.write(reinterpret_cast<const char*>(iDataBlocks), sizeof(IDB));
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
	fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));

	BitMap bm(md.totalDirectBlocks);
	fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

	int totalEntriesFS = md.totalEntries;
	iNodeEntry entry;

	for (int i = 0; i < totalEntriesFS; i++)
	{
		fileSystem.read(reinterpret_cast<char*>(&entry), sizeof(iNodeEntry));
		if (strcmp(directoryToChange, entry.fileName) == 0)
		{
			if (actual == entry.father)
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
	fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));

	BitMap bm(md.totalDirectBlocks);
	fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

	int totalEntriesFS = md.totalEntries;
	iNodeEntry entry;

	for (int i = 0; i < totalEntriesFS; i++)
	{
		fileSystem.read(reinterpret_cast<char*>(&entry), sizeof(iNodeEntry));
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
	fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
	int totalEntries = md.totalEntries;

	BitMap bm(md.totalDirectBlocks);
	fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

	iNodeEntry iRead;
	for (int i = 0; i < totalEntries; i++)
	{
		fileSystem.read(reinterpret_cast<char*>(&iRead), sizeof(iNodeEntry));
		if (iRead.father == father && iRead.rightBrother == -1)
		{
			fileSystem.close();
			return i;
		}

	}

	fileSystem.close();
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
	fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));

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

	for (int i = 0; i < totalEntries; i++)
	{
		fileSystem.read(reinterpret_cast<char*>(&iRead), sizeof(iNodeEntry));
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
				rewrite_iNode(getIndexBy_iNode(lb), lb);
			}


			fileSystem.seekp(ios::beg + sizeof(MetaData) + sizeof(bm) + (sizeof(iNodeEntry) * i));
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
	fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
	int totalEntries = md.totalEntries;

	BitMap bm(md.totalDirectBlocks);
	fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

	iNodeEntry iRead;
	cout << endl;
	for (int i = 0; i < totalEntries; i++)
	{
		fileSystem.read(reinterpret_cast<char*>(&iRead), sizeof(iNodeEntry));
		if (iRead.father == actualDir)
			cout << "\t" << iRead.fileName << "\t";

	}

	fileSystem.close();

}

void FSFunctions::rm(iNodeEntry iRead, int pos)
{

	if (pos == -1)
		return;
	int sonPos = iRead.firstSon;
	int brotherPos = iRead.rightBrother;

	iNodeEntry empty;
	rewrite_iNode(pos, empty);
	IDB* emptyIDB = new IDB;
	eraseIDB(pos);

	if (brotherPos != -1)
	{
		iNodeEntry brother = get_iNodeByIndex(brotherPos);
		rm(brother, brotherPos);
	}
	if (sonPos != -1)
	{
		iNodeEntry son = get_iNodeByIndex(sonPos);
		rm(son, sonPos);
	}
}

void FSFunctions::import(int actualDir, const char* fileName)
{
	//iNodeEntry declaration
	iNodeEntry newFile;
	newFile.fileType = 'F';
	strcpy(newFile.fileName, fileName);
	newFile.father = actualDir;
	newFile.rightBrother = get_LastBrother(actualDir);
	newFile.occupied = true;

	ifstream* fileReader = new ifstream();
	fileReader->open(fileName, ios::in | ios::out | ios::binary);

	fileReader->seekg(0, ios::end);
	long fileSizeInBytes = fileReader->tellg();

	long remainingSize = fileSizeInBytes;

	newFile.size = (unsigned int)fileSizeInBytes;

	fileReader->seekg(0, ios::beg);

	cout << "Size in bytes: " << fileSizeInBytes;

	//File System things
	fstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);

	MetaData md;
	fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
	BitMap bm(md.totalDirectBlocks);
	fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));
	fileSystem.seekg(ios::beg+sizeof(MetaData)+sizeof(bm)+(sizeof(iNodeEntry)*md.totalEntries));

	IDB* iDataBlock = new IDB;
	create_iNodeEntry(newFile);
	iNodeEntry createdNode = get_iNodeByName(fileName);

	//ofstream writer("prueba.jpeg", ios::binary | ios::app);

	for (int i = 0; i < CANTIDAD_DE_BLOQUES_INODE; i++)
	{
		if (!fileReader->eof())
		{
			if (remainingSize < 4096)
			{
				//bm.setOn((getIndexBy_iNode(createdNode)*CANTIDAD_DE_BLOQUES_INODE)+(i / 8));
				fileReader->read(reinterpret_cast<char*>(&iDataBlock->iBlocks[i].data), remainingSize);
				//writer.write(reinterpret_cast<const char*>(&iDataBlock->iBlocks[i].data), remainingSize);
			}
			else
			{
				//bm.setOn((getIndexBy_iNode(createdNode) * CANTIDAD_DE_BLOQUES_INODE) + (i / 8));
				fileReader->read(reinterpret_cast<char*>(&iDataBlock->iBlocks[i].data), 4096);
				//writer.write(reinterpret_cast<const char*>(&iDataBlock->iBlocks[i].data), 4096);
			}

			remainingSize -= 4096;
		}
		else
			break;

	}

	act_bitMap(bm);
	fileSystem.seekp(sizeof(MetaData) + sizeof(bm) + (sizeof(iNodeEntry) * md.totalEntries) + (sizeof(IDB) * getIndexBy_iNode(createdNode)), ios::beg);
	fileSystem.write(reinterpret_cast<const char*>(iDataBlock), sizeof(IDB));

	fileSystem.close();
	fileReader->close();

}

void FSFunctions::exportFile(const char* originFile, const char* exportFileName)
{
	ofstream exporter;
	exporter.open(exportFileName, ios::binary | ios::app);

	iNodeEntry iOriginFile = get_iNodeByName(originFile);
	int size = iOriginFile.size;
	int remainingSize = size;

	fstream fileSystem("fileSystem.recio", ios::in | ios::out | ios::binary);

	MetaData md;
	fileSystem.read(reinterpret_cast<char*>(&md), sizeof(MetaData));
	BitMap bm(md.totalDirectBlocks);
	fileSystem.read(reinterpret_cast<char*>(&bm), sizeof(bm));

	int index = getIndexBy_iNode(iOriginFile);
	fileSystem.seekg(sizeof(MetaData)+sizeof(bm)+(sizeof(iNodeEntry)*md.totalEntries)+(sizeof(IDB)*index), ios::beg);

	IDB* iDataBlock = new IDB;
	fileSystem.read(reinterpret_cast<char*>(iDataBlock), sizeof(IDB));

	iDataBlock;
	for (int i = 0; i < CANTIDAD_DE_BLOQUES_INODE; i++)
	{
		if (remainingSize <= 0)
			break;

		if (remainingSize < 4096)
			exporter.write(reinterpret_cast<const char*>(&iDataBlock->iBlocks[i].data), remainingSize);
		else
			exporter.write(reinterpret_cast<const char*>(&iDataBlock->iBlocks[i].data), 4096);

		remainingSize -= 4096;

	}

	exporter.close();
	fileSystem.close();

}
