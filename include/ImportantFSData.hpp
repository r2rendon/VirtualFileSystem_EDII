#include <iostream>

using namespace std;

const int CANTIDAD_DE_BLOQUES_INODE = 33308;

struct iNodeEntry{
    
    char fileName[25];
    char fileType;//F for File, D for Directory, N for None
    int father;
    int rightBrother;
    int firstSon;
    bool occupied;
    unsigned int size;

    iNodeEntry()
    {
        strcpy(fileName, "-");
        fileType = 'N';
        father = -1;
        rightBrother = -1;
        firstSon = -1;
        occupied = false;
        size = 0;
    }

};//SizeOf 48

struct MetaData{

    char fsName[30];
    unsigned int systemSize;
    char date[8];
    //char owner[30];
    unsigned int totalEntries;
    unsigned int bitmapSize;
    unsigned int totalDirectBlocks;

    MetaData()
    {
        strcpy(fsName, "-");
        systemSize = 0;
        strcpy(date, "-");
        totalEntries = 0;
        bitmapSize = 0;
        totalDirectBlocks = 0;
    }

};//SizeOf 56

struct BitMap{
    
    char * bitmap;

    BitMap()
    {
        
    }
    BitMap(int DBAmount)
    {
        bitmap = new char[DBAmount/8];
    }

    void initBitMap(int DBAmount)
    {
        for (int i = 0; i < DBAmount/8; i++)
            bitmap[i] = 0;
        
    }

    void printBitMap(int DBAmount)
    {
        const int SHIFT = 8 * sizeof(char) -1;
        const char MASK = 1 << SHIFT;

        for (int i = 0; i < DBAmount/8; i++)
        {
            char value;
            value = bitmap[i];
            for (int c = 0; c <= DBAmount/8; c++)
            {
                cout << (value & MASK ? '1' : '0');
                value <<= 1;

                if(c % 8 == 0)
                    cout << ' ';
            }
            
        }
        cout << endl;
    }

    void setOn(int nBlock)
    {
        int positionByte = nBlock/8;
        int iniPosition = (nBlock/8) * 8;

        for (int i = iniPosition, x = 7; i < (positionByte * 8) + 8; i++, x--)
        {
            if (i == nBlock)
            {
                bitmap[positionByte] |= 1 << x;
                break;
            }
            
        }
        
    }

    void setOff(int nBlock)
    {
        int positionByte = nBlock/8;
	    int iniPosition = (nBlock/8) * 8;

	    for(int i = iniPosition, x = 7; i < (positionByte * 8) + 8; i++, x--)
	    {
		    if(i == nBlock)
		    {
			    bitmap[positionByte] &= ~(1 << x); 
			    break;
		    }
	    }
    }

};

struct DataBlock
{
    char data[4096]{};

};

struct IDB{
    DataBlock iBlocks[CANTIDAD_DE_BLOQUES_INODE]{};
    IDB()
    {
        for (int i = 0; i < CANTIDAD_DE_BLOQUES_INODE; i++)
            for (int j = 0; j < 4096; j++)
                iBlocks[i].data[j] = '$';
    
    }
};
