// to enable CCLog_()
//#define COCOS2D_DEBUG 1

#include "cocos2d.h"
#include "FileOperation.h"
#include <stdio.h>
#include <string>

#include "Settings.h"

using namespace std;

void FileOperation::saveFile()
{
    string path = getFilePath();
	CCLog_("saveFile, path = %s", path.c_str());
	
	FILE *fp = fopen(path.c_str(), "w");

	if (! fp)
	{
		CCLog_("can not create file %s", path.c_str());
		return;
	}

	fputs("file example", fp);
	fclose(fp);
}

void FileOperation::readFile()
{
    string path = getFilePath();
	CCLog_("write, path = %s", path.c_str());
	
	FILE *fp = fopen(path.c_str(), "r");
	char buf[50] = {0};

	if (! fp)
	{
		CCLog_("can not open file %s", path.c_str());
		return;
	}

	fgets(buf, 50, fp);
	CCLog_("read content %s", buf);

	fclose(fp);
}

string FileOperation::getFilePath()
{
	// save to document folder
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	string path = [documentsDirectory UTF8String];
	path.append( "/test.txt" );

	return path;
}