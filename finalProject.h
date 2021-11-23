#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define DISK_SIZE 256

/*Ayelet Gibli
Id 208691675*/


// #define SYS_CALL
// ============================================================================
class fsInode {
    int fileSize;
    int singleInDirect;
    int num_of_direct_blocks;
    int block_size;
    int *directBlocks;


    public:

    fsInode(int _block_size, int _num_of_direct_blocks);

    int getFileSize();
    void addFileSize(int len);

    void addDirectBlock(int blockNum, int blockIndex);
    int getDirectBlock(int blockNum);

    void setInDirectBlock(int blockIndex);
    int getInDirectBlock();

    ~fsInode();
};

// ============================================================================
class FileDescriptor {
    pair<string, fsInode*> file;
    bool inUse;

    public:
    FileDescriptor(string FileName, fsInode* fsi);

    string getFileName();
    fsInode* getInode();

    bool isInUse();
    void setInUse(bool _inUse);

    ~FileDescriptor();
};
 
#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
// ============================================================================
class fsDisk {
    FILE *sim_disk_fd;
 
    bool is_formated;

	// BitVector - "bit" (int) vector, indicate which block in the disk is free
	//              or not.  (i.e. if BitVector[0] == 1 , means that the 
	//             first block is occupied. 
    int BitVectorSize;
    int *BitVector;

    // Unix directories are lists of association structures, 
    // each of which contains one filename and one inode number.
    map<string, fsInode*>  MainDir ; 
    map<string, fsInode*>::iterator mapIt;

    // FileDescriptors --  when you open a file, 
	// the operating system creates an entry to represent that file
    // This entry number is the file descriptor. 
    vector< FileDescriptor > OpenFileDescriptors;


    int direct_enteris;
    int block_size;
    int maxFileSize;

    public:

    fsDisk();

    void listAll();
    void fsFormat( int blockSize =4, int direct_Enteris_ = 3 );
    int CreateFile(string fileName);
    int OpenFile(string fileName);
    string CloseFile(int fd);
    int WriteToFile(int fd, char *buf, int len );
    int DelFile( string FileName );
    int ReadFromFile(int fd, char *buf, int len );
    int getNumOfBlocks(); 
    ~fsDisk();

    private:

    bool isFileExist(string fileName);
    int findFileFD(string fileName);
    int findEmptyBlock();
    int countEmptyBlocks();
    void deleteFileBlocks(fsInode* inode);
   


};