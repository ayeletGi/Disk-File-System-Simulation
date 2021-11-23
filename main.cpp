#include "finalProject.h"


/*Ayelet Gibli
Id 208691675*/

int main() {
    int blockSize; 
	int direct_entries;
    string fileName;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read; 
    int _fd;

    fsDisk *fs = new fsDisk();
    int cmd_;
    while(1) {
        cin >> cmd_;
        switch (cmd_)
        {
            case 0:   // exit
				delete fs;
				exit(0);
                break;

            case 1:  // list-file
                fs->listAll(); 
                break;
          
            case 2:    // format
                cin >> blockSize;
				cin >> direct_entries;
                fs->fsFormat(blockSize, direct_entries);
                cout << "FORMAT DISK: number of blocks: "<< fs->getNumOfBlocks() << endl;
                break;
          
            case 3:    // creat-file
                cin >> fileName;
                _fd = fs->CreateFile(fileName);
                if(_fd!=-1)
                    cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
            
            case 4:  // open-file
                cin >> fileName;
                _fd = fs->OpenFile(fileName);
                if(_fd!=-1)
                    cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
             
            case 5:  // close-file
                cin >> _fd;
                fileName = fs->CloseFile(_fd); 
                if(fileName!="-1")
                    cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
           
            case 6:   // write-file
                cin >> _fd;
                cin >> str_to_write;
                fs->WriteToFile( _fd , str_to_write , strlen(str_to_write));
                break;
          
            case 7:    // read-file
                cin >> _fd;
                cin >> size_to_read ;
                _fd=fs->ReadFromFile( _fd , str_to_read , size_to_read );
                if(_fd!=-1)   
                    cout << "ReadFromFile: " << str_to_read << endl;
                break;
           
            case 8:   // delete file 
                 cin >> fileName;
                _fd = fs->DelFile(fileName);
                if(_fd!=-1)
                    cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            default:
                break;
        }
    }

} 