#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Base class for a file or directory. It holds anything that is common to files and folders.
class FSObject
{
public:
	FSObject(){
		name="";
	}
	virtual ~FSObject() {}
	string name;
	int size; // Only the name of the object, like, "Cloud", "Amazon", "Azure.txt", NOT the full path
};

// This class represents a directory. A directory can have sub-directories or files in it; it does not have any other attributes.
// TODO: Add necessary data structures to this class to implement the functionality needed from a directory object

class DataBlock{
	public:
	string content;
	int count_parent;
	DataBlock(){
		count_parent=0;
	}
};
// This class represents a file. A file has data and can have links.
// TODO: Add necessary data structures to this class to implement the functionality needed from a file object
class FSFile:public FSObject{
	public:
	string extension;
	DataBlock* link;
	
};
class FSDirectory:public FSObject{
	public:
	vector<FSDirectory *>children;
	vector<FSFile *>files;
};
FSDirectory* root;
// "CDrive" object becomes the globally available root of the file system
// TODO: Once you design the FSDirectory class, uncomment this line; if you uncomment it sooner, then there will be a compile error
//FSDirectory CDrive("C:");

vector<string> getPath(string fullpath){
	size_t pos = 0;
	std::string token;
	string delimiter = "\\";
	vector<string>directories;
	while ((pos = fullpath.find(delimiter)) != std::string::npos) {
		token = fullpath.substr(0, pos);
		directories.push_back(token);
		fullpath.erase(0, pos + delimiter.length());
	}
	return directories;
}
FSFile* getfile(string fullpath){
	vector<string>directories = getPath(fullpath);
	int flag;
	FSDirectory *temp =root;
	for(int i=0;i<directories.size()-1;i++){
		flag=0;
		for(int j=0;j<temp->children.size();j++){
			if(temp->children[j]->name==directories[i]){
				temp=temp->children[j];
				flag=1;
				break;
			}
		}
		if(flag==0){
			return NULL;
		}
	}
	for(int i=0;i<temp->files.size();i++){
		if(temp->files[i]->name == directories[directories.size()-1] ){
			return temp->files[i];
		}
	}
	return NULL;
}
FSDirectory* getfolder(string fullpath){
	vector<string>directories = getPath(fullpath);
	int flag;
	FSDirectory *temp =root;
	for(int i=0;i<directories.size()-1;i++){
		flag=0;
		for(int j=0;j<temp->children.size();j++){
			if(temp->children[j]->name==directories[i]){
				temp=temp->children[j];
				flag=1;
				break;
			}
		}
		if(flag==0){
			return NULL;
		}
	}
	for(int i=0;i<temp->files.size();i++){
		if(temp->files[i]->name == directories[directories.size()-1] ){
			return temp;
		}
	}
	return NULL;
}
// Given a full file path like "C:\Cloud\Cloud.txt" as input, create the additional directory and file objects and link it to the CDrive object
int CreateFile(const string& fullFilePath, const string& data)
{
	// Your code here
	FSDirectory *temp = root;
	string fullpath = fullFilePath+"\\";
	vector<string>directories = getPath(fullpath);
	int flag;
	for(int i=0;i<directories.size()-1;i++){
		flag=0;
		for(int j=0;j<temp->children.size();j++){
			if(temp->children[j]->name==directories[i]){
				temp=temp->children[j];
				flag=1;
				break;
			}
		}
		if(flag==0){
			FSDirectory *fsdirectory = new FSDirectory();
			fsdirectory->name = directories[i];
			fsdirectory->size = 0;
			temp->children.push_back(fsdirectory);
			temp=temp->children[temp->children.size()-1];
		}
	}
	for(int i=0;i<temp->files.size();i++){
		if(temp->files[i]->name==directories[directories.size()-1]){
			cout<<"File with the same name exists"<<endl;
			return 1;
		}
	}
	FSFile *fsfile = new FSFile() ;
	DataBlock *datablock = new DataBlock();
	datablock->content = data;
	datablock->count_parent+=1;
	fsfile->name = directories[directories.size()-1];
	size_t pos = directories[directories.size()-1].find('.');
	string extension = directories[directories.size()-1].substr(pos+1, directories[directories.size()-1].size()-1);
	fsfile->link = datablock;
	fsfile->extension = extension;
	temp->files.push_back(fsfile);
	return 0; // Return value of 0 means success
}

// Print the contents of an existing file like "C:\Cloud\Cloud.txt" to console
int ReadFile(const string& fullFilePath)
{
	// Your code here
	string fullpath = fullFilePath+"\\";
	FSFile* temp = getfile(fullpath);
	if(temp!=NULL){
		cout<<temp->link->content<<endl;
		return 0;
	}
	cout<<"Path or file doesn't exist"<<endl;
	return 1; // Return value of 0 means success
}

// Replace the contents of an existing file like "C:\Cloud\Cloud.txt" with the new data
int ReplaceFileContent(const string& fullFilePath, const string& newData)
{
	// Your code here
	FSFile* temp=getfile(fullFilePath+"\\");
	if(temp!=NULL){
		temp->link->content = newData;
		return 0;
	}
	cout<<"Path or file doesn't exist"<<endl;
	return 1; // Return value of 0 means success
}

// Given a full file path like "C:\Cloud\Cloud.txt" as input, delete the file object representing Cloud.txt; do not delete the ancestors
int DeleteFile(const string& fullFilePath)
{
	// Your code here
	string fullfp = fullFilePath;
	FSFile* file = getfile(fullfp+"\\");
	FSDirectory* folder = getfolder(fullfp+"\\");
	if(file!=NULL && folder!=NULL){
		for(int i=0;i<folder->files.size();i++){
			if(folder->files[i]->name==file->name){
				file->link->count_parent-=1;
				if(file->link->count_parent == 0){
					delete file->link;
				}
				delete file;
				folder->files.erase(folder->files.begin()+i);
			}
		}
		return 0;
	}
	cout<<"File not found"<<endl;
	return 1; // Return value of 0 means success
}

// If a new link to an existing file "C:\Cloud\Microsoft\VMware-Azure.txt" is being made at "C:\Cloud\Amazon\VMware-AWS.txt",
// then both 'share' the data; the new link will point to the data of the existing file.
int AddFileLink(const string& existingFilePath, const string& newLinkPath)
{
	// Your code here
	string existingfp = existingFilePath;
	string newfp = newLinkPath;
	CreateFile(newLinkPath,"");
	FSFile* temp = getfile(existingfp+"\\");
	FSFile* temp1 = getfile(newfp+"\\");
	if(temp1!=NULL && temp !=NULL){
		temp1->link = temp->link;
		temp->link->count_parent+=1;
		return 0; 
	}
	cout<<"Path doesn't exist"<<endl;
	return 1;// Return value of 0 means success
}

// An existing file like "C:\Cloud\Microsoft\Azure.txt" can be moved to different location on the file system, like "C:\Cloud\MSFT\Azure.txt"
int MoveFile(const string& existingFilePath, const string& newFilePath)
{
	// Your code here
	string existingfp = existingFilePath;
	FSFile* temp = getfile(existingfp+"\\");
	if(temp!=NULL){
		int val = CreateFile(newFilePath,temp->link->content);
		if(val==0){
			return DeleteFile(existingFilePath);
		}
		else{
			cout<<"Couldn't create a newfile"<<endl;
			return 1;
		}	
	}
	cout<<"File path doesn't exist"<<endl;
	return 1;
	// Return value of 0 means success
}

// Iterate over the classes / data structures of the file system represented by the root directory "CDrive", and print the full paths of files, one on each line, like
// C:\Cloud\Cloud.txt, "All organizations are moving to the cloud now"
// C:\Cloud\Microsoft\Azure.txt, "Azure is a Microsoft provided cloud"
// ...
void printGivenDirectory(FSDirectory* cur,vector<string>path){
	for(int i=0;i<cur->files.size();i++){
		for(int j=0;j<path.size();j++){
			cout<<path[j]<<"->";
		}
		cout<<cur->files[i]->name<<endl;
		cout<<"Content:"<<cur->files[i]->link->content<<endl;
		cout<<endl;
	}
	for(int i=0;i<cur->children.size();i++){
		path.push_back(cur->children[i]->name);
		printGivenDirectory(cur->children[i],path);
		path.pop_back();
	}	
}
void PrintAllFilesInCDrive()
{
	// Your code here
	FSDirectory *temp = root;
	vector<string>path;
	printGivenDirectory(temp,path);
}

int main(int argc, char ** argv)
{
	root = new FSDirectory;
	root->name = "root";
	root->size = 0;
	CreateFile("C:\\Cloud\\Cloud.txt", "Cloud is gaining importance these days");
	ReadFile("C:\\Cloud\\Cloud.txt");
	

	AddFileLink("C:\\Cloud\\Cloud.txt", "C:\\links\\link.txt");
	ReadFile("C:\\Cloud\\Cloud.txt");
	ReadFile("C:\\links\\link.txt");
	PrintAllFilesInCDrive();
	
	DeleteFile("C:\\Cloud\\Cloud.txt");
	ReadFile("C:\\links\\link.txt");
	PrintAllFilesInCDrive();

	// Section 1: Create, read, update and delete file
	//CreateFile("C:\\Cloud\\Cloud.txt", "Cloud is gaining importance these days");
	// CreateFile("C:\\Cloud\\Cloud.txt", "Cloud is gaining importance these days");
	// ReadFile("C:\\Cloud\\Cloud.txt");
	// CreateFile("C:\\Cloud\\Cloud.txt", "Cloud is gaining importance these days 2");
	// ReadFile("C:\\Cloud\\Cloud.txt");
	
	// ReplaceFileContent("C:\\Cloud\\Cloud.txt", "All organizations are moving to the cloud now");
	// ReadFile("C:\\Cloud\\Cloud.txt");
	
	// DeleteFile("C:\\Cloud\\Cloud.txt");
	// ReadFile("C:\\Cloud\\Cloud.txt");
	
	// CreateFile("C:\\azure\\readme.txt", "Cloud is gaining importance these days");
	// PrintAllFilesInCDrive();
	
	// cout << "==========================="<<endl;
	
	// MoveFile("C:\\azure\\readme.txt", "C:\\Cloud\\azure\\Azure.txt");
	// ReadFile("C:\\azure\\readme.txt");
	// ReadFile("C:\\Cloud\\azure\\Azure.txt");
	
	// DeleteFile("C:\\Cloud\\azure\\Azure.txt");
	// ReadFile("C:\\azure\\readme.txt");
	// ReadFile("C:\\Cloud\\azure\\Azure.txt");
	// PrintAllFilesInCDrive();
	
	// cout << "#########################";
	
	// AddFileLink("C:\\azure\\readme.txt", "C:\\links\\link.txt");
	// ReadFile("C:\\azure\\readme.txt");
	// cout << "Reading link file #############";
	// ReadFile("C:\\links\\link.txt");
	// PrintAllFilesInCDrive();
	
	// DeleteFile("C:\\azure\\readme.txt");
	// ReadFile("C:\\links\\link.txt");
	
	// DeleteFile("C:\\links\\link.txt");
	// ReadFile("C:\\links\\link.txt");
	// PrintAllFilesInCDrive();



	/*ReplaceFileContent("C:\\Cloud\\Cloud.txt", "All organizations are moving to the cloud now");
	DeleteFile("C:\\Cloud\\Cloud.txt");

	PrintAllFilesInCDrive();

	CreateFile("C:\\Cloud\\Microsoft\\Azure.txt", "Azure is a Microsoft provided cloud");
	CreateFile("C:\\Cloud\\Amazon\\AWS.txt", "AWS is an Amazon provided cloud");

	// Section 2: Add file link
	CreateFile("C:\\Cloud\\Microsoft\\VMware-Azure.txt", "VMware integrates with both, Azure and AWS");
	ReadFile("C:\\Cloud\\Microsoft\\VMware-Azure.txt");
	cout<<"SECTION 2"<<endl;
	AddFileLink("C:\\Cloud\\Microsoft\\VMware-Azure.txt", "C:\\Cloud\\Amazon\\VMware-AWS.txt");
	ReadFile("C:\\Cloud\\Microsoft\\VMware-Azure.txt");
	cout<<"SECTION 3"<<endl;
	ReplaceFileContent("C:\\Cloud\\Microsoft\\VMware-AWS.txt", "Many vendors are integrating with multiple cloud providers");
	ReadFile("C:\\Cloud\\Microsoft\\VMware-Azure.txt");
	ReadFile("C:\\Cloud\\Microsoft\\VMware-AWS.txt");
	cout<<"SECTION 4"<<endl;
	DeleteFile("C:\\Cloud\\Microsoft\\VMware-Azure.txt");
	DeleteFile("C:\\Cloud\\Amazon\\VMware-AWS.txt");
	PrintAllFilesInCDrive();
	// Section 3: Move file
	MoveFile("C:\\Cloud\\Microsoft\\Azure.txt", "C:\\Cloud\\MSFT\\Azure.txt");*/
	return 0;
}
