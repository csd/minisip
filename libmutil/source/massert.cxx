
#include<libmutil/massert.h>

#include<config.h>

#include<string.h>
#include<iostream>
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
	std::string stackTrace = getStackTraceString();
	if (stackTrace.size()>0){
		std::cerr << "massert stack trace:"<<std::endl;
		std::cerr << stackTrace<<std::endl;
	}
	exit(1);
}

