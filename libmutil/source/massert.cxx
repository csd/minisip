
#include<libmutil/massert.h>

#include<config.h>

#include<string.h>
#include<stdio.h>
#include<libmutil/Exception.h>

void massertFailed(char *expr, char *file, char *baseFile, int line) {
	if (!strcmp(file, baseFile)) {
		printf("hello");
		fprintf(stderr,"massert(%s) failed in file %s, line %d\n", expr, file, line);
	} else {
		fprintf(stderr,"massert(%s) failed in file %s (included from %s), line %d\n", 
				expr, file, baseFile, line);
	}
	string stackTrace = getStackTraceString();
	if (stackTrace.size()>0){
		cerr << "massert stack trace:"<<endl;
		cerr << stackTrace<<endl;
	}
	exit(1);
}

