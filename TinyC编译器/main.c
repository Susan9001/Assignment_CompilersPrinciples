/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE // ��ǰ���ʷ�����
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE
/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE

#include "util.h"
#include "scan.h"
#include "analyze.h"
#include "code.h"
#include "cgen.h"

 /* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = TRUE; // �ʷ���������
int TraceParse = TRUE; // �﷨����
int TraceAnalyze = TRUE; 
int TraceCode = TRUE;

int Error = FALSE;

int main(int argc, char * argv[])
{
	TreeNode * syntaxTree;
	char pgm[120]; /* source code file name */

	if (argc != 2) {
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	strcpy(pgm, argv[1]);
	if (strchr(pgm, '.') == NULL)
		strcat(pgm, ".tny");
	source = fopen(pgm, "r");
	if (source == NULL) {
		fprintf(stderr, "File %s not found\n", pgm);
		exit(1);
	}
	//listing = stdout; /* send listing to screen */
    listing = fopen ("result.txt", "w");
	fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);
#if NO_PARSE
	while (getToken() != ENDFILE);           /* �ʷ��������� */
#else
	syntaxTree = parse();                  /* �﷨�������� */
	if (TraceParse) {
		fprintf(listing, "\nSyntax tree:\n");
		printTree(syntaxTree);
	}
#if !NO_ANALYZE                          /* ����������֣���ע�͵� */
	if (!Error) {
		if (TraceAnalyze) fprintf(listing, "\nBuilding Symbol Table...\n");
		buildSymtab(syntaxTree);
		if (TraceAnalyze) fprintf(listing, "\nChecking Types...\n");
		typeCheck(syntaxTree);
		if (TraceAnalyze) fprintf(listing, "\nType Checking Finished\n");
	}
#if !NO_CODE
	if (!Error) {
		char * codefile;
		int fnlen = strcspn(pgm, ".");
		codefile = (char *)calloc(fnlen + 4, sizeof(char));
		strncpy(codefile, pgm, fnlen);
		strcat(codefile, ".tm");
		code = fopen(codefile, "w");
		if (code == NULL) {
			printf("Unable to open %s\n", codefile);
			exit(1);
		}
		codeGen(syntaxTree, codefile);             /*  �������ɲ��֣���ע�͵� */
		fclose(code);
	}
#endif
#endif
#endif

    /* �ʷ��������� */
	while (getToken() != ENDFILE); 

	fclose(source);
	return 0;
}




