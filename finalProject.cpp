
#include "finalProject.h"


void decToBinary(int n, char &c){
    // array to store binary number
    int binaryNum[8];

    // counter for binary array
    int i = 0;
    while (n > 0)
    {
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--)
    {
        if (binaryNum[j] == 1)
            c = c | 1u << j;
    }
}

/*
************************* 
 fsInode funcitons
*************************
*/

//constructor.
fsInode::fsInode(int _block_size, int _num_of_direct_blocks){
    fileSize = 0;
    block_size = _block_size;
    num_of_direct_blocks = _num_of_direct_blocks;

    directBlocks = new int[num_of_direct_blocks];
    assert(directBlocks);

    for (int i = 0; i < num_of_direct_blocks; i++){
        directBlocks[i] = -1;
    }
    singleInDirect = -1;

}
//update file size.
void fsInode::addFileSize(int num){
    fileSize+=num;
}
//get file size.
int fsInode::getFileSize(){
    return fileSize;
}
//index= free block from the disk & blockNum is the place 2in directBlocks. 
void fsInode::addDirectBlock(int blockNum, int index){
    directBlocks[blockNum]=index;
}
//given a index to directBlocks return blockNum=directBlocks[index]. 
int fsInode::getDirectBlock(int index){
    return directBlocks[index];
}
//get inDirect block index.
int fsInode::getInDirectBlock(){
    return singleInDirect;
}
// set singleInDirect index.
void fsInode::setInDirectBlock(int blockNum){
    singleInDirect=blockNum;
}
//destructor.
fsInode::~fsInode(){
    delete[] directBlocks;
}

/*
************************* 
 FileDescriptor funcitons
*************************
*/

//constructor.
FileDescriptor::FileDescriptor(string FileName, fsInode *fsi){

    file.first = FileName;
    file.second = fsi;
    inUse = true;
}
//get file name.
string FileDescriptor::getFileName(){

    return file.first;
}
//get inode *.
fsInode *FileDescriptor::getInode(){

    return file.second;
}
//get inUse status.
bool FileDescriptor::isInUse(){

    return (inUse);
}
//set inUse flag.
void FileDescriptor::setInUse(bool _inUse){

    inUse = _inUse;
}
//destructor.
FileDescriptor::~FileDescriptor(){
}

/*
************************* 
 fsDisk funcitons
*************************
*/

//constructor.
fsDisk::fsDisk(){
    is_formated=false;
    sim_disk_fd = fopen(DISK_SIM_FILE, "r+");
    assert(sim_disk_fd);
    for (int i = 0; i < DISK_SIZE; i++)
    {
        int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
        ret_val = fwrite("\0", 1, 1, sim_disk_fd);
        assert(ret_val == 1);
    }
    fflush(sim_disk_fd);
}

//prints the contents of the disc.
void fsDisk::listAll(){
    int i = 0;
    for (auto it = begin(OpenFileDescriptors); it != end(OpenFileDescriptors); ++it)
    {
        cout << "index: " << i << ": FileName: " << it->getFileName() << " , isInUse: " << it->isInUse() << endl;
        i++;
    }
    char bufy;
    cout << "Disk content: '";
    for (i = 0; i < DISK_SIZE; i++)
    {
        int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
        ret_val = fread(&bufy, 1, 1, sim_disk_fd);
        cout << bufy;
    }
    cout << "'" << endl;
}

//get num of blocks in disk = bit vector size.
int fsDisk::getNumOfBlocks(){
    return BitVectorSize;
}

//format disk.
void fsDisk::fsFormat(int blockSize, int direct_Enteris_){

    //checking if the disk has been formatted.
    if(is_formated==true){
         cout << "The disk has been formatted. It cannot be changed. If you want to restart please exit and run the program again."<<endl;
        return;
    }
    //checking if the blockSize is legal.
    if((blockSize<=1)||(DISK_SIZE%blockSize!=0)){ //min block size=2
        cout << "Illegal block Size, try again."<<endl;
        return;
    }

    //initialize the attributes.
    this->block_size=blockSize;
    this->direct_enteris=direct_Enteris_;
    is_formated=true;
    maxFileSize = (direct_enteris +block_size)*block_size;

    // BitVector initialization.
    BitVectorSize=DISK_SIZE/blockSize;
    BitVector= new int [BitVectorSize];
    assert(BitVector);
    for(int i=0;i<BitVectorSize;i++)//at start all blocks are marked as empty
        BitVector[i]=0;

}

//create new file.
int fsDisk::CreateFile(string fileName){

    //checking if the disk has been formatted
    if(is_formated==false){
        cout << "Please format the disk before creating files!"<<endl;
        return -1;
    }

    //checking if the file exist
    if (isFileExist(fileName)){
        cout << "A file with this name already exists."<<endl;
        return -1;
    }

    //checking if there are empty blocks on the disk
    if(countEmptyBlocks()==0){
        cout << "The disk memory is full. please delete some files first."<<endl;
        return -1;
    }

    //OK- create a new file
    fsInode* inode= new fsInode(block_size, direct_enteris);
    assert(inode);
    FileDescriptor fd(fileName, inode);

    //updates all Data structures
    MainDir.insert(pair<string, fsInode*>(fileName, inode));
    OpenFileDescriptors.push_back(fd);
    
    return  OpenFileDescriptors.size()-1;
}

//mark file as open.
int fsDisk::OpenFile(string fileName){
    //checking if the disk has been formatted
    if(is_formated==false){
        cout << "Please format the disk before accessing files!"<<endl;
        return -1;
    }

    //checking if the file exist
    if (!(isFileExist(fileName))){
        cout << "A file with this name does not exists. Please create the file first."<<endl;
        return -1;
    }

    //finding the file
    int fd=findFileFD(fileName);

    //checking if the file is open
    if(OpenFileDescriptors[fd].isInUse()){
        cout << "The file is open already."<<endl;
        return -1;
    }

    //opens the file
   OpenFileDescriptors[fd].setInUse(true);

    return fd;
}

//mark file as closed.
string fsDisk::CloseFile(int fd){
    //checking if the disk has been formatted
    if(is_formated==false){
        cout << "Please format the disk before accessing files!"<<endl;
        return "-1";
    }

    //checks if the index exists
    if(fd>=OpenFileDescriptors.size()){
        cout << "A file with this fd does not exists. Please create the file first."<<endl;
        return "-1";
    }

    //checking if the file is closed
    if(!OpenFileDescriptors[fd].isInUse()){
        cout << "The file is closed already."<<endl;
        return "-1";
    }

    //close the file
    OpenFileDescriptors[fd].setInUse(false);

    return OpenFileDescriptors[fd].getFileName();
}

//write a string to a file memory manager(inode).
int fsDisk::WriteToFile(int fd, char *buf, int len){
    
    //checking if the disk has been formatted.
    if(is_formated==false){
        cout << "Please format the disk before accessing files!"<<endl;
        return -1;
    }
    //checks if the index exists.
    if(fd>=OpenFileDescriptors.size()){
        cout << "A file with this fd does not exists. Please create the file first."<<endl;
        return -1;
    }
    //checking if the file is closed.
    if(!OpenFileDescriptors[fd].isInUse()){
        cout << "The file is closed. Please open the file first."<<endl;
        return -1;
    }

    fsInode* inode= OpenFileDescriptors[fd].getInode();
    int fileSize=inode->getFileSize();
   
    //checking if there is place in the files block.
    if(len>(maxFileSize-fileSize)){
        cout << "You do not have enough space for this string in the file."<<endl;
        return -1;
    }

    //Calculate the number of blocks we will need.
    int blockInUse=fileSize/block_size;
    int offset=fileSize%block_size;

    int tempLen=len;
    bool inDirect=false;

    if(offset!=0){
        blockInUse++;
        tempLen-=(block_size-offset);
    }

    if(blockInUse>direct_enteris)
        inDirect=true;

    int numOfBlocks= tempLen/block_size;
    if(tempLen%block_size!=0)
        numOfBlocks++;

    if((!inDirect) && (blockInUse+numOfBlocks)>direct_enteris)
        numOfBlocks++;
  
    //checks if there are enough empty blocks   
    if(numOfBlocks>countEmptyBlocks()){
        cout << "You do not have enough space on the disk. Please delete some files first."<<endl;
        return -1;
    }
    
    //we can start writing finally!
    int i=0,ret_val;
    int blockIndex,lastBlock, inDirectBlock,inDirectIndex;
    char c;

    while(i<len){

        //first calculate the block index to write into from the disk.
        fileSize=inode->getFileSize();
        lastBlock=fileSize/block_size;
        offset=fileSize%block_size;

        //we can still use the direct blocks.
        if(!inDirect){
            if(offset!=0)
                blockIndex=inode->getDirectBlock(lastBlock);
            else if(lastBlock==direct_enteris){
                inDirect=true;
                continue; 
            }
            else{
                blockIndex=findEmptyBlock();
                inode->addDirectBlock(lastBlock,blockIndex);
            }
        }

        //inDerect=true. go to inderect block.
        else{
            if(inode->getInDirectBlock()==-1)//there is index block?
                inode->setInDirectBlock(findEmptyBlock());
            
            inDirectBlock=inode->getInDirectBlock();//get index block.
            inDirectIndex=lastBlock-direct_enteris;//num of block in index block.

            ret_val = fseek(sim_disk_fd, inDirectBlock*block_size+inDirectIndex, SEEK_SET);//go to the index block.

            if(offset!=0){//we have allocated a block. get the index.
                c=0;
                ret_val = fread(&c, 1, 1, sim_disk_fd);
                blockIndex=(int)c;
            }

            else{//allocate new block.
                blockIndex=findEmptyBlock();
                c=0;
                decToBinary(blockIndex,c);
                ret_val = fwrite(&c, 1, 1, sim_disk_fd);
                assert(ret_val == 1);
            }

        }

        //writing + updating...
        ret_val = fseek(sim_disk_fd, blockIndex*block_size+offset, SEEK_SET);
        ret_val = fwrite(&buf[i], 1, 1, sim_disk_fd);
        assert(ret_val == 1);
        //cout<<"write in disk at block: "<<blockIndex<<" and offset: "<<offset<<" the char: "<<buf[i]<< endl;

        i++;
        inode->addFileSize(1);
   }

    return fd;

}

//returns all the string in the file memory manager.
int fsDisk::ReadFromFile(int fd, char *buf, int len){

    //checking if the disk has been formatted.
    if(is_formated==false){
        cout << "Please format the disk before accessing files!"<<endl;
        return -1;
    }

    //checks if the index exists.
    if(fd>=OpenFileDescriptors.size()){
        cout << "A file with this fd does not exists. Please create the file first."<<endl;
        return -1;
    }

    //checking if the file is closed.
    if(!OpenFileDescriptors[fd].isInUse()){
        cout << "The file is closed. Please open the file first."<<endl;
        return -1;
    }

    fsInode* inode=OpenFileDescriptors[fd].getInode();
    int fileSize= inode->getFileSize();

    //Checks if the number of chars to read is valid.
    if(len<0){
        cout << "Illegal length. try again."<<endl;
        return -1;
    }
    if(len>fileSize){
        cout << "Notice: the file size is only- "<<fileSize<<endl;
        len=fileSize;
    }

    int dBIndex=0,i=0,offset=0,blockNum=0, inDBOffset=-1;
    int inDBIndex=inode->getInDirectBlock();
    int ret_val;
    char c;
    bool inDirectFlag=false;

    while(i<len){
        //read from the direct blocks first.
        if(!inDirectFlag){
            if(dBIndex==direct_enteris){
                offset=block_size;
                inDirectFlag=true;
                continue;
            }
            if(offset==block_size){
                offset=0;
                dBIndex++;
                continue;
            }
            blockNum= inode->getDirectBlock(dBIndex);
        }
        //if you need read from the inderect block too.
        else{   
           if(offset==block_size){
                offset=0;
                inDBOffset++;
                ret_val = fseek(sim_disk_fd, inDBIndex*block_size+inDBOffset, SEEK_SET);//go to the index block.
                c=0;
                ret_val = fread(&c, 1, 1, sim_disk_fd);
                blockNum=(int)c;
           }
        }

        ret_val = fseek(sim_disk_fd,blockNum*block_size+offset, SEEK_SET);
        ret_val = fread(&buf[i], 1, 1, sim_disk_fd);
        //cout<<"read from disk at block: "<<blockNum<<" and offset: "<<offset<<" the char: "<<buf[i]<<endl;
        offset++;
        i++;
    }
 
    buf[i]='\0';
    return fd;
}  

//Delete the file completely.
int fsDisk::DelFile(string FileName){

    //checking if the disk has been formatted.
    if(is_formated==false){
        cout << "Please format the disk before accessing files!"<<endl;
        return -1;
    }
    //checking if the file exist.
    if (!(isFileExist(FileName))){
        cout << "A file with this name does not exists. Please create the file first."<<endl;
        return -1;
    }

    //finding the file.
    int fd=findFileFD(FileName);
    fsInode* inode=OpenFileDescriptors[fd].getInode();

    //checking if the file is open.
    if(OpenFileDescriptors[fd].isInUse()){
        cout << "The file is open. Please close the file before deleting it."<<endl;
        return -1;
    }

    //deleting the data on the disk.
    deleteFileBlocks(inode);

    //updates all Data structures
    FileDescriptor deleted("",NULL);//deleted object

    mapIt=MainDir.find(FileName);
    delete mapIt->second;
    MainDir.erase(mapIt);

    OpenFileDescriptors.erase(OpenFileDescriptors.begin() + fd);
    OpenFileDescriptors.insert(OpenFileDescriptors.begin() + fd,deleted);
    CloseFile(fd);
    
    return fd;
}

//destructor.
fsDisk::~fsDisk(){
    fclose(sim_disk_fd);
    delete[] BitVector;
    for(mapIt=MainDir.begin();mapIt!=MainDir.end();mapIt++)
        delete mapIt->second;
    MainDir.clear();
    OpenFileDescriptors.clear();
}

//-----------------------------private functions for help-----------------------------//

//Checks if the file name already exists in MainDir.
bool fsDisk::isFileExist(string fileName){

    mapIt=MainDir.find(fileName);   //mapIt=iterator on the map.
    if (mapIt != MainDir.end())
        return true;    //found
    else
        return false;
}

//returns the file fd index in openFileDescriptors. 
int fsDisk::findFileFD(string fileName){

    for(int i=0; i<OpenFileDescriptors.size();i++){
        if(OpenFileDescriptors[i].getFileName()==fileName)
            return i;
    }
    return -1;
}

//return index of the next empty block.
int fsDisk::findEmptyBlock(){
    int i=0;
    for(;i<BitVectorSize;i++){
        if(BitVector[i]==0){
            BitVector[i]=1;
            return i;
        }
    }
    return -1;
}

//return the number of empty blocks on the disk
int fsDisk::countEmptyBlocks(){
    int counter=0;
    for(int i=0;i<BitVectorSize;i++){
        if(BitVector[i]==0)
            counter++;
    }
    return counter;
}

//delete all the file content from the disk.
void fsDisk::deleteFileBlocks(fsInode* inode){

    int fileSize= inode->getFileSize();
    int dBIndex=0,i=0,offset=0,blockNum=0, inDBOffset=-1;
    int inDBIndex=inode->getInDirectBlock();
    int ret_val;
    char c;
    bool inDirectFlag=false;

    while(i<fileSize){
        //delete the direct blocks first.
        if(!inDirectFlag){
            if(dBIndex==direct_enteris){
                offset=block_size;
                inDirectFlag=true;
                continue;
            }
            if(offset==block_size){
                offset=0;
                dBIndex++;
                continue;
            }
            blockNum= inode->getDirectBlock(dBIndex);
        }
        //if you have more, delete from the inderect block too.
        else{   
           if(offset==block_size){
                offset=0;
                inDBOffset++;
                ret_val = fseek(sim_disk_fd, inDBIndex*block_size+inDBOffset, SEEK_SET);//go to the index block.
                c=0;
                ret_val = fread(&c, 1, 1, sim_disk_fd);
                assert(ret_val == 1);
                blockNum=(int)c;
           }
        }

        ret_val = fseek(sim_disk_fd,blockNum*block_size+offset, SEEK_SET);
        ret_val = fwrite("\0", 1, 1, sim_disk_fd);       
        assert(ret_val == 1);        
        BitVector[blockNum]=0;
        //cout<<"BlockNum: "<<blockNum<<" char at offset: "<<offset<<" Successfully deleted."<<endl;
        offset++;
        i++;
    }

    //delete in-direct block if needed.
    if(inDirectFlag){
        i=0;
        while(i<block_size){
            ret_val = fseek(sim_disk_fd, inDBIndex*block_size+i, SEEK_SET);//go to the index block.
            ret_val = fwrite("\0", 1, 1, sim_disk_fd);       
            assert(ret_val == 1);  
            i++;
        }
        BitVector[inDBIndex]=0;
    }

        
}


        